#include "imgui-SFML.h"
#include "imgui.h"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>

#include <fmt/core.h>
#include <random>
#include <string>

#include "reuss/ScalarImage.h"
#include "reuss/ZmqReceiver.h"
#include "table.h"

struct ButtonColors {
    ImColor normal;
    ImColor hover;
    ImColor active;
};

static ButtonColors start_color{ImColor(77, 168, 77), ImColor(115, 191, 115),
                                ImColor(48, 105, 48)};
static ButtonColors stop_color{ImColor(223, 83, 83), ImColor(235, 147, 147),
                               ImColor(151, 28, 28)};
int main() {

    // State
    bool show_receiver = true;
    size_t packets_lost = 0;
    int64_t last_frame_caught = -1;
    int64_t total_frame_caught = -1;
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

    // Zmq Receiver (all this should probably go into a class)
    std::string endpoint = "ipc://sls_raw_data";
    reuss::ZmqReceiver zmq_receiver{endpoint};
    zmq_receiver.set_zmq_hwm(1);
    zmq_receiver.set_timeout(10);
    int64_t frame_number = -1;
    std::vector<uint16_t> buffer(512 * 1024);

    // std::vector<std::string> nodes{"someip:5000", "anotherip:5001"};

    double scale_factor = 1.;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 255);

    sf::RenderWindow window(sf::VideoMode(1500, 1000), "Jungfrau EM Control");
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    ImGui::SFML::Init(window);

    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::Clock deltaClock;

    sf::Texture texture;
    if (!texture.create(width, height)) {
        throw std::runtime_error("Could not create texture");
    }

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
            //only render if we have to

            // img.map(); //uses quite some cpu, only render if have to, std::atomic<bool> data_changed?
            texture.update(img.data());

            // Data window
            // ImGui::SetNextWindowContentSize(ImVec2(width, height));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin("Live data", &show_data_window,
                         ImGuiWindowFlags_HorizontalScrollbar |
                             ImGuiWindowFlags_NoScrollWithMouse);
            ImGui::Image(texture,
                         ImVec2(width * scale_factor, height * scale_factor));
            if (ImGui::IsWindowHovered()) {

                if (auto wheel = ImGui::GetIO().MouseWheel; wheel) {
                    
                    auto mouse_pos = ImGui::GetIO().MousePos;
                    auto screen_pos = ImGui::GetCursorScreenPos();
                    auto x = (mouse_pos.x-screen_pos.x -  ImGui::GetScrollX())/(width * scale_factor);
                    auto y = (mouse_pos.y+screen_pos.y -  ImGui::GetScrollY())/(height * scale_factor);
                    y-=1;
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
                ImGui::Text("%d", packets_lost);
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

        window.clear();
        window.draw(shape);
        window.resetGLStates();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}