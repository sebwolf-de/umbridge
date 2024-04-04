#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include <string>
#include <vector>

#include <umbridge.h>

#include "WorkerList.h"

namespace umbridge {
class LoadBalancer {
  private:
  static void startServer();
  static void stopServer();

  public:
  WorkerList wl;
  LoadBalancer();
  ~LoadBalancer();
  // void submitHQJob(const std::string& modelName, bool forceDefaultSubmissionScript = false);
  void queryUrls(int numberOfWorkers);
};
}; // namespace umbridge

#endif
