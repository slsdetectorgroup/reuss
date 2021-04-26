#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>


#include <random>
#include <fmt/core.h>

struct Pixel
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} __attribute__((packed));

struct Image
{
    Image(size_t width, size_t height)
        : width(width), height(height), pixels(width * height) {}
    Pixel &operator()(size_t row, size_t col)
    {
        return pixels[col + row * width];
    }
    uint8_t *data() { return reinterpret_cast<uint8_t *>(pixels.data()); }
    void set_alpha(uint8_t alpha)
    {
        for (auto &p : pixels)
            p.a = alpha;
    }
    size_t width;
    size_t height;
    std::vector<Pixel> pixels;
};

int main()
{

    bool show_demo_window = true;
    bool show_another_window = false;
    bool show_data_window = true;

    double scale_factor = 1.;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 255);

    sf::RenderWindow window(sf::VideoMode(1500, 1000), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    ImGui::SFML::Init(window);

    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::Clock deltaClock;

    int width = 256;
    int height = 256;

    sf::Texture texture;
    if (!texture.create(width, height))
    {
        throw std::runtime_error("Could not create texture");
    }

    Image img(width, height);
    img.set_alpha(255);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        //Generate random numbers
        for (int row = 0; row < height; ++row)
        {
            for (int col = 0; col < width; ++col)
            {
                img(row, col).r = dist(rng);
                img(row, col).g = dist(rng);
                img(row, col).b = dist(rng);
            }
        }

        texture.update(img.data());
        // sf::Sprite sprite(texture);

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Detector control");

        ImGui::Text(fmt::format("Counter: {}", scale_factor).c_str());
        ImGui::Button("Start");
        ImGui::Button("Stop");
        ImGui::Button("Pedestal");
        ImGui::End();

        ImGui::Begin("Some window");                       // Create a window called "Hello, world!" and append into it.
        ImGui::Text("This is some useful text.");          // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);
        ImGui::Checkbox("Show data", &show_data_window);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        // ImGui::CreateContext();
        // ImPlot::CreateContext();
        // ImGui::ShowDemoWindow();
        // ImPlot::DestroyContext();
        // ImGui::DestroyContext();
        
        if (show_data_window)
        {

            ImGui::SetNextWindowContentSize(ImVec2(width, height));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            
            // ImGui::SetNextWindowContentSize(ImVec2(width, height), ImGuiCond_Once);
            ImGui::Begin("Live data", &show_data_window, ImGuiWindowFlags_HorizontalScrollbar);
            if (ImGui::IsWindowHovered())
                scale_factor += 0.1 * ImGui::GetIO().MouseWheel;
            ImGui::Image(texture, ImVec2(width * scale_factor, height * scale_factor));
            ImGui::End();
            ImGui::PopStyleVar();
        }

        window.clear();
        window.draw(shape);
        window.resetGLStates();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}