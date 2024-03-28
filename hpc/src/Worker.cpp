#include "Worker.h"

#include "spdlog/spdlog.h"

#include "WorkerList.h"

void umbridge::Worker::processRequest(std::shared_ptr<umbridge::Request> r) {
  {
    std::unique_lock<std::mutex> lk(WorkerList::workersMutex);
    occupied = true;
  }
  r->state = Request::RequestState::Processing;
  spdlog::info("Process request on {}. This might take a while.", url);
  umbridge::HTTPModel client(url, "forward");

  const std::vector<std::vector<double>> outputs = client.Evaluate(r->input, r->config);
  r->output.clear();
  for (const auto& output : outputs) {
    r->output.push_back(output);
  }
  r->state = Request::RequestState::Finished;
  if (!cv.expired()) {
    cv.lock()->notify_all();
  }
  {
    std::unique_lock<std::mutex> lk(WorkerList::workersMutex);
    occupied = false;
  }
}
