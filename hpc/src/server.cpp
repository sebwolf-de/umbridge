#include <fstream>
#include <array>
#include <string>
#include <vector>

#include <unistd.h>
#include <limits.h>
#include <mpi.h>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

void writeServer(const std::string& filename, std::vector<std::array<char, HOST_NAME_MAX>>& allHostnames, int from, int to)  {
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
    spdlog::error("I am supposed to run {} servers, each with {} ranks. Expected {} ranks, but got }. Aborting now", numberOfServers, ranksPerServer, numberOfServers*ranksPerServer, worldSize);
    return -1;
  }

  std::array<char, HOST_NAME_MAX> myHostname;
  gethostname(myHostname.data(), HOST_NAME_MAX);
  std::vector<std::array<char, HOST_NAME_MAX>> allHostnames(worldSize);
  MPI_Allgather(myHostname.data(), HOST_NAME_MAX, MPI_CHAR, allHostnames.data()->data(), HOST_NAME_MAX, MPI_CHAR, MPI_COMM_WORLD);

  if (worldRank % ranksPerServer == 0) {
    spdlog::info("I will host a server on {}", myHostname.data());

    int serverId = worldRank / ranksPerServer;
    const std::string serverFile = std::string(get_current_dir_name()) + "/servers/server-" + std::to_string(serverId) + ".txt";
    const std::string logFile = std::string(get_current_dir_name()) + "/logs/server-" + std::to_string(serverId) + ".log";
    auto logger = spdlog::basic_logger_mt("SeisSol-Logger", logFile);
    spdlog::set_default_logger(logger);

    writeServer(serverFile, allHostnames, worldRank, worldRank + ranksPerServer);
    setenv("MACHINE_FILE", serverFile.c_str(), 1);
    setenv("SERVER_ID", std::to_string(serverId).c_str(), 1);

    std::array<char, 512> outputBuffer;
    std::string command = "hq_scripts/job.sh 2&>1";
    FILE* pipe = popen(command.c_str(), "r");
    while (fgets(outputBuffer.data(), 512, pipe) != NULL) {
      spdlog::info(std::string(outputBuffer.data()));
    }
  } else {
    spdlog::info("I will idle");
  }
  MPI_Finalize(); return 0;
}
