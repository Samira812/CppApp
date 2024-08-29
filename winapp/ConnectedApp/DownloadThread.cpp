#include "DownloadThread.h" 
#define CPPHTTPLIB_OPENSSL_SUPPORT 
#include "httplib.h" 
#include "nlohmann/json.hpp" 
#include <algorithm> 

// JSON serialization for the Task struct 
//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Task, name, priority, done, details, id)

void DownloadThread::operator()(CommonObjects& common) {
    httplib::Client cli("https://dummyjson.com");

    while (!common.exit_flag) {
        if (common.sync_needed) {
            // Serialize tasks to JSON
            nlohmann::json json_tasks = common.task_map;
            auto json_string = json_tasks.dump();

            // Perform a POST request to upload tasks
            auto res = cli.Post("/tasks/upload", json_string, "application/json");
            if (res && res->status == 200) {
                std::cout << "Tasks synchronized successfully." << std::endl;
                common.sync_needed = false; // Reset sync flag after successful upload
            }
            else {
                std::cout << "Failed to synchronize tasks." << std::endl;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(5)); // Check every 5 seconds
    }

    // Perform a GET request to retrieve tasks
    auto res = cli.Get("/tasks");
    if (res && res->status == 200) {
        auto json_result = nlohmann::json::parse(res->body);
        auto json_tasks = json_result["tasks"].get<std::vector<Task>>();

        // Clear existing tasks in the map before adding new ones
        common.task_map.clear();

        // Populate the unordered_map directly from the downloaded tasks
        for (const auto& task : json_tasks) {
            common.task_map[task.id] = task; // Insert each task into the map using its ID
        }

        // Indicate that data is ready to be displayed if the map is not empty
        if (!common.task_map.empty()) {
            common.data_ready = true;
        }
    }
}

void DownloadThread::SetUrl(std::string_view new_url) {
    _download_url = new_url;
}