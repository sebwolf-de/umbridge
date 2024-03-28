#include "JobQueue.h"

void umbridge::JobQueue::push(const std::shared_ptr<Request>& r) {
  requests.push_back(r);
  std::cout << "Pushed request, now " << countWaiting() << " models are waiting." << std::endl;
}

std::shared_ptr<umbridge::Request> umbridge::JobQueue::firstWaiting() const {
  for (auto r : requests) {
    if (r == nullptr) {
      continue;
    }
    if (r->state == Request::RequestState::Waiting) {
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
    if (r->state == Request::RequestState::Waiting) {
      counter++;
    }
  }
  return counter;
}
