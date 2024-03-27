#ifndef WORKER_H
#define WORKER_H

#include <condition_variable>
#include <vector>
#include <chrono>
#include <thread>

#include "Request.h"

namespace umbridge {

class WorkerList;

class Worker {
  public:
  static void process(std::shared_ptr<Worker>& w, Request* r) {
    w->processRequest(r);
  }
  bool occupied{};
  std::string url;
  WorkerList* wl;
  std::condition_variable* cv;

  Worker(std::string url) : occupied(false), url(std::move(url)), wl(nullptr), cv(nullptr) {};

  void processRequest(Request* r);


};

}
#endif
