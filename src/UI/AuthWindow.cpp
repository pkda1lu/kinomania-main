#include "UI/AuthWindow.hpp"
#include <iostream>

AuthWindow::AuthWindow(sf::RenderWindow& window, Database& db)
    : window(window), database(db), m_isVisible(false), 
    isLoginMode(true), activeInput(-1), errorMessage(""), 
    onLoginSuccessCallback(nullptr), cursorBlinkTime(0.0f), cursorVisible(false) {
    loadResources();
    initializeUI();
}

AuthWindow::~AuthWindow() {}

void AuthWindow::loadResources() {
    if (!font.loadFromFile("resources/fonts/arial.ttf")) {
        std::cerr << "Error loading font in AuthWindow" << std::endl;
        if (!font.loadFromFile("build/resources/fonts/arial.ttf")) {
            throw std::runtime_error("Failed to load any font");
        }
    }
    
    backgroundColor = sf::Color(32, 32, 32, 230);
    accentColor = sf::Color(0, 128, 255);
    textColor = sf::Color(240, 240, 240);
}

void AuthWindow::initializeUI() {
    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);
    
    background.setSize(sf::Vector2f(windowWidth, windowHeight));
    background.setFillColor(backgroundColor);
    
    panelWidth = 400.f;
    panelHeight = isLoginMode ? 350.f : 400.f;
    panel.setSize(sf::Vector2f(panelWidth, panelHeight));
    panel.setPosition((windowWidth - panelWidth) / 2, (windowHeight - panelHeight) / 2);
    panel.setFillColor(sf::Color(48, 48, 48, 255));
    panel.setOutlineThickness(2);
    panel.setOutlineColor(accentColor);
    
    closeButton.setSize(sf::Vector2f(30, 30));
    closeButton.setFillColor(sf::Color(64, 64, 64));
    closeButton.setPosition(panel.getPosition().x + panelWidth - 40, panel.getPosition().y + 10);
    
    closeText.setFont(font);
    closeText.setString("X");
    closeText.setCharacterSize(18);
    closeText.setFillColor(textColor);
    closeText.setPosition(closeButton.getPosition().x + (closeButton.getSize().x - closeText.getLocalBounds().width) / 2 - 5, 
                        closeButton.getPosition().y + (closeButton.getSize().y - closeText.getLocalBounds().height) / 2 - 9);
    
    title.setFont(font);
    title.setString(isLoginMode ? "Login" : "Register");
    title.setCharacterSize(24);
    title.setFillColor(textColor);
    title.setPosition(panel.getPosition().x + (panelWidth - title.getLocalBounds().width) / 2, panel.getPosition().y + 20);
    
    usernameInput.setSize(sf::Vector2f(panelWidth - 80, 40));
    usernameInput.setPosition(panel.getPosition().x + 40, panel.getPosition().y + 80);
    usernameInput.setFillColor(sf::Color(64, 64, 64));
    usernameInput.setOutlineThickness(1);
    usernameInput.setOutlineColor(sf::Color(100, 100, 100));
    
    usernameLabel.setFont(font);
    usernameLabel.setString("Username");
    usernameLabel.setCharacterSize(16);
    usernameLabel.setFillColor(textColor);
    usernameLabel.setPosition(usernameInput.getPosition().x, usernameInput.getPosition().y - 25);
    
    usernameText.setFont(font);
    usernameText.setString("");
    usernameText.setCharacterSize(18);
    usernameText.setFillColor(textColor);
    usernameText.setPosition(usernameInput.getPosition().x + 10, usernameInput.getPosition().y + 10);
    
    passwordInput.setSize(sf::Vector2f(panelWidth - 80, 40));
    passwordInput.setPosition(panel.getPosition().x + 40, panel.getPosition().y + 150);
    passwordInput.setFillColor(sf::Color(64, 64, 64));
    passwordInput.setOutlineThickness(1);
    passwordInput.setOutlineColor(sf::Color(100, 100, 100));
    
    passwordLabel.setFont(font);
    passwordLabel.setString("Password");
    passwordLabel.setCharacterSize(16);
    passwordLabel.setFillColor(textColor);
    passwordLabel.setPosition(passwordInput.getPosition().x, passwordInput.getPosition().y - 25);
    
    passwordText.setFont(font);
    passwordText.setString("");
    passwordText.setCharacterSize(18);
    passwordText.setFillColor(textColor);
    passwordText.setPosition(passwordInput.getPosition().x + 10, passwordInput.getPosition().y + 10);
    
    emailInput.setSize(sf::Vector2f(panelWidth - 80, 40));
    emailInput.setPosition(panel.getPosition().x + 40, panel.getPosition().y + 220);
    emailInput.setFillColor(sf::Color(64, 64, 64));
    emailInput.setOutlineThickness(1);
    emailInput.setOutlineColor(sf::Color(100, 100, 100));
    
    emailLabel.setFont(font);
    emailLabel.setString("Email");
    emailLabel.setCharacterSize(16);
    emailLabel.setFillColor(textColor);
    emailLabel.setPosition(emailInput.getPosition().x, emailInput.getPosition().y - 25);
    
    emailText.setFont(font);
    emailText.setString("");
    emailText.setCharacterSize(18);
    emailText.setFillColor(textColor);
    emailText.setPosition(emailInput.getPosition().x + 10, emailInput.getPosition().y + 10);
    
    submitButton.setSize(sf::Vector2f(panelWidth - 80, 50));
    submitButton.setPosition(panel.getPosition().x + 40, isLoginMode ? panel.getPosition().y + 220 : panel.getPosition().y + 290);
    submitButton.setFillColor(accentColor);
    
    submitText.setFont(font);
    submitText.setString(isLoginMode ? "Login" : "Register");
    submitText.setCharacterSize(18);
    submitText.setFillColor(textColor);
    submitText.setPosition(submitButton.getPosition().x + (submitButton.getSize().x - submitText.getLocalBounds().width) / 2, submitButton.getPosition().y + (submitButton.getSize().y - submitText.getLocalBounds().height) / 2 - 5);
    
    switchModeButton.setSize(sf::Vector2f(panelWidth - 80, 30));
    switchModeButton.setPosition(panel.getPosition().x + 40, isLoginMode ? panel.getPosition().y + 290 : panel.getPosition().y + 350);
    switchModeButton.setFillColor(sf::Color(64, 64, 64));
    
    switchModeText.setFont(font);
    switchModeText.setString(isLoginMode ? "Need an account? Register" : "Already have an account? Login");
    switchModeText.setCharacterSize(14);
    switchModeText.setFillColor(textColor);
    switchModeText.setPosition(switchModeButton.getPosition().x + (switchModeButton.getSize().x - switchModeText.getLocalBounds().width) / 2, switchModeButton.getPosition().y + (switchModeButton.getSize().y - switchModeText.getLocalBounds().height) / 2 - 5);
    
    errorText.setFont(font);
    errorText.setString("");
    errorText.setCharacterSize(14);
    errorText.setFillColor(sf::Color::Red);
    errorText.setPosition(panel.getPosition().x + 40, submitButton.getPosition().y - 30);
    
    cursor.setSize(sf::Vector2f(2, 24));
    cursor.setFillColor(sf::Color::White);
    cursor.setPosition(usernameText.getPosition().x, usernameText.getPosition().y);
}

void AuthWindow::show() {
    m_isVisible = true;
    errorMessage = "";
    updateUI();
}

void AuthWindow::hide() {
    m_isVisible = false;
    errorMessage = "";
}

void AuthWindow::toggleMode() {
    isLoginMode = !isLoginMode;
    updateUI();
}

void AuthWindow::setOnLoginSuccess(std::function<void(const std::string&)> callback) {
    onLoginSuccessCallback = callback;
}

void AuthWindow::updateUI() {
    title.setString(isLoginMode ? "Login" : "Register");
    
    panelHeight = isLoginMode ? 350.f : 400.f;
    panel.setSize(sf::Vector2f(panelWidth, panelHeight));
    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);
    panel.setPosition((windowWidth - panelWidth) / 2, (windowHeight - panelHeight) / 2);
    
    title.setPosition(panel.getPosition().x + (panelWidth - title.getLocalBounds().width) / 2, panel.getPosition().y + 20);
    
    closeButton.setPosition(panel.getPosition().x + panelWidth - 40, panel.getPosition().y + 10);
    closeText.setPosition(closeButton.getPosition().x + (closeButton.getSize().x - closeText.getLocalBounds().width) / 2 - 5, 
                        closeButton.getPosition().y + (closeButton.getSize().y - closeText.getLocalBounds().height) / 2 - 9);
    
    usernameInput.setPosition(panel.getPosition().x + 40, panel.getPosition().y + 80);
    usernameLabel.setPosition(usernameInput.getPosition().x, usernameInput.getPosition().y - 25);
    usernameText.setPosition(usernameInput.getPosition().x + 10, usernameInput.getPosition().y + 10);
    
    passwordInput.setPosition(panel.getPosition().x + 40, panel.getPosition().y + 150);
    passwordLabel.setPosition(passwordInput.getPosition().x, passwordInput.getPosition().y - 25);
    passwordText.setPosition(passwordInput.getPosition().x + 10, passwordInput.getPosition().y + 10);
    
    emailInput.setPosition(panel.getPosition().x + 40, panel.getPosition().y + 220);
    emailLabel.setPosition(emailInput.getPosition().x, emailInput.getPosition().y - 25);
    emailText.setPosition(emailInput.getPosition().x + 10, emailInput.getPosition().y + 10);
    
    submitButton.setPosition(panel.getPosition().x + 40, isLoginMode ? panel.getPosition().y + 220 : panel.getPosition().y + 290);
    submitText.setString(isLoginMode ? "Login" : "Register");
    submitText.setPosition(submitButton.getPosition().x + (submitButton.getSize().x - submitText.getLocalBounds().width) / 2, submitButton.getPosition().y + (submitButton.getSize().y - submitText.getLocalBounds().height) / 2 - 5);
    
    switchModeButton.setPosition(panel.getPosition().x + 40, isLoginMode ? panel.getPosition().y + 290 : panel.getPosition().y + 350);
    switchModeText.setString(isLoginMode ? "Need an account? Register" : "Already have an account? Login");
    switchModeText.setPosition(switchModeButton.getPosition().x + (switchModeButton.getSize().x - switchModeText.getLocalBounds().width) / 2, switchModeButton.getPosition().y + (switchModeButton.getSize().y - switchModeText.getLocalBounds().height) / 2 - 5);
    
    errorText.setString(errorMessage);
    errorText.setPosition(panel.getPosition().x + 40, submitButton.getPosition().y - 30);
    
    updateCursorPosition();
}

void AuthWindow::handleEvent(const sf::Event& event) {
    if (!m_isVisible) return;
    
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        hide();
        return;
    }
    
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        
        if (closeButton.getGlobalBounds().contains(mousePos)) {
            hide();
            return;
        }
        
        if (usernameInput.getGlobalBounds().contains(mousePos)) {
            activeInput = 0;
            cursorVisible = true;
            cursorBlinkTime = 0;
            updateCursorPosition();
        } else if (passwordInput.getGlobalBounds().contains(mousePos)) {
            activeInput = 1;
            cursorVisible = true;
            cursorBlinkTime = 0;
            updateCursorPosition();
        } else if (!isLoginMode && emailInput.getGlobalBounds().contains(mousePos)) {
            activeInput = 2;
            cursorVisible = true;
            cursorBlinkTime = 0;
            updateCursorPosition();
        } else if (submitButton.getGlobalBounds().contains(mousePos)) {
            handleSubmit();
        } else if (switchModeButton.getGlobalBounds().contains(mousePos)) {
            toggleMode();
        } else {
            activeInput = -1;
        }
    } else if (event.type == sf::Event::TextEntered && activeInput >= 0) {
        if (event.text.unicode == 8) {
            if (activeInput == 0 && !usernameText.getString().isEmpty()) {
                std::string newString = usernameText.getString().toAnsiString();
                newString.pop_back();
                usernameText.setString(newString);
            } else if (activeInput == 1 && !passwordText.getString().isEmpty()) {
                std::string newString = passwordText.getString().toAnsiString();
                newString.pop_back();
                passwordText.setString(newString);
            } else if (activeInput == 2 && !emailText.getString().isEmpty() && !isLoginMode) {
                std::string newString = emailText.getString().toAnsiString();
                newString.pop_back();
                emailText.setString(newString);
            }
            updateCursorPosition();
        } else if (event.text.unicode == 9) {
            if (isLoginMode) {
                activeInput = (activeInput + 1) % 2;
            } else {
                activeInput = (activeInput + 1) % 3;
            }
            cursorVisible = true;
            cursorBlinkTime = 0;
            updateCursorPosition();
        } else if (event.text.unicode == 13) {
            handleSubmit();
        } else if (event.text.unicode >= 32) {
            if (activeInput == 0) {
                usernameText.setString(usernameText.getString() + static_cast<char>(event.text.unicode));
            } else if (activeInput == 1) {
                passwordText.setString(passwordText.getString() + static_cast<char>(event.text.unicode));
            } else if (activeInput == 2 && !isLoginMode) {
                emailText.setString(emailText.getString() + static_cast<char>(event.text.unicode));
            }
            updateCursorPosition();
        }
    } else if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
        
        if (submitButton.getGlobalBounds().contains(mousePos)) {
            submitButton.setFillColor(sf::Color(30, 144, 255));
        } else {
            submitButton.setFillColor(accentColor);
        }
        
        if (switchModeButton.getGlobalBounds().contains(mousePos)) {
            switchModeButton.setFillColor(sf::Color(80, 80, 80));
        } else {
            switchModeButton.setFillColor(sf::Color(64, 64, 64));
        }
    }
}

void AuthWindow::handleSubmit() {
    std::string username = usernameText.getString().toAnsiString();
    std::string password = passwordText.getString().toAnsiString();
    
    if (username.empty() || password.empty()) {
        errorMessage = "Please fill in all fields";
        updateUI();
        return;
    }
    
    if (isLoginMode) {
        if (database.authenticateUser(username, password)) {
            if (onLoginSuccessCallback) {
                onLoginSuccessCallback(username);
            }
            hide();
        } else {
            errorMessage = "Invalid username or password";
            updateUI();
        }
    } else {
        std::string email = emailText.getString().toAnsiString();
        if (email.empty()) {
            errorMessage = "Please fill in all fields";
            updateUI();
            return;
        }
        
        if (database.registerUser(username, password, email)) {
            if (onLoginSuccessCallback) {
                onLoginSuccessCallback(username);
            }
            hide();
        } else {
            errorMessage = "Registration failed. Username may already exist.";
            updateUI();
        }
    }
}

void AuthWindow::updateCursorPosition() {
    sf::Text* activeText = nullptr;
    if (activeInput == 0) {
        activeText = &usernameText;
    } else if (activeInput == 1) {
        activeText = &passwordText;
    } else if (activeInput == 2) {
        activeText = &emailText;
    }
    
    if (activeText) {
        float textWidth = activeText->getLocalBounds().width;
        cursor.setPosition(activeText->getPosition().x + textWidth, activeText->getPosition().y);
    }
}

void AuthWindow::update(float deltaTime) {
    if (m_isVisible && activeInput >= 0) {
        cursorBlinkTime += deltaTime;
        if (cursorBlinkTime >= 0.5f) {
            cursorBlinkTime = 0;
            cursorVisible = !cursorVisible;
        }
    }
}

void AuthWindow::render() {
    if (!m_isVisible) return;
    
    window.draw(background);
    window.draw(panel);
    window.draw(title);
    
    window.draw(closeButton);
    window.draw(closeText);
    
    window.draw(usernameLabel);
    window.draw(usernameInput);
    window.draw(usernameText);
    
    window.draw(passwordLabel);
    window.draw(passwordInput);
    window.draw(passwordText);
    
    if (!isLoginMode) {
        window.draw(emailLabel);
        window.draw(emailInput);
        window.draw(emailText);
    }
    
    window.draw(submitButton);
    window.draw(submitText);
    window.draw(switchModeButton);
    window.draw(switchModeText);
    
    if (!errorMessage.empty()) {
        window.draw(errorText);
    }
    
    if (cursorVisible && activeInput >= 0) {
        window.draw(cursor);
    }
} 