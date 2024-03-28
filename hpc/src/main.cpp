#include <iostream>

#include "LoadBalancer.h"

#include "QueuingModel.h"

std::condition_variable umbridge::QueuingModel::requestFinished;
std::condition_variable umbridge::QueuingModel::queuesChanged;
std::mutex umbridge::QueuingModel::queueMutex;
std::mutex umbridge::WorkerList::workersMutex;
std::mutex umbridge::JobQueue::jobsMutex;

constexpr unsigned NumberOfInputs = 1;
constexpr unsigned NumberOfOutputs = 1;
constexpr unsigned Port = 4343;

int main(int argc, char** argv) {
  assert(argc == 2);
  const int numberOfJobs = std::atoi(argv[1]);
  umbridge::LoadBalancer lb;
  for (int i = 0; i < numberOfJobs; i++) {
    lb.submitHQJob("");
  }
  umbridge::QueuingModel q("QueuingModel", NumberOfInputs, NumberOfOutputs, lb.wl);
  std::thread t(umbridge::QueuingModel::processQueue, &q);
  t.detach();
  const std::vector<umbridge::Model*> models = {&q};
  umbridge::serveModels(models, "0.0.0.0", Port, true, false);

  return 0;
}
