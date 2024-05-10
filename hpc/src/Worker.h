#ifndef WORKER_H
#define WORKER_H

#include <condition_variable>
#include <memory>
#include <string>
#include <utility>

#include "Request.h"

namespace umbridge {

class WorkerList;

class Worker {
  public:
  static void process(std::shared_ptr<Worker> w, std::shared_ptr<Request> r) {
    w->processRequest(r);
  }
  bool occupied{};
  int id;
  std::string url;
  std::weak_ptr<WorkerList> wl;
  std::weak_ptr<std::condition_variable> cv;

  Worker(std::string url, int id) : url(std::move(url)), id(id) {};

  void processRequest(std::shared_ptr<Request> r);
};

} // namespace umbridge
#endif
