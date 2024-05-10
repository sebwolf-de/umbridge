#include "LoadBalancer.h"

#include <filesystem>
#include <fstream>
#include <string>

#include "spdlog/spdlog.h"

// run and get the result of command
std::string getCommandOutput(const std::string& command) {
  FILE* pipe = popen(command.c_str(),
                     "r"); // execute the command and return the output as stream
  if (pipe == nullptr) {
    spdlog::error("Failed to execute the command: {}.", command);
    return "";
  }

  std::array<char, 128> buffer;
  std::string output;
  while (fgets(buffer.data(), 128, pipe) != nullptr) {
    output += buffer.data();
  }
  pclose(pipe);

  return output;
}

// wait until file is created
bool waitForFile(const std::string& filename) {
  // Check if the file exists
  while (!std::filesystem::exists(filename)) {
    // If the file doesn't exist, wait for a certain period
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return true;
}

std::string readUrl(const std::string& filename) {
  std::ifstream file(filename);
  std::string url;
  if (file.is_open()) {
    const std::string fileContents((std::istreambuf_iterator<char>(file)),
                                   (std::istreambuf_iterator<char>()));
    url = fileContents;
    file.close();
  } else {
    spdlog::error("Unable to open file {}.", filename);
  }

  // delete the line break
  if (!url.empty()) {
    url.pop_back();
  }

  return url;
}

void umbridge::LoadBalancer::queryUrls(int numberOfWorkers) {
  for (int i = 0; i < numberOfWorkers; i++) {
    std::string jobId = std::to_string(i);

    // Also wait until job is running and url file is written
    spdlog::info("Waiting for job {}.", jobId);
    const std::string urlFile = "./urls/url-" + jobId + ".txt";
    waitForFile(urlFile);

    const std::string url = readUrl(urlFile);
    spdlog::info("Job {} started, running on {}.", jobId, url);
    wl.add(std::make_shared<Worker>(url, i));
  }
}
