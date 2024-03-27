#include "Worker.h"

#include "WorkerList.h"

void umbridge::Worker::processRequest(umbridge::Request* r) {
  occupied = true;
  r->state = Request::JobState::Processing;
  std::cout << "Process request on " << url << ". This might take a while." << std::endl;
  umbridge::HTTPModel client(url, "forward");

  const std::vector<std::vector<double>> outputs = client.Evaluate(r->input, r->config);
  r->output.clear();
  for (const auto& output : outputs) {
    r->output.push_back(output);
  }
  r->state = Request::JobState::Finished;
  if (cv != nullptr) {
    cv->notify_all();
  }
  occupied = false;
}
