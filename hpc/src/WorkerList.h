#ifndef WORKERLIST_H
#define WORKERLIST_H

#include <cassert>
#include <memory>
#include <mutex>
#include <vector>

#include "Worker.h"

namespace umbridge {
class WorkerList {
  public:
  static std::mutex workersMutex;
  std::vector<std::shared_ptr<Worker>> workers;

  void add(const std::shared_ptr<Worker>& w);

  std::shared_ptr<Worker> getFreeWorker();
};
} // namespace umbridge
#endif
