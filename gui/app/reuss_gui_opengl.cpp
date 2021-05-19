// Dear ImGui: standalone example application for GLFW + OpenGL 3, using
// programmable pipeline (GLFW is a cross-platform general purpose library for
// handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation,
// etc.) If you are new to Dear ImGui, read documentation from the docs/ folder
// + read the top of imgui.cpp. Read online:
// https://github.com/ocornut/imgui/tree/master/docs

// reuss
#include "reuss/ScalarImage.h"
#include "reuss/ZmqReceiver.h"
// end reuss

#include <fmt/core.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include <stdio.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load
//  OpenGL function pointers. Helper libraries are often used for this purpose!
//  Here we are supporting a few common ones (gl3w, glew, glad). You may use
//  another loader/header of your choice (glext, glLoadGen, etc.), or chose to
//  manually implement your own.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h> // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h> // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h> // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h> // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE // GLFW including OpenGL headers causes ambiguity or
                          // multiple definition errors.
#include <glbinding/Binding.h> // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE // GLFW including OpenGL headers causes ambiguity or
                          // multiple definition errors.
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h> // Initialize with glbinding::initialize()
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to
// maximize ease of testing and compatibility with old VS compilers. To link
// with VS2010-era libraries, VS2015+ requires linking with
// legacy_stdio_definitions.lib, which we do using this pragma. Your own project
// should not be affected, as you are likely to link with a newer binary of GLFW
// that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) &&                                 \
    !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// Simple helper function to load an image into a OpenGL texture with common
// settings
bool LoadTexture(const char *image_data, GLuint *out_texture, int width,
                 int height, int format = GL_RGBA) {

    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE); // This is required on WebGL for non
                                       // power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format,
                 GL_UNSIGNED_BYTE, image_data);

    *out_texture = image_texture;
    return true;
}

static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

struct ButtonColors {
    ImColor normal;
    ImColor hover;
    ImColor active;
};
static constexpr int format = GL_BGR;

int main(int, char **) {

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

        // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char *glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char *glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
    // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow *window =
        glfwCreateWindow(1500, 1000, "reuss::Jungfrau EM GUI", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    //  glewExperimental = true;
    bool err = glewInit() != GLEW_OK;
    fmt::print("glewInit: {}\n", err);
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    bool err = gladLoadGL(glfwGetProcAddress) ==
               0; // glad2 recommend using the windowing library loader instead
                  // of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char *name) {
        return (glbinding::ProcAddress)glfwGetProcAddress(name);
    });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader
                      // is likely to requires some form of initialization.
#endif
    if (err) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
    // Keyboard Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //
    // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can
    // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
    // them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
    // need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please
    // handle those errors in your application (e.g. use an assertion, or
    // display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and
    // stored into a texture when calling
    // ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame
    // below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string
    // literal you need to write a double backslash \\ !
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    // ImFont* font =
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
    // NULL, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool show_receiver = true;
    size_t lost_packets = 0;
    int64_t last_frame_caught = -1;
    int64_t total_frame_caught = -1;
    bool show_panel = false;
    bool show_data_window = true;
    bool receiver_started = false;
    bool connect_zmq = false;

    double scale_factor = 1.;

    static ButtonColors start_color{
        ImColor(77, 168, 77), ImColor(115, 191, 115), ImColor(48, 105, 48)};
    static ButtonColors stop_color{ImColor(223, 83, 83), ImColor(235, 147, 147),
                                   ImColor(151, 28, 28)};

    // color state
    int width = 1024;
    int height = 512;
    float cmin = 0., cmax = 10000.;
    const char *items[] = {"viridis", "cviridis", "inferno", "bone"};
    const int colormaps[] = {cv::COLORMAP_VIRIDIS, cv::COLORMAP_CIVIDIS,
                             cv::COLORMAP_INFERNO, cv::COLORMAP_BONE};
    int item_current = 0;
    reuss::ScalarImage<uint16_t> img(width, height);
    img.set_clim(cmin, cmax);
    img.set_colormap(colormaps[item_current]);
    img.map();

    // Zmq Receiver (all this should probably go into a class)
    std::string endpoint = "ipc://sls_raw_data";
    reuss::ZmqReceiver zmq_receiver{endpoint};
    zmq_receiver.set_zmq_hwm(1);
    zmq_receiver.set_timeout(10);
    int64_t frame_number = -1;
    std::vector<uint16_t> buffer(512 * 1024);

    GLuint gltexture;
    LoadTexture((char *)img.data(), &gltexture, width, height, format);
    err = glGetError();
    fmt::print("texture: {} err: {}:{}\n", gltexture, err,
               glewGetErrorString(err));

unsigned int VBO;
glGenBuffers(1, &VBO);  
glBindBuffer(GL_ARRAY_BUFFER, VBO);  
// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    const char *vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    const char *fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n\n"
        "void main()\n"
        "{\n"
        "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n";
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fmt::print("err: {}\n", infoLog);
    }
    glDeleteShader(fragmentShader);
float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
}; 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);  
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 1. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
        // tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data
        // to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
        // data to your main application. Generally you may always pass all
        // inputs to dear imgui, and hide them from your application based on
        // those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End
        // pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!"); // Create a window called "Hello,
                                           // world!" and append into it.

            ImGui::Text(
                "This is some useful text."); // Display some text (you can use
                                              // a format strings too)

            ImGui::SliderFloat(
                "float", &f, 0.0f,
                1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3(
                "clear color",
                (float *)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button(
                    "Button")) // Buttons return true when clicked (most widgets
                               // return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
            ImGui::End();
        }
        {
            ImGui::Begin("Detector control");
            ImGui::Text(fmt::format("Counter: {}", scale_factor).c_str());
            ImGui::Button("Start");
            ImGui::Button("Stop");
            ImGui::Button("Pedestal");
            ImGui::End();
        }
        {

            ImGui::Begin("Windows"); // Create a window called "Hello, world!"
                                     // and append into it.
            ImGui::Text(
                "This is some useful text."); // Display some text (you can
                                              // use a format strings too)
            ImGui::Checkbox("Receiver", &show_receiver);
            ImGui::Checkbox("Show panel", &show_panel);
            ImGui::Checkbox("Show data", &show_data_window);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
            ImGui::End();
        }

        if (show_data_window) {
            // only render if we have to

            img.map(); // uses quite some cpu, only render if have to,
            // std::atomic<bool> data_changed?

            // Data window
            // ImGui::SetNextWindowContentSize(ImVec2(width, height));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin("Live data", &show_data_window,
                         ImGuiWindowFlags_HorizontalScrollbar |
                             ImGuiWindowFlags_NoScrollWithMouse);

            
            err = glGetError();
            fmt::print("texture: {} err: {} : {}\n", gltexture, err,
                       glewGetErrorString(err));
            glUseProgram(shaderProgram);
            glBindTexture(GL_TEXTURE_2D, gltexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 512, 0, format,
                         GL_UNSIGNED_BYTE, img.data());
            
            // ImGui::AddImage((void *)gltexture);
            ImGui::Image((void *)gltexture,
                         ImVec2(width * scale_factor, height * scale_factor));
            
             glUseProgram(0);
            if (ImGui::IsWindowHovered()) {

                if (auto wheel = ImGui::GetIO().MouseWheel; wheel) {

                    auto mouse_pos = ImGui::GetIO().MousePos;
                    auto screen_pos = ImGui::GetCursorScreenPos();
                    // auto x =
                    //     (mouse_pos.x - screen_pos.x - ImGui::GetScrollX()) /
                    //     (width * scale_factor);
                    // auto y =
                    //     (mouse_pos.y + screen_pos.y - ImGui::GetScrollY()) /
                    //     (height * scale_factor);
                    // y -= 1;
                    // ImGui::SetScrollHereX(x);
                    // ImGui::SetScrollHereY(y);
                    // fmt::print("Mouse: {}, {}\n", x, y);
                    scale_factor += 0.1 * wheel;
                }
                // scale_factor += 0.1 * ImGui::GetIO().MouseWheel;
            }

            ImGui::End();
           
            ImGui::PopStyleVar();
            ImGui::Begin("Plot control");
            if (ImGui::DragFloatRange2("clim", &cmin, &cmax, 0.25f, 0.0f,
                                       10000.0f, "cmin: %.1f", "cmax: %.1f",
                                       ImGuiSliderFlags_AlwaysClamp))
                img.set_clim(cmin, cmax);

            if (ImGui::ListBox("listbox", &item_current, items,
                               IM_ARRAYSIZE(items), 4)) {
                img.set_colormap(colormaps[item_current]);
            }
            ImGui::End();
        }

        if (show_receiver) {
            auto button_size = ImVec2(300, 50);
            ImGui::Begin("Receiver", &show_receiver);
            if (receiver_started) {
                ImGui::PushStyleColor(ImGuiCol_Button,
                                      (ImVec4)stop_color.normal);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                      (ImVec4)stop_color.hover);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                                      (ImVec4)stop_color.active);
                if (ImGui::Button("Stop", button_size))
                    receiver_started = false;
                ImGui::PopStyleColor(3);

            } else {
                ImGui::PushStyleColor(ImGuiCol_Button,
                                      (ImVec4)start_color.normal);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                      (ImVec4)start_color.hover);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                                      (ImVec4)start_color.active);

                if (ImGui::Button("Start", button_size))
                    receiver_started = true;
                ImGui::PopStyleColor(3);
            }
            if (ImGui::Checkbox("Connect ZMQ", &connect_zmq)) {
                if (connect_zmq) {
                    fmt::print("Connecting zmq\n");
                    zmq_receiver.connect();
                } else {
                    fmt::print("Disconnecting zmq\n");
                    zmq_receiver.disconnect();
                }
            }
            if (connect_zmq) {
                zmq_receiver.receive_into(
                    1, &frame_number,
                    reinterpret_cast<std::byte *>(img.raw_data()));
                // img.map();
                // texture.update(img.data());
            }

            ImGui::Text("Status");
            if (ImGui::BeginTable("receiver_table", 2)) {
                ImGui::TableNextColumn();
                ImGui::Text("Packets Lost:");
                ImGui::TableNextColumn();
                ImGui::Text("%d", lost_packets);
                ImGui::TableNextColumn();
                ImGui::Text("Last Frame Caught:");
                ImGui::TableNextColumn();
                ImGui::Text("%d", last_frame_caught);
                ImGui::TableNextColumn();
                ImGui::Text("Total Frames Caught:");
                ImGui::TableNextColumn();
                ImGui::Text("%d", total_frame_caught);
            }
            ImGui::EndTable();
            // }

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w,
                     clear_color.y * clear_color.w,
                     clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
