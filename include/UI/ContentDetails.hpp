#pragma once
#include <SFML/Graphics.hpp>
#include "Database.hpp"
#include <vector>
#include <string>
#include <memory>
#include <sstream>

struct CommentObject {
    sf::RectangleShape panel;
    sf::Text username;
    sf::Text rating;
    sf::Text text;
};

class ContentDetails {
public:
    ContentDetails(sf::RenderWindow& window, Database& db);
    ~ContentDetails();

    void show(int contentId);
    void hide();
    void handleEvent(const sf::Event& event);
    void update(float deltaTime);
    void render();
    bool isVisible() const;

private:
    void loadResources();
    void initializeUI();
    void updateUI();
    void updateCursorPosition();
    void loadComments();

    sf::RenderWindow& window;
    Database& database;
    bool m_isVisible;
    
    Content content;
    sf::Texture coverTexture;
    sf::RectangleShape coverImage;
    sf::Font font;
    sf::Color backgroundColor;
    sf::Color textColor;
    sf::Color accentColor;
    
    sf::RectangleShape background;
    sf::RectangleShape closeButton;
    sf::Text closeText;
    
    sf::Text titleText;
    sf::Text genreText;
    sf::Text yearText;
    sf::Text descriptionText;
    sf::Text averageRatingText;
    
    sf::Text ratingTitle;
    sf::CircleShape ratingStars[5];
    sf::Text ratingTexts[5];
    int currentRating;
    
    sf::Text commentTitle;
    sf::RectangleShape commentInput;
    sf::Text commentText;
    sf::RectangleShape submitButton;
    sf::Text submitText;
    
    sf::Text commentsTitle;
    std::vector<CommentObject> commentObjects;
    
    sf::RectangleShape cursor;
    bool cursorVisible;
    float cursorBlinkTime;
    bool isCommentActive;

    void wrapText(sf::Text& text, float maxWidth, const sf::Font& font);
}; 