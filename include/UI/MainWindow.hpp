#pragma once

#include <SFML/Graphics.hpp>
#include "Database.hpp"
#include "UI/ContentDetails.hpp"
#include "UI/AuthWindow.hpp"
#include <string>
#include <vector>
#include <memory>

class MainWindow {
public:
    MainWindow(sf::RenderWindow& window, Database& db);
    ~MainWindow();

    void handleEvent(const sf::Event& event);
    void update();
    void render();

private:
    sf::RenderWindow& window;
    Database& database;
    sf::Font font;
    sf::Texture logoTexture;
    sf::Sprite logo;
    
    sf::RectangleShape searchBar;
    sf::Text searchText;
    sf::RectangleShape loginButton;
    sf::Text loginText;
    sf::RectangleShape registerButton;
    sf::Text registerText;
    sf::CircleShape clearSearchButton;
    sf::Text clearSearchText;
    
    sf::RectangleShape addContentButton;
    sf::Text addContentText;
    
    bool showAddContentPanel;
    sf::RectangleShape addContentPanel;
    sf::RectangleShape addContentPanelClose;
    sf::Text addContentPanelCloseText;
    sf::Text addContentPanelTitle;
    
    sf::RectangleShape titleField;
    sf::Text titleLabel;
    sf::Text titleFieldText;
    
    sf::RectangleShape yearField;
    sf::Text yearLabel;
    sf::Text yearFieldText;
    
    sf::RectangleShape categoryField;
    sf::Text categoryLabel;
    sf::Text categoryFieldText;
    
    sf::RectangleShape coverPathField;
    sf::Text coverPathLabel;
    sf::Text coverPathFieldText;
    
    sf::RectangleShape descriptionField;
    sf::Text descriptionLabel;
    sf::Text descriptionFieldText;
    
    sf::RectangleShape genresField;
    sf::Text genresLabel;
    sf::Text genresFieldText;
    
    sf::RectangleShape tagsField;
    sf::Text tagsLabel;
    sf::Text tagsFieldText;
    
    sf::RectangleShape submitButton;
    sf::Text submitButtonText;
    
    int activeField;
    sf::RectangleShape cursor;
    bool cursorVisible;
    float cursorBlinkTime;
    
    std::vector<sf::RectangleShape> categoryButtons;
    std::vector<sf::Text> categoryTexts;
    std::vector<std::string> categories = {
        "All", "Movies", "Cartoons", "TV Series", "Animated Series", "Anime"
    };
    int selectedCategory;
    
    std::vector<Content> contentList;
    std::vector<sf::RectangleShape> contentItems;
    std::vector<sf::Text> contentTitles;
    std::vector<sf::Text> contentYears;
    std::vector<sf::Text> contentGenres;
    std::vector<sf::Sprite> contentCovers;
    
    std::unique_ptr<ContentDetails> contentDetails;
    
    std::unique_ptr<AuthWindow> authWindow;
    std::string currentUsername;
    
    sf::RectangleShape background;
    
    bool isSearchActive;
    
    bool isAdminPanelVisible = false;
    bool isEditing = false;
    int editingContentId = -1;
    
    bool showImportPanel = false;
    bool isImportTextActive = false;
    sf::RectangleShape importPanel;
    sf::Text importPanelTitle;
    sf::RectangleShape importPanelClose;
    sf::Text importPanelCloseText;
    sf::RectangleShape importTextField;
    sf::Text importTextLabel;
    sf::Text importTextContent;
    sf::RectangleShape importButton;
    sf::Text importButtonText;
    sf::RectangleShape loadFileButton;
    sf::Text loadFileButtonText;
    
    void initializeUI();
    void loadResources();
    void updateLayout();
    void handleSearch(const std::string& query);
    void handleCategoryClick(int index);
    void handleContentClick(int index);
    void updateContentGrid();
    void handleLoginClick();
    void handleRegisterClick();
    void onLoginSuccess(const std::string& username);
    
    void initializeAddContentPanel();
    void toggleAddContentPanel();
    void handleAddContentClick();
    void handleAddContentFormSubmit();
    void handleAddContentFieldClick(int fieldIndex);
    void updateCursorPosition();
    
    void toggleAdminPanel();
    
    void handleImportData();
    
    void loadFromTextFile();
    
    void wrapText(sf::Text& text, float maxWidth, const sf::Font& font);
    
    static constexpr float CATEGORY_WIDTH = 200.0f;
    static constexpr float LOGO_WIDTH = 180.0f;
    static constexpr float LOGO_HEIGHT = 60.0f;
    static constexpr float SEARCH_BAR_WIDTH = 400.0f;
    static constexpr float SEARCH_BAR_HEIGHT = 40.0f;
    static constexpr float BUTTON_WIDTH = 100.0f;
    static constexpr float BUTTON_HEIGHT = 40.0f;
    static constexpr float CATEGORY_HEIGHT = 50.0f;
}; 