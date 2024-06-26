#include "QueuingModel.h"

#include "spdlog/spdlog.h"

void umbridge::QueuingModel::waitUntilJobFinished(const Request::RequestState& lock) {
  std::unique_lock lk(queueMutex);
  spdlog::debug("Waiting for evaluation ...");
  requestFinished->wait(lk, [&lock] { return lock == Request::RequestState::Finished; });
  spdlog::debug("...finished waiting.");
}

void umbridge::QueuingModel::processQueue(std::shared_ptr<QueuingModel>& qm) {
  while (true) {
    std::unique_lock lockJobs(JobQueue::jobsMutex);
    if (!qm->q.empty()) {
      const std::unique_lock<std::mutex> lockWorkers(umbridge::WorkerList::workersMutex);
      std::shared_ptr<Worker> availableWorker = qm->wl.getFreeWorker();
      if (availableWorker != nullptr) {
        std::shared_ptr<Request> r = qm->q.firstWaiting();
        availableWorker->cv = requestFinished;
        availableWorker->occupied = true;
        std::thread t(Worker::process, availableWorker, r);
        t.detach();
      }
    }
    queuesChanged->wait(lockJobs);
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
    std::unique_lock lockJobs(JobQueue::jobsMutex);
    q.push(r);
  }
  // Notify queuesChanged, because a new request has been submitted.
  queuesChanged->notify_all();
  std::thread t(waitUntilJobFinished, std::ref(r->state));
  t.join();

  // Notify queuesChanged, because a request has been finished.
  queuesChanged->notify_all();

  // Need look before return, because the return will delete the request.
  std::unique_lock lockJobs(JobQueue::jobsMutex);
  return r->output;
}

bool umbridge::QueuingModel::SupportsEvaluate() { return true; }

bool umbridge::QueuingModel::SupportsGradient() { return false; }

bool umbridge::QueuingModel::SupportsApplyJacobian() { return false; }

bool umbridge::QueuingModel::SupportsApplyHessian() { return false; }
