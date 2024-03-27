#ifndef REQUEST_H
#define REQUEST_H

#include <vector>

#include <httplib.h>
#include <json.hpp>

namespace umbridge {
class Request {
public:
  enum class JobState { Waiting, Processing, Finished };
  const std::vector<std::vector<double>> &input;
  const nlohmann::json &config;
  std::vector<std::vector<double>> output;
  JobState state;

  Request(const std::vector<std::vector<double>> &i, const nlohmann::json &c)
      : input(i), config(c), state(JobState::Waiting){};
};

} // namespace umbridge
#endif
