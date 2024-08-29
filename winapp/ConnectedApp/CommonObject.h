#pragma once

#include <atomic>
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "nlohmann/json.hpp"

// Task struct definition
struct Task {
    std::string name;
    char priority;  // 1: urgent and important, 2: urgent and not important, 3: not urgent and important, 4: not urgent and not important
    bool done;
    bool show_details = false; // Flag to track if details should be shown
    std::string details; // String to hold task details
    int id; // Unique identifier for each task

    // Serialization and Deserialization
    friend void to_json(nlohmann::json& j, const Task& task) {
        j = nlohmann::json{ {"name", task.name}, {"priority", task.priority}, {"done", task.done}, {"show_details", task.show_details}, {"details", task.details}, {"id", task.id} };
    }

    friend void from_json(const nlohmann::json& j, Task& task) {
        j.at("name").get_to(task.name);
        j.at("priority").get_to(task.priority);
        j.at("done").get_to(task.done);
        j.at("show_details").get_to(task.show_details);
        j.at("details").get_to(task.details);
        j.at("id").get_to(task.id);
    }
};

// Define a type alias for JSON to avoid typing the full namespace
using json = nlohmann::json;

struct CommonObjects {
    std::atomic_bool exit_flag = false;
    std::atomic_bool start_download = false;
    std::atomic_bool data_ready = false;
    std::atomic_bool sync_needed = false;
    std::string url;
    std::unordered_map<int, Task> task_map;
    int next_task_id = 0;
 

       // File paths
       const std::string TASKS_FILE_PATH = "tasks.json";

        //Functions for file operations
       void SaveTasksToFile() {
           json json_tasks;
           for (const auto& pair : task_map) {
               json_tasks[pair.first] = pair.second;
           }

           std::ofstream file(TASKS_FILE_PATH);
           if (file.is_open()) {
               file << json_tasks.dump(4);
               file.close();
           }
           else {
               std::cerr << "\nFailed to open file for saving tasks." << std::endl;
           }
       }

       void LoadTasksFromFile() {
           if (std::filesystem::exists(TASKS_FILE_PATH)) {
               std::ifstream file(TASKS_FILE_PATH);
               if (file.is_open()) {
                   json json_tasks;
                   file >> json_tasks;
                   file.close();

                   task_map.clear();
                   for (const auto& item : json_tasks.items()) {
                       Task task = item.value();
                       task_map[task.id] = task;
                   }

                   if (!task_map.empty()) {
                       auto max_elem = std::max_element(task_map.begin(), task_map.end(), [](const auto& a, const auto& b) {
                           return a.first < b.first;
                           });
                       next_task_id = max_elem->first + 1;
                       data_ready = true;
                   }
                   else {
                       next_task_id = 0;
                   }

                   std::cout << "Tasks loaded from file successfully ;)" << std::endl;
               }
               else {
                   std::cerr << "Failed to open file for loading tasks." << std::endl;
               }
           }
           else {
               std::cout << "No tasks file found, starting fresh %)" << std::endl;
           }
       }
};