#include "imgui-SFML.h"
#include "imgui.h"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <fmt/core.h>
#include <random>
#include <string>

#include "reuss/ScalarImage.h"
#include "reuss/ZmqReceiver.h"
#include "table.h"

#include "reuss/StreamingReceiver.h"

struct ButtonColors {
    ImColor normal;
    ImColor hover;
    ImColor active;
};

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

static ButtonColors start_color{ImColor(77, 168, 77), ImColor(115, 191, 115),
                                ImColor(48, 105, 48)};
static ButtonColors stop_color{ImColor(223, 83, 83), ImColor(235, 147, 147),
                               ImColor(151, 28, 28)};

static constexpr int format = GL_BGR;

static sf::Shader *sptr;

// void (*ImDrawCallback)(const ImDrawList* parent_list, const ImDrawCmd* cmd);
void bind_shader(const ImDrawList *parent_list, const ImDrawCmd *cmd) {
    // sf::Shader::bind(sptr);
}

void clear_shader(const ImDrawList *parent_list, const ImDrawCmd *cmd) {
    // sf::Shader::bind(NULL);
}

int main() {

    // State
    bool show_receiver = true;
    size_t packets_lost = 0;
    int64_t last_frame_caught = -1;
    int64_t total_frames_caught = 0;
    bool show_panel = false;
    bool show_data_window = true;
    bool receiver_started = false;
    bool connect_zmq = false;

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

    // UDP Receiver
    reuss::StreamingReceiver udp_receiver;

    // Zmq Receiver (all this should probably go into a class)
    // std::string endpoint = "ipc://sls_raw_data";
    std::string endpoint = "tcp://localhost:4545";
    reuss::ZmqReceiver zmq_receiver{endpoint};
    zmq_receiver.set_zmq_hwm(3);
    zmq_receiver.set_timeout(10);
    int64_t frame_number = -1;
    std::vector<uint16_t> buffer(512 * 1024);

    double scale_factor = 1.;

    std::vector<uint8_t> image_data(100 * 100 * 4);
    for (int i = 0; i < image_data.size(); ++i) {
        image_data[i] = 100;
    }

    sf::RenderWindow window(sf::VideoMode(1500, 1000), "Jungfrau EM Control");
    window.setFramerateLimit(30);
    ImGui::SFML::Init(window);
    sf::Clock deltaClock;

    char mess[] = "hej";
    const std::string fragmentShaderSource =
        "#version 110\n"
        "uniform sampler2D texture,textures;\n"
        "varying vec2 texcoord;\n"
        "void main()\n"
        "{\n"
        "vec4 col = texture2D(texture, texcoord);\n"
        "col.a =  float(0.5);\n"
        "gl_FragColor = col;\n"
        "}\n";

    GLuint gltexture;
    LoadTexture((char *)img.data(), &gltexture, 512, 1024, format);

    if (!sf::Shader::isAvailable()) {
        fmt::print("Shader not available");
        exit(1);
    }
    sf::Shader shader;
    if (!shader.loadFromMemory(fragmentShaderSource, sf::Shader::Fragment)) {
        fmt::print("Could not load shader\n");
        exit(1);
    }
    sptr = &shader;

    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Detector control");
        ImGui::Text(fmt::format("Counter: {}", scale_factor).c_str());
        ImGui::Button("Start");
        ImGui::Button("Stop");
        ImGui::Button("Pedestal");
        ImGui::End();

        ImGui::Begin("Windows"); // Create a window called "Hello, world!" and
                                 // append into it.
        ImGui::Text("This is some useful text."); // Display some text (you can
                                                  // use a format strings too)
        ImGui::Checkbox("Receiver", &show_receiver);
        ImGui::Checkbox("Show panel", &show_panel);
        ImGui::Checkbox("Show data", &show_data_window);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::End();

        if (show_data_window) {
            // only render if we have to

            img.map(); // uses quite some cpu, only render if have to,
            // std::atomic<bool> data_changed?
            // texture.update(img.data());

            // Data window
            // ImGui::SetNextWindowContentSize(ImVec2(width, height));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin("Live data", &show_data_window,
                         ImGuiWindowFlags_HorizontalScrollbar |
                             ImGuiWindowFlags_NoScrollWithMouse);

            glBindTexture(GL_TEXTURE_2D, gltexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 512, 0, format,
                         GL_UNSIGNED_BYTE, img.data());
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImDrawList *drawList = ImGui::GetWindowDrawList();

            drawList->AddCallback(&bind_shader, NULL);
            drawList->AddImage(gltexture, pos,
                               ImVec2(pos.x + 1024, pos.y + 512), ImVec2(0, 1),
                               ImVec2(1, 0));
            drawList->AddCallback(&clear_shader, NULL);
            if (ImGui::IsWindowHovered()) {

                if (auto wheel = ImGui::GetIO().MouseWheel; wheel) {

                    auto mouse_pos = ImGui::GetIO().MousePos;
                    auto screen_pos = ImGui::GetCursorScreenPos();
                    auto x =
                        (mouse_pos.x - screen_pos.x - ImGui::GetScrollX()) /
                        (width * scale_factor);
                    auto y =
                        (mouse_pos.y + screen_pos.y - ImGui::GetScrollY()) /
                        (height * scale_factor);
                    y -= 1;
                    ImGui::SetScrollHereX(x);
                    ImGui::SetScrollHereY(y);
                    fmt::print("Mouse: {}, {}\n", x, y);
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
                if (ImGui::Button("Stop", button_size)){
                    receiver_started = false;
                    udp_receiver.stop();
                    fmt::print("stopped\n");
                }
                    
                ImGui::PopStyleColor(3);

            } else {
                ImGui::PushStyleColor(ImGuiCol_Button,
                                      (ImVec4)start_color.normal);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                      (ImVec4)start_color.hover);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                                      (ImVec4)start_color.active);

                if (ImGui::Button("Start", button_size)){
                    receiver_started = true;
                    udp_receiver.start();
                    fmt::print("started\n");
                }
                    
                ImGui::PopStyleColor(3);
            }
            if (ImGui::Checkbox("Connect ZMQ", &connect_zmq)) {
                if (connect_zmq) {
                    fmt::print("Connecting zmq\n");
                    // zmq_receiver.connect();
                } else {
                    fmt::print("Disconnecting zmq\n");
                    // zmq_receiver.disconnect();
                }
            }
            if (connect_zmq) {
                zmq_receiver.connect();
                zmq_receiver.receive_into(
                    1, &frame_number,
                    reinterpret_cast<std::byte *>(img.raw_data()));
                zmq_receiver.disconnect();
            }

            ImGui::Text("Status");
            if (ImGui::BeginTable("receiver_table", 2)) {
                ImGui::TableNextColumn();
                ImGui::Text("Packets Lost:");
                packets_lost = udp_receiver.packets_lost(); //should we always update?
                ImGui::TableNextColumn();
                ImGui::Text("%d", packets_lost);
                ImGui::TableNextColumn();
                ImGui::Text("Last Frame Caught:");
                last_frame_caught = udp_receiver.last_frame();
                ImGui::TableNextColumn();
                ImGui::Text("%d", last_frame_caught);
                ImGui::TableNextColumn();
                ImGui::Text("Total Frames Caught:");
                total_frames_caught = udp_receiver.total_frames();
                ImGui::TableNextColumn();
                ImGui::Text("%d", total_frames_caught);
            }
            ImGui::EndTable();
            // }

            ImGui::End();
        }

        window.clear();
        window.draw(shape, &shader);
        window.resetGLStates();
        ImGui::SFML::Render(window);
        // ImGui::SFML::Render(gltexture);
        window.display();
    }

    ImGui::SFML::Shutdown();
}