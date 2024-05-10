#include "JobQueue.h"

#include <memory>
#include <mutex>

#include "spdlog/spdlog.h"

#include "Request.h"

std::mutex umbridge::JobQueue::vectorMutex;

void umbridge::JobQueue::push(std::shared_ptr<Request> r) {
  const std::unique_lock<std::mutex> lk(vectorMutex);
  requests.push_back(r);
  spdlog::info("Pushed request, now {} models are waiting, chain has size/capacity {}/{}.", countWaiting(), requests.size(), requests.capacity());
}

std::shared_ptr<umbridge::Request> umbridge::JobQueue::firstWaiting() const {
  const std::unique_lock<std::mutex> lk(vectorMutex);
  for (const auto& r : requests) {
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
