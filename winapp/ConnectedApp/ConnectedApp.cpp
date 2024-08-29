//#include <GL/glew.h>       // For OpenGL functions and GLEW
//#include <GL/gl.h>         // For standard OpenGL functions
#include <iostream>        // For standard I/O (e.g., std::cerr)
#include <thread>
#include <string>
#include <limits>
#include <algorithm>
//#include "stb_image.h"     // For loading images
//#define STB_IMAGE_IMPLEMENTATION
#include "CommonObject.h"
#include "DrawThread.h"
#include "DownloadThread.h"
//#include "GuiMain.h"
#include "../../shared/ImGuiSrc/imgui.h"

//GLuint LoadTexture(const char* filename) {
//    int width, height, channels;
//    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
//    if (data == nullptr) {
//        std::cerr << "Failed to load texture: " << filename << std::endl;
//        return 0;
//    }
//
//    GLuint textureID;
//    glGenTextures(1, &textureID);
//    glBindTexture(GL_TEXTURE_2D, textureID);
//    glTexImage2D(GL_TEXTURE_2D, 0, (channels == 4 ? GL_RGBA : GL_RGB), width, height, 0, (channels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, data);
//    glGenerateMipmap(GL_TEXTURE_2D);
//
//    stbi_image_free(data); // Free the image data after uploading it to GPU
//    return textureID;
//}

int main() {
    // Initialize OpenGL and GLEW
   /* GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
        return -1;
    }*/

    // Create the shared object that holds the tasks and synchronization flags
    CommonObjects common;

    // Load tasks from file at the start
    common.LoadTasksFromFile();

    //GLuint backgroundTexture = LoadTexture("Externals/wallpaper.jpg");  // Load the background texture

    // Create instances of DrawThread and DownloadThread
    DrawThread drawThread;
    DownloadThread downloadThread;

    // Start the GUI thread in parallel
    std::thread gui_thread([&]() {
        drawThread(common);
        });

    //// Render background
    //ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    //ImGui::SetNextWindowSize(ImVec2(1920, 1080), ImGuiCond_Always);
    //ImGui::Begin("Background", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoInputs);
    //ImGui::Image((void*)(intptr_t)backgroundTexture, ImVec2(1920, 1080));
    //ImGui::End();

    // Save tasks to file before exiting
    common.SaveTasksToFile();

    // Wait for the GUI thread to finish
    if (gui_thread.joinable())
        gui_thread.join();

    std::cout << "Application exiting..." << std::endl;
    return 0;
}
