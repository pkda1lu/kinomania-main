#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include "Database.hpp"

class MainWindow;

class Application {
public:
    Application();
    ~Application();

    void run();

private:
    void initialize();
    void handleEvents();
    void update();
    void render();

    sf::RenderWindow window;
    sf::Color backgroundColor;
    sf::Color accentColor;
    
    bool isFullscreen;
    bool isAdminPanelVisible;
    
    void toggleFullscreen();
    void toggleAdminPanel();
    
    static constexpr int WINDOW_WIDTH = 1280;
    static constexpr int WINDOW_HEIGHT = 720;
    static constexpr const char* WINDOW_TITLE = "Kinomania";
    static constexpr const char* WINDOW_ICON = "resources/images/icon.png";

    std::unique_ptr<Database> database;
    std::unique_ptr<MainWindow> mainWindow;
}; 