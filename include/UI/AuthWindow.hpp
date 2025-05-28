#pragma once

#include <SFML/Graphics.hpp>
#include "Database.hpp"
#include <string>
#include <functional>

class AuthWindow {
public:
    AuthWindow(sf::RenderWindow& window, Database& db);
    ~AuthWindow();

    void show();
    void hide();
    bool isVisible() const { return m_isVisible; }
    
    void handleEvent(const sf::Event& event);
    void update(float deltaTime);
    void render();

    void setOnLoginSuccess(std::function<void(const std::string&)> callback);

private:
    sf::RenderWindow& window;
    Database& database;
    bool m_isVisible;
    bool isLoginMode;
    int activeInput;
    std::string errorMessage;
    
    sf::Font font;
    sf::Color backgroundColor;
    sf::Color accentColor;
    sf::Color textColor;
    
    float panelWidth;
    float panelHeight;
    sf::RectangleShape background;
    sf::RectangleShape panel;
    sf::Text title;
    
    sf::RectangleShape usernameInput;
    sf::Text usernameLabel;
    sf::Text usernameText;
    
    sf::RectangleShape passwordInput;
    sf::Text passwordLabel;
    sf::Text passwordText;
    
    sf::RectangleShape emailInput;
    sf::Text emailLabel;
    sf::Text emailText;
    
    sf::RectangleShape submitButton;
    sf::Text submitText;
    sf::RectangleShape switchModeButton;
    sf::Text switchModeText;
    sf::Text errorText;
    
    sf::RectangleShape closeButton;
    sf::Text closeText;
    
    sf::RectangleShape cursor;
    bool cursorVisible;
    float cursorBlinkTime;
    
    std::function<void(const std::string&)> onLoginSuccessCallback;
    
    void loadResources();
    void initializeUI();
    void updateUI();
    void toggleMode();
    void handleSubmit();
    void updateCursorPosition();
}; 