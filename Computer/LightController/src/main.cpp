// system includes
#include <filesystem>
#include <iostream>
#include <string>


// graphics libraries include
#include <imgui.h>
#include <implot.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_opengl3_loader.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
    #include <GLES2/gl2.h>
#endif

#include <GLFW/glfw3.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
    #pragma comment(lib, "legacy_stdio_definitions")
#endif



// user includes
#include "backend/other/defines.hpp"
#include "frontend/MasterWindow.hpp"
#include "frontend/other/BaseWindow/BaseWindow.hpp"


// main helper functions
void glfw_error_callback(int error, const char *description);
void start_cycle();
void end_cycle(GLFWwindow *const window);



int main()
{
    // set glsl version
    const char *glsl_version = "#version 150";

    // set glfw callback function
    glfwSetErrorCallback(glfw_error_callback);

    // initialise glfw
    if (!glfwInit()) exit(EXIT_FAILURE);

// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    // create master window
    GLFWwindow *window = glfwCreateWindow(MASTER_WINDOW_WIDTH, MASTER_WINDOW_HEIGHT, "LightController", nullptr, nullptr);

    // check if window was created successfully
    if (window == nullptr) exit(EXIT_FAILURE);

    // make context current
    glfwMakeContextCurrent(window);

    // enable vsync
    glfwSwapInterval(1);

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Setup Platform and Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // add fonts (order is crucial)
    // PAGEFILLING
    io.Fonts->AddFontFromFileTTF(std::filesystem::canonical(ARIALBLACK_FONT_PATH).string().c_str(), 320.0F);
    // TITLE
    io.Fonts->AddFontFromFileTTF(std::filesystem::canonical(ARIALBOLD_FONT_PATH).string().c_str(), 56.0F);
    // SUBTITLE
    io.Fonts->AddFontFromFileTTF(std::filesystem::canonical(ARIALBOLD_FONT_PATH).string().c_str(), 40.0F);
    // TEXT
    io.Fonts->AddFontFromFileTTF(std::filesystem::canonical(ARIAL_FONT_PATH).string().c_str(), 32.0F);
    // INFORMATION
    io.Fonts->AddFontFromFileTTF(std::filesystem::canonical(ARIAL_FONT_PATH).string().c_str(), 16.0F);
    // NUMBER
    io.Fonts->AddFontFromFileTTF(std::filesystem::canonical(MONACO_FONT_PATH).string().c_str(), 32.0F);

    // style setting
    BaseWindow *baseWindow                   = new BaseWindow();
    style.Colors[ImGuiCol_TableBorderStrong] = baseWindow->default_ImGuiCol_TableBorderStrong;
    style.Colors[ImGuiCol_TableBorderLight]  = baseWindow->default_ImGuiCol_TableBorderLight;
    style.Colors[ImGuiCol_Button]            = baseWindow->default_ImGuiCol_Button;
    style.Colors[ImGuiCol_ButtonHovered]     = baseWindow->default_ImGuiCol_ButtonHovered;
    style.Colors[ImGuiCol_ButtonActive]      = baseWindow->default_ImGuiCol_ButtonActive;
    style.Colors[ImGuiCol_WindowBg]          = baseWindow->default_ImGuiCol_WindowBg;
    style.Colors[ImGuiCol_ChildBg]           = baseWindow->default_ImGuiCol_ChildBg;
    style.Colors[ImGuiCol_FrameBg]           = baseWindow->default_ImGuiCol_FrameBg;
    style.WindowPadding                      = baseWindow->default_WindowPadding;
    style.FramePadding                       = baseWindow->default_FramePadding;
    style.WindowBorderSize                   = baseWindow->default_WindowBorderSize;
    style.WindowRounding                     = baseWindow->default_WindowRounding;
    style.FrameBorderSize                    = baseWindow->default_FrameBorderSize;
    style.FrameRounding                      = baseWindow->default_FrameRounding;
    style.ChildBorderSize                    = baseWindow->default_ChildBorderSize;
    style.ChildRounding                      = baseWindow->default_ChildRounding;

    // update windows context
    MasterWindow *masterWindow = new MasterWindow;

    while (!glfwWindowShouldClose(window))
    {
        start_cycle();

        ImGui::NewFrame();
        ImPlot::CreateContext();
        masterWindow->Draw(ImVec2(0.0F, 0.0F), ImVec2(io_width, io_height));
        ImPlot::DestroyContext();
        ImGui::Render();

        end_cycle(window);
    }

    // cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}



// basic callback function for glfw
void glfw_error_callback(int error, const char *description) { fprintf(stderr, "Glfw Error %d: %s\n", error, description); }



// start new frame
void start_cycle()
{
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
}



// end new frame
void end_cycle(GLFWwindow *const window)
{
    const auto clear_color = ImVec4(30.0F / 255.0F, 30.0F / 255.0F, 30.0F / 255.0F, 1.00f);
    int        display_w   = 0;
    int        display_h   = 0;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}
