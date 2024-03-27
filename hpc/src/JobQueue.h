#ifndef JOBQUEUE_H
#define JOBQUEUE_H

#include <cassert>
#include <iostream>
#include <vector>

#include <umbridge.h>

#include "Request.h"

namespace umbridge {
class JobQueue {
  public:
  private:
  static std::mutex m;
  std::vector<std::shared_ptr<Request>> requests;

  public:
  JobQueue(){};

  void push(std::shared_ptr<Request> r) {
    const std::unique_lock<std::mutex> lk(m);
    requests.push_back(r);
    std::cout << "Pushed request, now " << countWaiting() << " models are waiting." << std::endl;
  }

  std::shared_ptr<Request> firstWaiting() {
    const std::unique_lock<std::mutex> lk(m);
    for (unsigned i = 0; i < requests.size(); i++) {
      std::shared_ptr<Request> r = requests.at(i);
      if (r == nullptr) {
        continue; 
      }
      if (r->state == Request::JobState::Waiting) {
        return r;
      }
    }
    return nullptr;
  }

  bool empty() {
    return firstWaiting() == nullptr;
  }
 
  unsigned countWaiting() {
    unsigned counter = 0;
    for (unsigned i = 0; i < requests.size(); i++) {
      std::shared_ptr<Request> r = requests.at(i);
      if (r == nullptr) {
        continue; 
      }
      if (r->state == Request::JobState::Waiting) {
        counter ++;
      }
    }
    return counter;
  }

};

}
#endif
