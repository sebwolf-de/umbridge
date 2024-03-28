#include "JobQueue.h"

#include "spdlog/spdlog.h"

std::mutex umbridge::JobQueue::vectorMutex;

void umbridge::JobQueue::push(std::shared_ptr<Request> r) {
  std::unique_lock<std::mutex> lk(vectorMutex);
  requests.push_back(r);
  spdlog::info("Pushed request, now {} models are waiting.", countWaiting());
}

std::shared_ptr<umbridge::Request> umbridge::JobQueue::firstWaiting() const {
  std::unique_lock<std::mutex> lk(vectorMutex);
  for (auto r : requests) {
    if (r.expired()) {
      continue;
    } else {
      std::shared_ptr<umbridge::Request> candidate = r.lock();
      if (candidate->state == Request::RequestState::Waiting) {
        return candidate;
      }
    }
  }
  return nullptr;
}

bool umbridge::JobQueue::empty() const { return firstWaiting() == nullptr; }

unsigned umbridge::JobQueue::countWaiting() const {
  unsigned counter = 0;
  for (const auto& r : requests) {
    if (r.expired()) {
      continue;
    }
    if (r.lock()->state == Request::RequestState::Waiting) {
      counter++;
    }
  }
  return counter;
}
