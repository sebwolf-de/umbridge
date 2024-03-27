#ifndef JOBQUEUE_H
#define JOBQUEUE_H

#include <cassert>
#include <iostream>
#include <vector>

#include <umbridge.h>

#include "Request.h"

namespace umbridge {
class JobQueue {
  private:
  static std::mutex m;
  std::vector<std::shared_ptr<Request>> requests;

  public:
  JobQueue() = default;

  void push(const std::shared_ptr<Request>& r);
  std::shared_ptr<Request> firstWaiting() const;
  bool empty() const;
  unsigned countWaiting() const;
};

} // namespace umbridge
#endif
