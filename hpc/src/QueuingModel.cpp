#include "QueuingModel.h"
void umbridge::QueuingModel::wait(const Request::JobState& lock) {
  std::unique_lock<std::mutex> lk(m);
  std::cerr << "Waiting for evaluation ..." << std::endl;
  cv.wait(lk, [&lock] { return lock == Request::JobState::Finished; });
  std::cerr << "...finished waiting." << std::endl;
}

void umbridge::QueuingModel::processQueue(QueuingModel* qm) {
  while (true) {
    if (!qm->q.empty()) {
      std::shared_ptr<Worker> availableWorker = qm->wl.getFreeWorker();
      const std::unique_lock<std::mutex> lk(umbridge::WorkerList::m);
      if (availableWorker != nullptr) {
        const std::shared_ptr<Request> r = qm->q.firstWaiting();
        availableWorker->cv = &cv;
        std::thread t(Worker::process, std::ref(availableWorker), r.get());
        t.detach();
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}

std::vector<std::size_t> umbridge::QueuingModel::GetInputSizes(const json& config) const {
  return {numberOfInputs};
}

std::vector<std::size_t> umbridge::QueuingModel::GetOutputSizes(const json& config) const {
  return {numberOfOutputs};
}

std::vector<std::vector<double>>
    umbridge::QueuingModel::Evaluate(const std::vector<std::vector<double>>& inputs, json config) {
  const std::shared_ptr<Request> r = std::make_shared<Request>(inputs, config);
  q.push(r);
  std::thread t(wait, std::ref(r->state));
  t.join();

  return r->output;
}

bool umbridge::QueuingModel::SupportsEvaluate() { return true; }

bool umbridge::QueuingModel::SupportsGradient() { return false; }

bool umbridge::QueuingModel::SupportsApplyJacobian() { return false; }

bool umbridge::QueuingModel::SupportsApplyHessian() { return false; }
