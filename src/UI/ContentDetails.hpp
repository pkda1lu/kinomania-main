#pragma once

#include <SFML/Graphics.hpp>
#include "Database.hpp"
#include <vector>
#include <memory>

class ContentDetails {
public:
    ContentDetails(sf::RenderWindow& window, Database& db);
    ~ContentDetails();
    
    void show(const Content& content);
    void hide();
    void handleEvent(const sf::Event& event);
    void update();
    void render();
    bool isVisible() const;
    
private:
    void initializeUI();
    void updateLayout();
    void handleRatingClick(int rating);
    void handleCommentSubmit();
    void loadComments();
    
    sf::RenderWindow& window;
    Database& database;
    sf::Font font;
    sf::Texture coverTexture;
    sf::Sprite cover;
    sf::Text title;
    sf::Text year;
    sf::Text genres;
    sf::Text tags;
    sf::Text description;
    std::vector<sf::RectangleShape> starButtons;
    std::vector<sf::Text> starTexts;
    sf::RectangleShape commentInput;
    sf::Text commentText;
    std::vector<sf::Text> comments;
    Content currentContent;
    bool visible;
    int currentRating;
    
    sf::RectangleShape background;
    sf::RectangleShape contentPanel;
    sf::RectangleShape closeButton;
    sf::Text closeText;
    
    static constexpr float COVER_WIDTH = 200.0f;
    static constexpr float COVER_HEIGHT = 300.0f;
    static constexpr float STAR_SIZE = 40.0f;
    static constexpr float COMMENT_INPUT_HEIGHT = 40.0f;
}; 