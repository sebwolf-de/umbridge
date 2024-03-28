#include "Worker.h"

#include "WorkerList.h"

void umbridge::Worker::processRequest(std::shared_ptr<umbridge::Request> r) {
  occupied = true;
  r->state = Request::RequestState::Processing;
  std::cout << "Process request on " << url << ". This might take a while." << std::endl;
  umbridge::HTTPModel client(url, "forward");

  const std::vector<std::vector<double>> outputs = client.Evaluate(r->input, r->config);
  r->output.clear();
  for (const auto& output : outputs) {
    r->output.push_back(output);
  }
  r->state = Request::RequestState::Finished;
  if (!cv.expired()) {
    cv.lock()->notify_all();
  }
  occupied = false;
}
