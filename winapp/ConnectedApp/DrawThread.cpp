#include "DrawThread.h"
#include "../../shared/ImGuiSrc/imgui.h"


static int current_interface_state = 0; // 0 for intro, 1 for main interface

void DrawAppWindow(void* common_ptr) {
    auto common = static_cast<CommonObjects*>(common_ptr);

    // Introductory Interface
    if (current_interface_state == 0) {
        ImGui::Begin("Introduction", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
        // Center the window
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 window_pos = ImVec2((io.DisplaySize.x - 700) / 2, (io.DisplaySize.y - 500) / 2);  // Increased the window size
        ImGui::SetWindowPos(window_pos);

        // Draw the rectangle with the text
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Light green background
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));  // Black text color
        ImGui::BeginChild("TextChild", ImVec2(650, 200), true);  // Increased the size of the rectangle

        // Center text within the rectangle
        ImVec2 text_size = ImGui::CalcTextSize("Welcome to Eisenhower's Frog!\nManage your tasks effectively using the Eisenhower Matrix.\nClick the button to start.");
        ImGui::SetCursorPosX((650 - text_size.x) / 2);  // Adjusted width to match the rectangle size
        ImGui::SetCursorPosY((200 - text_size.y) / 2);  // Adjusted height to match the rectangle size

        ImGui::TextWrapped("Welcome to Eisenhower's Frog!\nManage your tasks effectively using the Eisenhower Matrix.\nClick the button to start.");
        ImGui::EndChild();
        ImGui::PopStyleColor(2);  // Pop the text color and child background color

        // Add a "let's get started" button with a white background color and black text
        ImGui::SetCursorPosX((650 - 80) / 2); // Center the button relative to the new rectangle size
        ImGui::SetCursorPosY(250); // Position it below the text box
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // White background color
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));  // Black text color
        if (ImGui::Button("let's get started!")) {
            current_interface_state = 1; // Move to the main interface
        }
        ImGui::PopStyleColor(2);  // Pop the text color and button background color

        ImGui::End();
    }
        
    // Main Interface
    else if (current_interface_state == 1) {
        // Main Task Management Interface
    // Define and initialize the variables
        static char task_name[128] = "";          // Buffer for task name
        static int selected_priority = 1;         // Selected priority
        static bool show_empty_name_warning = false; // Flag for empty name warning
        static bool show_duplicate_warning = false; // Flag for duplicate task warning
        static bool show_done_only = false;    // New variable to control showing only done tasks
        static bool show_not_done_only = false; // New variable to control showing only not done tasks
        static const char* priorities[] = {
           "1: Urgent and Important",
           "2: Urgent and Not Important",
           "3: Not Urgent and Important",
           "4: Not Urgent and Not Important"
        };

        ImGui::Begin("Eisenhower's Frog", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);// | ImGuiWindowFlags_NoResize );
        if (ImGui::Button("Add Task")) {
            ImGui::OpenPopup("Add New Task");
        }

        float availableWidth = ImGui::GetContentRegionAvail().x;

        // Define button sizes (adjust these as needed)
        float buttonPadding = 10.0f;  // Padding between buttons
        float buttonWidth = 150.0f;    // Assuming each button has the same width
        float totalButtonsWidth = (buttonWidth * 3) + (buttonPadding * 2); // 3 buttons + 2 paddings

        // Calculate start position for centering the buttons
        float startPosX = (availableWidth - totalButtonsWidth) / 2.0f;

        ImGui::SameLine();
        
        // Set cursor position for centered buttons
        ImGui::SetCursorPosX(startPosX);

     
        // Add buttons for filtering tasks
        if (show_done_only) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 1.0f, 0.6f, 1.0f));  // Light green color for "Show Done Tasks"
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Button]);
        }
        if (ImGui::Button("Show Done Tasks")) {
            show_done_only = true;
            show_not_done_only = false;
        }

        ImGui::PopStyleColor();

        ImGui::SameLine();
        if (show_not_done_only) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.6f, 0.6f, 1.0f));  // Light red color for "Show Not Done Tasks"
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Button]);// Default color
        }
        if (ImGui::Button("Show Not Done Tasks")) {
            show_done_only = false;
            show_not_done_only = true;
        }
        ImGui::PopStyleColor();


        ImGui::SameLine();
        if (!show_done_only && !show_not_done_only) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.6f, 1.0f, 1.0f));  // Light blue color for "Show All Tasks"
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Button]);
        }
        if (ImGui::Button("Show All Tasks")) {
            show_done_only = false;
            show_not_done_only = false;
        }
        ImGui::PopStyleColor();


        ImGui::SameLine();
        ImGui::SetCursorPosX(availableWidth - buttonWidth);
        if (ImGui::Button("Click to save to file", ImVec2(buttonWidth, 0))) {
            common->SaveTasksToFile();
        }

        if (ImGui::BeginPopupModal("Add New Task", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::InputText("Task Name", task_name, IM_ARRAYSIZE(task_name));
            ImGui::SliderInt("Priority", &selected_priority, 1, 4);

            if (ImGui::Button("Save")) {
                // Check if task name is empty
                if (strlen(task_name) == 0) {
                    show_empty_name_warning = true; // Set flag to show empty name warning
                    show_duplicate_warning = false; // Reset duplicate warning
                }
                else {
                    // Check for duplicates
                    bool duplicate_found = false;
                    for (const auto& pair : common->task_map) {
                        const Task& existing_task = pair.second;
                        if (existing_task.name == task_name && existing_task.priority == selected_priority) {
                            duplicate_found = true;
                            break;
                        }
                    }

                    if (duplicate_found) {
                        show_duplicate_warning = true;  // Set flag to show duplicate warning
                        show_empty_name_warning = false; // Reset empty name warning
                    }
                    else {
                        Task new_task;
                        new_task.name = task_name;
                        new_task.priority = selected_priority;
                        new_task.done = false;
                        new_task.id = common->next_task_id++;

                        common->task_map[new_task.id] = new_task;
                        common->sync_needed = true;
                        common->data_ready = true;

                        memset(task_name, 0, sizeof(task_name));
                        selected_priority = 1;
                        show_duplicate_warning = false; // Reset warnings
                        show_empty_name_warning = false; // Reset warnings
                        ImGui::CloseCurrentPopup();
                    }
                }
            }

            // Show warning message for empty task name
            if (show_empty_name_warning) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Task name cannot be empty!");
            }

            // Show warning message for duplicate tasks
            if (show_duplicate_warning) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Task already exists!");
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
                show_duplicate_warning = false; // Reset warnings
                show_empty_name_warning = false; // Reset warnings
            }

            ImGui::EndPopup();
        }



        if (common->data_ready) {
            if (ImGui::BeginTable("Tasks", 5)) {
                ImGui::TableSetupColumn("Task Name");
                ImGui::TableSetupColumn("Priority");
                ImGui::TableSetupColumn("Done");
                ImGui::TableSetupColumn("Details");
                ImGui::TableSetupColumn("Delete");
                ImGui::TableHeadersRow();


                std::vector<std::pair<int, Task>> sorted_tasks(common->task_map.begin(), common->task_map.end());

                std::sort(sorted_tasks.begin(), sorted_tasks.end(), [](const auto& a, const auto& b) {
                    if (a.second.priority != b.second.priority) {
                        return a.second.priority < b.second.priority;
                    }
                    return a.first < b.first;
                    });

                for (const auto& task_pair : sorted_tasks) {
                    Task& task = common->task_map[task_pair.first];

                    // Filter tasks based on the selected button
                    if (show_done_only && !task.done) continue;
                    if (show_not_done_only && task.done) continue;

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", task.name.c_str());

                    ImGui::TableSetColumnIndex(1);
                    switch (task.priority) {
                    case 1: ImGui::Text("Urgent and Important"); break;
                    case 2: ImGui::Text("Urgent and Not Important"); break;
                    case 3: ImGui::Text("Not Urgent and Important"); break;
                    case 4: ImGui::Text("Not Urgent and Not Important"); break;
                    }

                    ImGui::TableSetColumnIndex(2);
                    bool previous_done_state = task.done;

                    ImGui::PushStyleColor(ImGuiCol_CheckMark, task.done ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                    ImGui::Checkbox(("##Done" + std::to_string(task.id)).c_str(), &task.done);
                    ImGui::PopStyleColor();

                    if (!previous_done_state && task.done) {
                        printf("\a");
                    }

                    ImGui::TableSetColumnIndex(3);
                    bool has_details = !task.details.empty();

                    if (has_details) {
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.0f, 0.5f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.3f, 0.7f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.0f, 0.4f, 1.0f));
                    }

                    if (ImGui::Button(("Details##" + std::to_string(task.id)).c_str())) {
                        task.show_details = !task.show_details;
                    }

                    if (has_details) {
                        ImGui::PopStyleColor(3);
                    }

                    if (task.show_details) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Details:");

                        ImGui::TableSetColumnIndex(1);
                        char buffer[512];
                        strncpy_s(buffer, task.details.c_str(), sizeof(buffer));
                        if (ImGui::InputTextMultiline(("##DetailsInput" + std::to_string(task.id)).c_str(), buffer, sizeof(buffer),
                            ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 3))) {
                            task.details = buffer;
                        }
                    }

                    ImGui::TableSetColumnIndex(4);
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

                    if (ImGui::Button(("Delete##" + std::to_string(task.id)).c_str())) {
                        common->task_map.erase(task.id);
                        ImGui::PopStyleColor(3);
                        continue;
                    }

                    ImGui::PopStyleColor(3);
                }

                ImGui::EndTable();
            }


        // Positioning the motivational quotes button directly below the tasks table aligned to the right
        ImGui::Spacing(); // Add some spacing between the table and button
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10); // Fine-tune spacing below the table

        // Calculate button position to align it to the right corner
        ImVec2 windowSize = ImGui::GetWindowSize();
        float buttonWidth = 250.0f; // Approximate button width, adjust if needed
        float buttonHeight = 60.0f; // Approximate button height, adjust if needed
        ImVec2 buttonSize(buttonWidth, buttonHeight);

        // Set cursor position to right align the button below the table
        ImGui::SetCursorPosX(windowSize.x - buttonWidth - 10); // 10 pixels padding from the right edge

        // Set lighter green color for the motivational button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 1.0f, 0.6f, 1.0f));          // Light green color
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.9f, 0.5f, 1.0f));   // Slightly darker green on hover
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.8f, 0.4f, 1.0f));    // Even darker green when active

        // Set text color to black
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // Black text

        // Display the "Motivational Quote" button
        if (ImGui::Button("Feeling unmotivated? click me!", ImVec2(buttonWidth, buttonHeight))) {
            std::string quotes_url = "https://zenquotes.io/api/random";
            std::string command = "start " + quotes_url; // Use 'xdg-open' on Linux or 'open' on MacOS
            system(command.c_str());
        }

        ImGui::PopStyleColor(4); // Pop colors for both button and text

    }


    ImGui::End();
}
}

void DrawThread::operator()(CommonObjects& common) {
    GuiMain(DrawAppWindow, &common);  // Ensure GuiMain is correctly declared and defined
    common.exit_flag = true;
}


