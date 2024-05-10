#include "Worker.h"

#include "spdlog/spdlog.h"
#include "umbridge.h"

#include "WorkerList.h"

void umbridge::Worker::processRequest(std::shared_ptr<umbridge::Request> r) {
  {
    const std::unique_lock<std::mutex> lk(WorkerList::workersMutex);
    occupied = true;
    spdlog::debug("Worker {} now occupied.", id);
  }
  r->state = Request::RequestState::Processing;
  umbridge::HTTPModel client(url, "forward");

  const std::vector<std::vector<double>> outputs = client.Evaluate(r->input, r->config);
  r->output.clear();
  for (const auto& output : outputs) {
    r->output.push_back(output);
  }
  r->state = Request::RequestState::Finished;
  {
    const std::unique_lock<std::mutex> lk(WorkerList::workersMutex);
    occupied = false;
    spdlog::debug("Worker {} now free.", id);
  }
  if (!cv.expired()) {
    cv.lock()->notify_all();
  }
}
