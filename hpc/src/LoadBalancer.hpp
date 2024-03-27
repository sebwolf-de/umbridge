#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include <string>
#include <vector>

#include <umbridge.h>

#include "WorkerList.h"

namespace umbridge {
  class LoadBalancer {
    private:
    void startServer();
    void stopServer();

    public:
    WorkerList wl;
    LoadBalancer();
    ~LoadBalancer();
    void submitHQJob(const std::string &model_name, bool force_default_submission_script=false);
  };
};

#endif
