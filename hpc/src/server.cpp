#include <array>
#include <fstream>
#include <string>
#include <vector>

#include <limits.h>
#include <mpi.h>
#include <unistd.h>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

void writeServer(const std::string& filename,
                 std::vector<std::array<char, HOST_NAME_MAX>>& allHostnames,
                 int from,
                 int to) {
  std::ofstream file(filename);
  if (file.is_open()) {
    for (int i = from; i < to; i++) {
      file << allHostnames.at(i).data() << std::endl;
    }
    file.close();
  } else {
    spdlog::error("Unable to open file {}.", filename);
  }
}

int main(int argc, char** argv) {
  assert(argc == 3);
  const int numberOfServers = std::atoi(argv[1]);
  const int ranksPerServer = std::atoi(argv[2]);

  MPI_Init(&argc, &argv);
  int worldRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
  int worldSize;
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
  if (numberOfServers * ranksPerServer != worldSize) {
    spdlog::error("I am supposed to run {} servers, each with {} ranks. Expected {} ranks, but got {}. Aborting now.",
                  numberOfServers,
                  ranksPerServer,
                  numberOfServers * ranksPerServer,
                  worldSize);
    MPI_Finalize();
    return -1;
  }
  std::array<char, HOST_NAME_MAX> myHostname;
  gethostname(myHostname.data(), HOST_NAME_MAX);
  std::vector<std::array<char, HOST_NAME_MAX>> allHostnames(worldSize);
  MPI_Allgather(myHostname.data(),
                HOST_NAME_MAX,
                MPI_CHAR,
                allHostnames.data()->data(),
                HOST_NAME_MAX,
                MPI_CHAR,
                MPI_COMM_WORLD);

  if (worldRank % ranksPerServer == 0) {
    spdlog::info("I will host a server on {}", myHostname.data());

    const int serverId = worldRank / ranksPerServer;
    const std::string serverFile = std::string(get_current_dir_name()) + "/servers/server-" +
                                   std::to_string(serverId) + ".txt";

    writeServer(serverFile, allHostnames, worldRank, worldRank + ranksPerServer);
    setenv("MACHINE_FILE", serverFile.c_str(), 1);
    setenv("SERVER_ID", std::to_string(serverId).c_str(), 1);

    const std::string command = "job_scripts/job.sh 2>&1";
    FILE* outPipe = popen(command.c_str(), "r");

    std::string logFile;
    auto slurmJobId = getenv("SLURM_JOBID");
    if (slurmJobId != nullptr) {
      logFile = std::string(get_current_dir_name()) + "/logs/job-" + std::string(slurmJobId) + "_server-" + std::to_string(serverId) + ".log";
    } else  {
      logFile = std::string(get_current_dir_name()) + "/logs/server-" + std::to_string(serverId) + ".log";
    }
    
    auto logger = spdlog::basic_logger_mt("SeisSol-Logger", logFile);
    logger->set_pattern("%v");  

    std::array<char, 512> outputBuffer;
    while (fgets(outputBuffer.data(), 512, outPipe) != nullptr) {
        std::string message(outputBuffer.data());
        message.erase(std::remove(message.begin(), message.end(), '\n'), message.cend());
        if (message.rfind("Started server successfully", 0) == 0) {
          spdlog::info(message);
        }
        logger->info(message);
        logger->flush();
    }
  } else {
    spdlog::info("I will idle");
  }
  MPI_Finalize();
  return 0;
}
