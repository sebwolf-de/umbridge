#ifndef UMBRIDGE
#define UMBRIDGE

// #define LOGGINGS-

// Increase timeout to allow for long-running models.
// This should be (to be on the safe side) significantly greater than the
// maximum time your model may take
#define CPPHTTPLIB_READ_TIMEOUT_SECOND 60 * 60

#include <string>
#include <vector>

#include "json.hpp"
#include "httplib.h"

using json = nlohmann::json;

namespace umbridge {

class Model {
  protected:
  std::string name;

  public:
  Model(std::string name);

  virtual std::vector<std::size_t>
      GetInputSizes(const json& config_json = json::parse("{}")) const = 0;
  virtual std::vector<std::size_t>
      GetOutputSizes(const json& config_json = json::parse("{}")) const = 0;

  virtual std::vector<std::vector<double>> Evaluate(const std::vector<std::vector<double>>& inputs,
                                                    json config_json = json::parse("{}"));

  virtual std::vector<double> Gradient(unsigned int outWrt,
                                       unsigned int inWrt,
                                       const std::vector<std::vector<double>>& inputs,
                                       const std::vector<double>& sens,
                                       json config_json = json::parse("{}"));

  virtual std::vector<double> ApplyJacobian(unsigned int outWrt,
                                            unsigned int inWrt,
                                            const std::vector<std::vector<double>>& inputs,
                                            const std::vector<double>& vec,
                                            json config_json = json::parse("{}"));
  virtual std::vector<double> ApplyHessian(unsigned int outWrt,
                                           unsigned int inWrt1,
                                           unsigned int inWrt2,
                                           const std::vector<std::vector<double>>& inputs,
                                           const std::vector<double>& sens,
                                           const std::vector<double>& vec,
                                           json config_json = json::parse("{}"));

  virtual bool SupportsEvaluate();
  virtual bool SupportsGradient();
  virtual bool SupportsApplyJacobian();
  virtual bool SupportsApplyHessian();

  std::string GetName() const;

  std::vector<std::string> SupportedModels(std::string host,
                                           httplib::Headers headers = httplib::Headers());
};

// Client-side Model connecting to a server for the actual evaluations etc.
class HTTPModel : public Model {
  public:
  HTTPModel(std::string host, std::string name, httplib::Headers headers = httplib::Headers());

  std::vector<std::size_t>
      GetInputSizes(const json& config_json = json::parse("{}")) const override;

  std::vector<std::size_t>
      GetOutputSizes(const json& config_json = json::parse("{}")) const override;

  std::vector<std::vector<double>> Evaluate(const std::vector<std::vector<double>>& inputs,
                                            json config_json = json::parse("{}")) override;

  std::vector<double> Gradient(unsigned int outWrt,
                               unsigned int inWrt,
                               const std::vector<std::vector<double>>& inputs,
                               const std::vector<double>& sens,
                               json config_json = json::parse("{}")) override;

  std::vector<double> ApplyJacobian(unsigned int outWrt,
                                    unsigned int inWrt,
                                    const std::vector<std::vector<double>>& inputs,
                                    const std::vector<double>& vec,
                                    json config_json = json::parse("{}")) override;

  std::vector<double> ApplyHessian(unsigned int outWrt,
                                   unsigned int inWrt1,
                                   unsigned int inWrt2,
                                   const std::vector<std::vector<double>>& inputs,
                                   const std::vector<double>& sens,
                                   const std::vector<double>& vec,
                                   json config_json = json::parse("{}")) override;

  bool SupportsEvaluate() override;
  bool SupportsGradient() override;
  bool SupportsApplyJacobian() override;
  bool SupportsApplyHessian() override;

  private:
  mutable httplib::Client cli;
  httplib::Headers headers;

  bool supportsEvaluate = false;
  bool supportsGradient = false;
  bool supportsApplyJacobian = false;
  bool supportsApplyHessian = false;
};
json parse_result_with_error_handling(const httplib::Result& res);

// Check if inputs dimensions match model's expected input size and return error
// in httplib response
bool check_input_sizes(const std::vector<std::vector<double>>& inputs,
                       const json& config_json,
                       const Model& model,
                       httplib::Response& res);

// Check if sensitivity vector's dimension matches correct model output size and
// return error in httplib response
bool check_sensitivity_size(const std::vector<double>& sens,
                            int outWrt,
                            const json& config_json,
                            const Model& model,
                            httplib::Response& res);

// Check if vector's dimension matches correct model output size and return
// error in httplib response
bool check_vector_size(const std::vector<double>& vec,
                       int inWrt,
                       const json& config_json,
                       const Model& model,
                       httplib::Response& res);

// Check if outputs dimensions match model's expected output size and return
// error in httplib response
bool check_output_sizes(const std::vector<std::vector<double>>& outputs,
                        const json& config_json,
                        const Model& model,
                        httplib::Response& res);

// Check if inWrt is between zero and model's input size inWrt and return error
// in httplib response
bool check_input_wrt(int inWrt,
                     const json& config_json,
                     const Model& model,
                     httplib::Response& res);

// Check if outWrt is between zero and model's output size outWrt and return
// error in httplib response
bool check_output_wrt(int outWrt,
                      const json& config_json,
                      const Model& model,
                      httplib::Response& res);

// Construct response for unsupported feature
void write_unsupported_feature_response(httplib::Response& res, std::string feature);

// log  request
void log_request(const httplib::Request& req, const httplib::Response& res);

// Get model from name
Model& get_model_from_name(std::vector<Model*>& models, std::string name);

// Check if model exists and return error in httplib response
bool check_model_exists(std::vector<Model*>& models, std::string name, httplib::Response& res);

// Provides access to a model via network
void serveModels(std::vector<Model*> models,
                 std::string host,
                 int port,
                 bool enable_parallel = false,
                 bool error_checks = true);
} // namespace umbridge

#endif
