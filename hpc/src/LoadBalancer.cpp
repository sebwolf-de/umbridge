#include "LoadBalancer.hpp"

#include <filesystem>
#include <string>

// run and get the result of command
std::string getCommandOutput(const std::string command)
{
    FILE *pipe = popen(command.c_str(), "r"); // execute the command and return the output as stream
    if (!pipe)
    {
        std::cerr << "Failed to execute the command: " + command << std::endl;
        return "";
    }

    char buffer[128];
    std::string output;
    while (fgets(buffer, 128, pipe))
    {
        output += buffer;
    }
    pclose(pipe);

    return output;
}

// wait until file is created
bool waitForFile(const std::string &filename)
{
    // Check if the file exists
    while (!std::filesystem::exists(filename)) {
        // If the file doesn't exist, wait for a certain period
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return true;
}

std::string readUrl(const std::string &filename)
{
    std::ifstream file(filename);
    std::string url;
    if (file.is_open())
    {
        std::string file_contents((std::istreambuf_iterator<char>(file)),
                                  (std::istreambuf_iterator<char>()));
        url = file_contents;
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file " << filename << " ." << std::endl;
    }

    // delete the line break
    if (!url.empty())
        url.pop_back();

    return url;
}

    // state = ["WAITING", "RUNNING", "FINISHED", "CANCELED"]
    bool waitForHQJobState(const std::string &job_id, const std::string &state)
    {
        const std::string command = "hq job info " + job_id + " | grep State | awk '{print $4}'";
        // std::cout << "Checking runtime: " << command << std::endl;
        std::string job_status;

        do
        {
            job_status = getCommandOutput(command);

            // Delete the line break
            if (!job_status.empty())
                job_status.pop_back();

            // Don't wait if there is an error or the job is ended
            if (job_status == "" || (state != "FINISHED" && job_status == "FINISHED") || job_status == "FAILED" || job_status == "CANCELED")
            {
                std::cerr << "Wait for job status failure, status : " << job_status << std::endl;
                return false;
            }

            sleep(1);
        } while (job_status != state);

        return true;
    }
   
void umbridge::LoadBalancer::submitHQJob(const std::string &model_name, bool force_default_submission_script) {
        // Use model specific job script if available, default otherwise.
        const std::filesystem::path submission_script_dir("./hq_scripts");
        const std::filesystem::path submission_script_generic("job.sh");
        const std::filesystem::path submission_script_model_specific("job_" + model_name + ".sh");

        std::string hq_command = "hq submit --output-mode=quiet ";
        if (std::filesystem::exists(submission_script_dir / submission_script_model_specific) && !force_default_submission_script)
        {
            hq_command += (submission_script_dir / submission_script_model_specific).string();
        }
        else if (std::filesystem::exists(submission_script_dir / submission_script_generic)) 
        {
            hq_command += (submission_script_dir / submission_script_generic).string();
        }
        else
        {
            throw std::runtime_error("Job submission script not found: Check that file 'hq_script/job.sh' exists.");
        }
        std::cout << hq_command << std::endl;

        // Submit the HQ job and retrieve the HQ job ID.
        std::string job_id = getCommandOutput(hq_command);

        // Delete the line break.
        if (!job_id.empty())
        {
            job_id.pop_back();
        }

        std::cout << "Waiting for job " << job_id << " to start." << std::endl;

        // Wait for the HQ Job to start
        waitForHQJobState(job_id, "RUNNING");

        // Also wait until job is running and url file is written
        std::string url_file = "./urls/url-" + job_id + ".txt";
        waitForFile(url_file);

        std::string url = readUrl(url_file);
        std::cout << "Job " << job_id << " started, running on " << url <<  std::endl;
        wl.add(std::make_shared<Worker>(url));
    }

    void umbridge::LoadBalancer::startServer() {
      std::system("hq server stop &> /dev/null");
      std::filesystem::remove_all("./urls");

      std::system("hq server start &");
      sleep(1); // Workaround: give the HQ server enough time to start.

      // Create HQ allocation queue
      std::system("hq_scripts/allocation_queue.sh");
    }

    void umbridge::LoadBalancer::stopServer() {
      std::system("hq server stop");
    }

umbridge::LoadBalancer::LoadBalancer() {
  startServer();
}

umbridge::LoadBalancer::~LoadBalancer() {
  stopServer();
}
