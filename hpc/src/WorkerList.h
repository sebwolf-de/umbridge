#ifndef WORKERLIST_H
#define WORKERLIST_H

#include <cassert>
#include <iostream>
#include <vector>

#include <umbridge.h>

#include "Worker.h"

namespace umbridge {
class WorkerList {
  public:
  static std::mutex m;
  std::vector<std::shared_ptr<Worker>> workers;

  void add(const std::shared_ptr<Worker>& w);

  std::shared_ptr<Worker> getFreeWorker();
};
} // namespace umbridge
#endif
