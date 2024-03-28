#include "QueuingModel.h"

void umbridge::QueuingModel::waitUntilJobFinished(const Request::RequestState& lock) {
  std::unique_lock lk(queueMutex);
  std::cerr << "Waiting for evaluation ..." << std::endl;
  requestFinished->wait(lk, [&lock] { return lock == Request::RequestState::Finished; });
  std::cerr << "...finished waiting." << std::endl;
}

void umbridge::QueuingModel::processQueue(std::shared_ptr<QueuingModel>& qm) {
  while (true) {
    std::unique_lock lk(JobQueue::jobsMutex);
    if (!qm->q.empty()) {
      std::shared_ptr<Worker> availableWorker = qm->wl.getFreeWorker();
      const std::unique_lock<std::mutex> lk(umbridge::WorkerList::workersMutex);
      if (availableWorker != nullptr) {
        std::shared_ptr<Request> r = qm->q.firstWaiting();
        availableWorker->cv = requestFinished;
        std::thread t(Worker::process, std::ref(availableWorker), std::ref(r));
        t.detach();
      }
    }
    queuesChanged->wait(lk);
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
  std::shared_ptr<Request> r = std::make_shared<Request>(inputs, config);
  {
    std::unique_lock(JobQueue::jobsMutex);
    q.push(r);
  }
  // Notify queuesChanged, because a new request has been submitted.
  queuesChanged->notify_all();
  std::thread t(waitUntilJobFinished, std::ref(r->state));
  t.join();

  // Notify queuesChanged, because a request has been finished.
  queuesChanged->notify_all();

  return r->output;
}

bool umbridge::QueuingModel::SupportsEvaluate() { return true; }

bool umbridge::QueuingModel::SupportsGradient() { return false; }

bool umbridge::QueuingModel::SupportsApplyJacobian() { return false; }

bool umbridge::QueuingModel::SupportsApplyHessian() { return false; }
