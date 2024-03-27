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

  void add(const std::shared_ptr<Worker>& w) {
    const std::unique_lock<std::mutex>(m);
    workers.push_back(w);
  }

  std::shared_ptr<Worker> getFreeWorker() {
    std::shared_ptr<Worker> freeWorker = nullptr;
    for (const auto& w : workers) {
      if (!w->occupied) {
        freeWorker = w;
      }
    }
    return freeWorker;
  }
};

} // namespace umbridge
#endif
