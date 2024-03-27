#ifndef REQUEST_H
#define REQUEST_H

#include <vector>

#include <json.hpp>
#include <httplib.h>

namespace umbridge{
class Request {
  public:
  enum class JobState {
    Waiting,
    Processing,
    Finished
  };
  const std::vector<std::vector<double>>& input;
  const nlohmann::json& config;
  std::vector<std::vector<double>> output;
  JobState state;

  Request(const std::vector<std::vector<double>>& i, const nlohmann::json& c)
      : input(i), config(c), state(JobState::Waiting) {};

};

}
#endif
