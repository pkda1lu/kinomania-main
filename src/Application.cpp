#include "Application.hpp"
#include <iostream>
#include "Database.hpp"
#include "UI/MainWindow.hpp"

Application::Application() 
    : isFullscreen(false)
    , isAdminPanelVisible(false)
{
    backgroundColor = sf::Color(18, 18, 18);
    accentColor = sf::Color(0, 245, 208);
}

Application::~Application() {
    window.close();
}

void Application::initialize() {
    window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE, sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
    database = std::make_unique<Database>();
    if (!database->initialize()) {
        throw std::runtime_error("Не удалось инициализировать базу данных");
    }
    mainWindow = std::make_unique<MainWindow>(window, *database);
}

void Application::run() {
    initialize();

    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void Application::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::F11) {
                toggleFullscreen();
            }
            else if (event.key.code == sf::Keyboard::P && 
                     event.key.control && 
                     event.key.alt) {
                toggleAdminPanel();
            }
        }
        if (mainWindow) mainWindow->handleEvent(event);
    }
}

void Application::update() {
    if (mainWindow) mainWindow->update();
}

void Application::render() {
    window.clear(backgroundColor);
    if (mainWindow) mainWindow->render();
    window.display();
}

void Application::toggleFullscreen() {
    isFullscreen = !isFullscreen;
    if (isFullscreen) {
        window.create(sf::VideoMode::getFullscreenModes()[0], WINDOW_TITLE, sf::Style::Fullscreen);
    } else {
        window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE, sf::Style::Titlebar | sf::Style::Close);
    }
}

void Application::toggleAdminPanel() {
    isAdminPanelVisible = !isAdminPanelVisible;
}