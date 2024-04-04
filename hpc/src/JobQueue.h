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
  std::vector<std::weak_ptr<Request>> requests;

  public:
  static std::mutex jobsMutex;
  static std::mutex vectorMutex;
  JobQueue() = default;

  void push(std::shared_ptr<Request> r);
  [[nodiscard]] std::shared_ptr<Request> firstWaiting() const;
  [[nodiscard]] bool empty() const;
  [[nodiscard]] unsigned countWaiting() const;
};

} // namespace umbridge
#endif
