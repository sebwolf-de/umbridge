#include "WorkerList.h"

#include <memory>
#include <mutex>

#include "Worker.h"

void umbridge::WorkerList::add(const std::shared_ptr<Worker>& w) {
  const std::unique_lock<std::mutex>(m);
  workers.push_back(w);
}

std::shared_ptr<umbridge::Worker> umbridge::WorkerList::getFreeWorker() {
  std::shared_ptr<Worker> freeWorker = nullptr;
  for (const auto& w : workers) {
    if (!w->occupied) {
      freeWorker = w;
    }
  }
  return freeWorker;
}
