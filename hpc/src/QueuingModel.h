#ifndef QUEUINGMODEL_H
#define QUEUINGMODEL_H

#include <chrono>
#include <cmath>
#include <condition_variable>
#include <linux/limits.h>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include <umbridge.h>

#include "JobQueue.h"
#include "Request.h"
#include "WorkerList.h"
#include "Worker.h"

namespace umbridge {
class QueuingModel : public umbridge::Model { private:
  static std::mutex m;
  static std::condition_variable cv;
  const size_t numberOfInputs;
  const size_t numberOfOutputs;
  WorkerList& wl;
  JobQueue q;

  static void wait(const Request::JobState& lock) {
    std::unique_lock<std::mutex> lk(m);
    std::cerr << "Waiting for evaluation ..." << std::endl;
    cv.wait(lk, [&lock] { return lock == Request::JobState::Finished; });
    std::cerr << "...finished waiting." << std::endl;
  }
  
  public:
  static void processQueue(QueuingModel* qm) {
    while (true) {
      if (!qm->q.empty()) {
        std::shared_ptr<Worker> availableWorker = qm->wl.getFreeWorker();
        std::unique_lock<std::mutex> lk(qm->wl.m);
        if (availableWorker != nullptr) {
          std::shared_ptr<Request> r = qm->q.firstWaiting();
          availableWorker->cv = &cv;
          std::thread t(Worker::process, std::ref(availableWorker), r.get());
          t.detach();
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

  }

  QueuingModel(std::string name,
        size_t numberOfInputs,
        size_t numberOfOutputs,
        WorkerList& wl)
      : umbridge::Model(std::move(name)), numberOfInputs(numberOfInputs),
        numberOfOutputs(numberOfOutputs), wl(wl)  {
  }

  [[nodiscard]] std::vector<std::size_t>
      GetInputSizes(const json& config = json::parse("{}")) const override {
    return {numberOfInputs};
  }

  [[nodiscard]] std::vector<std::size_t>
      GetOutputSizes(const json& config = json::parse("{}")) const override {
    return {numberOfOutputs};
  }

  [[nodiscard]] std::vector<std::vector<double>>
      Evaluate(const std::vector<std::vector<double>>& inputs,
               json config = json::parse("{}")) override {
    std::shared_ptr<Request> r = std::make_shared<Request>(inputs, config);
    q.push(r);
    std::thread t(wait, std::ref(r->state));
    t.join();

    return r->output;
  }
  

  [[nodiscard]] bool SupportsEvaluate() override { return true; }

  [[nodiscard]] bool SupportsGradient() override { return false; }

  [[nodiscard]] bool SupportsApplyJacobian() override { return false; }

  [[nodiscard]] bool SupportsApplyHessian() override { return false; }
}; 
}
#endif
