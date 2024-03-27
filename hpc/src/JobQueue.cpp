#include "JobQueue.h"

void umbridge::JobQueue::push(const std::shared_ptr<Request>& r) {
  const std::unique_lock<std::mutex> lk(m);
  requests.push_back(r);
  std::cout << "Pushed request, now " << countWaiting() << " models are waiting." << std::endl;
}

std::shared_ptr<umbridge::Request> umbridge::JobQueue::firstWaiting() const {
  const std::unique_lock<std::mutex> lk(m);
  for (auto r : requests) {
    if (r == nullptr) {
      continue;
    }
    if (r->state == Request::JobState::Waiting) {
      return r;
    }
  }
  return nullptr;
}

bool umbridge::JobQueue::empty() const { return firstWaiting() == nullptr; }

unsigned umbridge::JobQueue::countWaiting() const {
  unsigned counter = 0;
  for (const auto& r : requests) {
    if (r == nullptr) {
      continue;
    }
    if (r->state == Request::JobState::Waiting) {
      counter++;
    }
  }
  return counter;
}
