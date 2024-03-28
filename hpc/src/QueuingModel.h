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
#include "Worker.h"
#include "WorkerList.h"

namespace umbridge {
class QueuingModel : public umbridge::Model {
  private:
  static std::mutex queueMutex;
  static std::shared_ptr<std::condition_variable> requestFinished;
  static std::shared_ptr<std::condition_variable> queuesChanged;
  const size_t numberOfInputs;
  const size_t numberOfOutputs;
  WorkerList& wl;
  JobQueue q;

  static void waitUntilJobFinished(const Request::RequestState& lock);

  public:
  static void processQueue(std::shared_ptr<QueuingModel>& qm);

  QueuingModel(std::string name, size_t numberOfInputs, size_t numberOfOutputs, WorkerList& wl)
      : umbridge::Model(std::move(name)), numberOfInputs(numberOfInputs),
        numberOfOutputs(numberOfOutputs), wl(wl) {}

  [[nodiscard]] std::vector<std::size_t>
      GetInputSizes(const json& config = json::parse("{}")) const override;

  [[nodiscard]] std::vector<std::size_t>
      GetOutputSizes(const json& config = json::parse("{}")) const override;

  [[nodiscard]] std::vector<std::vector<double>>
      Evaluate(const std::vector<std::vector<double>>& inputs,
               json config = json::parse("{}")) override;

  [[nodiscard]] bool SupportsEvaluate() override;

  [[nodiscard]] bool SupportsGradient() override;

  [[nodiscard]] bool SupportsApplyJacobian() override;

  [[nodiscard]] bool SupportsApplyHessian() override;
};
} // namespace umbridge
#endif
