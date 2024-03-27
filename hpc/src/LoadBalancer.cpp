#include "LoadBalancer.hpp"

#include <filesystem>
#include <string>

// run and get the result of command
std::string getCommandOutput(const std::string& command) {
  FILE* pipe = popen(command.c_str(),
                     "r"); // execute the command and return the output as stream
  if (pipe == nullptr) {
    std::cerr << "Failed to execute the command: " + command << std::endl;
    return "";
  }

  char buffer[128];
  std::string output;
  while (fgets(buffer, 128, pipe) != nullptr) {
    output += buffer;
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
    std::string const fileContents((std::istreambuf_iterator<char>(file)),
                                   (std::istreambuf_iterator<char>()));
    url = fileContents;
    file.close();
  } else {
    std::cerr << "Unable to open file " << filename << " ." << std::endl;
  }

  // delete the line break
  if (!url.empty()) {
    url.pop_back();
  }

  return url;
}

// state = ["WAITING", "RUNNING", "FINISHED", "CANCELED"]
bool waitForHQJobState(const std::string& jobId, const std::string& state) {
  const std::string command = "hq job info " + jobId + " | grep State | awk '{print $4}'";
  // std::cout << "Checking runtime: " << command << std::endl;
  std::string jobStatus;

  do {
    jobStatus = getCommandOutput(command);

    // Delete the line break
    if (!jobStatus.empty()) {
      jobStatus.pop_back();
    }

    // Don't wait if there is an error or the job is ended
    if (jobStatus.empty() || (state != "FINISHED" && jobStatus == "FINISHED") ||
        jobStatus == "FAILED" || jobStatus == "CANCELED") {
      std::cerr << "Wait for job status failure, status : " << jobStatus << std::endl;
      return false;
    }

    sleep(1);
  } while (jobStatus != state);

  return true;
}

void umbridge::LoadBalancer::submitHQJob(const std::string& modelName,
                                         bool forceDefaultSubmissionScript) {
  // Use model specific job script if available, default otherwise.
  const std::filesystem::path submissionScriptDir("./hq_scripts");
  const std::filesystem::path submissionScriptGeneric("job.sh");
  const std::filesystem::path submissionScriptModelSpecific("job_" + modelName + ".sh");

  std::string hqCommand = "hq submit --output-mode=quiet ";
  if (std::filesystem::exists(submissionScriptDir / submissionScriptModelSpecific) &&
      !forceDefaultSubmissionScript) {
    hqCommand += (submissionScriptDir / submissionScriptModelSpecific).string();
  } else if (std::filesystem::exists(submissionScriptDir / submissionScriptGeneric)) {
    hqCommand += (submissionScriptDir / submissionScriptGeneric).string();
  } else {
    throw std::runtime_error("Job submission script not found: Check that file "
                             "'hq_script/job.sh' exists.");
  }
  std::cout << hqCommand << std::endl;

  // Submit the HQ job and retrieve the HQ job ID.
  std::string jobId = getCommandOutput(hqCommand);

  // Delete the line break.
  if (!jobId.empty()) {
    jobId.pop_back();
  }

  std::cout << "Waiting for job " << jobId << " to start." << std::endl;

  // Wait for the HQ Job to start
  waitForHQJobState(jobId, "RUNNING");

  // Also wait until job is running and url file is written
  std::string const urlFile = "./urls/url-" + jobId + ".txt";
  waitForFile(urlFile);

  std::string const url = readUrl(urlFile);
  std::cout << "Job " << jobId << " started, running on " << url << std::endl;
  wl.add(std::make_shared<Worker>(url));
}

void umbridge::LoadBalancer::startServer() {
  std::system("hq server stop &> /dev/null");
  std::filesystem::remove_all("./urls");

  std::system("hq server start &");
  sleep(1); // Workaround: give the HQ server enough time to start.

  // Create HQ allocation queue
  std::system("hq_scripts/allocation_queue.sh");
}

void umbridge::LoadBalancer::stopServer() { std::system("hq server stop"); }

umbridge::LoadBalancer::LoadBalancer() { startServer(); }

umbridge::LoadBalancer::~LoadBalancer() { stopServer(); }
