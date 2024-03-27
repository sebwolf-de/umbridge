#ifndef WORKER_H
#define WORKER_H

#include <chrono>
#include <condition_variable>
#include <thread>
#include <vector>

#include "Request.h"

namespace umbridge {

class WorkerList;

class Worker {
  public:
  static void process(std::shared_ptr<Worker>& w, Request* r) { w->processRequest(r); }
  bool occupied{};
  std::string url;
  WorkerList* wl{nullptr};
  std::condition_variable* cv{nullptr};

  Worker(std::string url) : url(std::move(url)){};

  void processRequest(Request* r);
};

} // namespace umbridge
#endif
