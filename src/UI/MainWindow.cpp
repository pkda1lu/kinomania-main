#include "UI/MainWindow.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <Windows.h>
#include <commdlg.h>


MainWindow::MainWindow(sf::RenderWindow& window, Database& db)
    : window(window), database(db), selectedCategory(-1), isSearchActive(false), 
      showAddContentPanel(false), activeField(-1), cursorVisible(false), cursorBlinkTime(0.0f) {
    loadResources();
    initializeUI();
    initializeAddContentPanel();
    contentDetails = std::make_unique<ContentDetails>(window, database);
    authWindow = std::make_unique<AuthWindow>(window, database);
    authWindow->setOnLoginSuccess([this](const std::string& username) {
        onLoginSuccess(username);
    });
}

MainWindow::~MainWindow() {}

void MainWindow::loadResources() {
    if (!font.loadFromFile("resources/fonts/arial.ttf")) {
        std::cerr << "Error loading font" << std::endl;
        if (!font.loadFromFile("build/resources/fonts/arial.ttf")) {
            throw std::runtime_error("Failed to load any font");
        }
    }
    if (!logoTexture.loadFromFile("resources/images/logo.png")) {
        std::cerr << "Failed to load logo" << std::endl;
    }
    logo.setTexture(logoTexture);
    logo.setScale(LOGO_WIDTH / logoTexture.getSize().x, LOGO_HEIGHT / logoTexture.getSize().y);
    background.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
    background.setFillColor(sf::Color(32, 32, 32));
}

void MainWindow::initializeUI() {
    searchBar.setSize(sf::Vector2f(SEARCH_BAR_WIDTH, SEARCH_BAR_HEIGHT));
    searchBar.setFillColor(sf::Color(48, 48, 48));
    searchBar.setOutlineThickness(2);
    searchBar.setOutlineColor(sf::Color(64, 64, 64));
    searchText.setFont(font);
    searchText.setString("Search...");
    searchText.setCharacterSize(20);
    searchText.setFillColor(sf::Color(180, 180, 180));
    
    clearSearchButton.setRadius(15);
    clearSearchButton.setFillColor(sf::Color(48, 48, 48));
    clearSearchButton.setOutlineThickness(2);
    clearSearchButton.setOutlineColor(sf::Color(64, 64, 64));
    clearSearchText.setFont(font);
    clearSearchText.setString("X");
    clearSearchText.setCharacterSize(16);
    clearSearchText.setFillColor(sf::Color::White);
    
    loginButton.setSize(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
    loginButton.setFillColor(sf::Color(48, 48, 48));
    loginButton.setOutlineThickness(2);
    loginButton.setOutlineColor(sf::Color(64, 64, 64));
    loginText.setFont(font);
    loginText.setString("Login");
    loginText.setCharacterSize(20);
    loginText.setFillColor(sf::Color::White);
    
    registerButton.setSize(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
    registerButton.setFillColor(sf::Color(48, 48, 48));
    registerButton.setOutlineThickness(2);
    registerButton.setOutlineColor(sf::Color(64, 64, 64));
    registerText.setFont(font);
    registerText.setString("Register");
    registerText.setCharacterSize(20);
    registerText.setFillColor(sf::Color::White);
    
    categoryButtons.clear();
    categoryTexts.clear();
    float categoryY = 100;
    for (const auto& category : categories) {
        sf::RectangleShape button;
        button.setSize(sf::Vector2f(CATEGORY_WIDTH, CATEGORY_HEIGHT));
        button.setFillColor(sf::Color(48, 48, 48));
        button.setOutlineThickness(2);
        button.setOutlineColor(sf::Color(64, 64, 64));
        button.setPosition(20, categoryY);
        sf::Text text;
        text.setFont(font);
        text.setString(category);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);
        text.setPosition(30, categoryY + 10);
        categoryButtons.push_back(button);
        categoryTexts.push_back(text);
        categoryY += CATEGORY_HEIGHT + 10;
    }
    updateLayout();
    updateContentGrid();
    
    float importPanelWidth = 600;
    float importPanelHeight = 500;
    float importPanelX = (window.getSize().x - importPanelWidth) / 2;
    float importPanelY = (window.getSize().y - importPanelHeight) / 2;
    
    importPanel.setSize(sf::Vector2f(importPanelWidth, importPanelHeight));
    importPanel.setPosition(importPanelX, importPanelY);
    importPanel.setFillColor(sf::Color(40, 40, 40));
    importPanel.setOutlineThickness(2);
    importPanel.setOutlineColor(sf::Color(80, 80, 80));
    
    importPanelTitle.setFont(font);
    importPanelTitle.setString("Import Data from Text File");
    importPanelTitle.setCharacterSize(24);
    importPanelTitle.setFillColor(sf::Color::White);
    sf::FloatRect importTitleBounds = importPanelTitle.getLocalBounds();
    importPanelTitle.setPosition(
        importPanelX + (importPanelWidth - importTitleBounds.width) / 2,
        importPanelY + 20
    );
    
    importPanelClose.setSize(sf::Vector2f(30, 30));
    importPanelClose.setPosition(importPanelX + importPanelWidth - 40, importPanelY + 10);
    importPanelClose.setFillColor(sf::Color(80, 80, 80));
    
    importPanelCloseText.setFont(font);
    importPanelCloseText.setString("X");
    importPanelCloseText.setCharacterSize(18);
    importPanelCloseText.setFillColor(sf::Color::White);
    sf::FloatRect importCloseBounds = importPanelCloseText.getLocalBounds();
    importPanelCloseText.setPosition(
        importPanelClose.getPosition().x + (importPanelClose.getSize().x - importCloseBounds.width) / 2 - 2,
        importPanelClose.getPosition().y + (importPanelClose.getSize().y - importCloseBounds.height) / 2 - 9
    );
    
    loadFileButton.setSize(sf::Vector2f(200, 40));
    loadFileButton.setPosition(importPanelX + 20, importPanelY + 70);
    loadFileButton.setFillColor(sf::Color(80, 140, 220));
    
    loadFileButtonText.setFont(font);
    loadFileButtonText.setString("Load from .txt file");
    loadFileButtonText.setCharacterSize(16);
    loadFileButtonText.setFillColor(sf::Color::White);
    sf::FloatRect loadFileTextBounds = loadFileButtonText.getLocalBounds();
    loadFileButtonText.setPosition(
        loadFileButton.getPosition().x + (loadFileButton.getSize().x - loadFileTextBounds.width) / 2,
        loadFileButton.getPosition().y + (loadFileButton.getSize().y - loadFileTextBounds.height) / 2 - 8
    );
    
    importTextLabel.setFont(font);
    importTextLabel.setString("Enter data in the following format:");
    importTextLabel.setCharacterSize(18);
    importTextLabel.setFillColor(sf::Color::White);
    importTextLabel.setPosition(importPanelX + 20, importPanelY + 130);
    
    sf::Text formatHelpText;
    formatHelpText.setFont(font);
    formatHelpText.setString(
        "title=Movie Title\n"
        "year=2023\n"
        "category=Movies\n"
        "cover=filename.jpg\n"
        "description=Movie description\n"
        "genres=Genre1, Genre2, Genre3\n"
        "tags=tag1, tag2, tag3\n"
        "\n"
        "title=Next movie title\n"
        "year=2022\n"
        "...\n"
    );
    formatHelpText.setCharacterSize(14);
    formatHelpText.setFillColor(sf::Color(180, 180, 180));
    formatHelpText.setPosition(importPanelX + 20, importPanelY + 100);
    
    importTextField.setSize(sf::Vector2f(importPanelWidth - 40, 260));
    importTextField.setPosition(importPanelX + 20, importPanelY + 180);
    importTextField.setFillColor(sf::Color(60, 60, 60));
    importTextField.setOutlineThickness(1);
    importTextField.setOutlineColor(sf::Color(100, 100, 100));
    
    importTextContent.setFont(font);
    importTextContent.setString("");
    importTextContent.setCharacterSize(16);
    importTextContent.setFillColor(sf::Color::White);
    importTextContent.setPosition(importPanelX + 30, importPanelY + 190);
    
    importButton.setSize(sf::Vector2f(200, 50));
    importButton.setPosition(importPanelX + (importPanelWidth - 200) / 2, importPanelY + importPanelHeight - 70);
    importButton.setFillColor(sf::Color(0, 128, 0));
    
    importButtonText.setFont(font);
    importButtonText.setString("Import");
    importButtonText.setCharacterSize(20);
    importButtonText.setFillColor(sf::Color::White);
    sf::FloatRect importBtnBounds = importButtonText.getLocalBounds();
    importButtonText.setPosition(
        importButton.getPosition().x + (importButton.getSize().x - importBtnBounds.width) / 2,
        importButton.getPosition().y + (importButton.getSize().y - importBtnBounds.height) / 2 - 10
    );
}

void MainWindow::initializeAddContentPanel() {
    float panelWidth = 520;
    float panelHeight = 650;
    float panelX = (window.getSize().x - panelWidth) / 2;
    float panelY = (window.getSize().y - panelHeight) / 2;
    
    addContentPanel.setSize(sf::Vector2f(panelWidth, panelHeight));
    addContentPanel.setPosition(panelX, panelY);
    addContentPanel.setFillColor(sf::Color(48, 48, 54));
    addContentPanel.setOutlineThickness(4);
    addContentPanel.setOutlineColor(sf::Color(30, 30, 30, 180));
    
    addContentPanelTitle.setFont(font);
    addContentPanelTitle.setString("Add New Content");
    addContentPanelTitle.setCharacterSize(28);
    addContentPanelTitle.setFillColor(sf::Color::White);
    sf::FloatRect titleBounds = addContentPanelTitle.getLocalBounds();
    addContentPanelTitle.setPosition(
        panelX + (panelWidth - titleBounds.width) / 2,
        panelY + 24
    );
    
    addContentPanelClose.setSize(sf::Vector2f(34, 34));
    addContentPanelClose.setPosition(panelX + panelWidth - 44, panelY + 14);
    addContentPanelClose.setFillColor(sf::Color(80, 80, 80));
    addContentPanelClose.setOutlineThickness(1);
    addContentPanelClose.setOutlineColor(sf::Color(120, 120, 120));
    
    addContentPanelCloseText.setFont(font);
    addContentPanelCloseText.setString("X");
    addContentPanelCloseText.setCharacterSize(20);
    addContentPanelCloseText.setFillColor(sf::Color(220, 220, 220));
    sf::FloatRect closeBounds = addContentPanelCloseText.getLocalBounds();
    addContentPanelCloseText.setPosition(
        addContentPanelClose.getPosition().x + (addContentPanelClose.getSize().x - closeBounds.width) / 2 - 2,
        addContentPanelClose.getPosition().y + (addContentPanelClose.getSize().y - closeBounds.height) / 2 - 9
    );
    
    float fieldWidth = 420;
    float fieldHeight = 44;
    float fieldX = panelX + (panelWidth - fieldWidth) / 2;
    float fieldY = panelY + 70;
    float fieldSpacing = 68;
    float labelOffset = -28;
    
    titleLabel.setFont(font);
    titleLabel.setString("Title:");
    titleLabel.setCharacterSize(18);
    titleLabel.setFillColor(sf::Color(200, 200, 200));
    titleLabel.setPosition(fieldX, fieldY + labelOffset);
    
    titleField.setSize(sf::Vector2f(fieldWidth, fieldHeight));
    titleField.setPosition(fieldX, fieldY);
    titleField.setFillColor(sf::Color(62, 62, 70));
    titleField.setOutlineThickness(2);
    titleField.setOutlineColor(sf::Color(90, 90, 120));
    
    titleFieldText.setFont(font);
    titleFieldText.setString("");
    titleFieldText.setCharacterSize(18);
    titleFieldText.setFillColor(sf::Color(255, 255, 255, 220));
    titleFieldText.setPosition(fieldX + 12, fieldY + 12);
    
    fieldY += fieldSpacing;
    yearLabel.setFont(font);
    yearLabel.setString("Year:");
    yearLabel.setCharacterSize(18);
    yearLabel.setFillColor(sf::Color(200, 200, 200));
    yearLabel.setPosition(fieldX, fieldY + labelOffset);
    
    yearField.setSize(sf::Vector2f(fieldWidth, fieldHeight));
    yearField.setPosition(fieldX, fieldY);
    yearField.setFillColor(sf::Color(62, 62, 70));
    yearField.setOutlineThickness(2);
    yearField.setOutlineColor(sf::Color(90, 90, 120));
    
    yearFieldText.setFont(font);
    yearFieldText.setString("");
    yearFieldText.setCharacterSize(18);
    yearFieldText.setFillColor(sf::Color(255, 255, 255, 220));
    yearFieldText.setPosition(fieldX + 12, fieldY + 12);
    
    fieldY += fieldSpacing;
    categoryLabel.setFont(font);
    categoryLabel.setString("Category:");
    categoryLabel.setCharacterSize(18);
    categoryLabel.setFillColor(sf::Color(200, 200, 200));
    categoryLabel.setPosition(fieldX, fieldY + labelOffset);
    
    categoryField.setSize(sf::Vector2f(fieldWidth, fieldHeight));
    categoryField.setPosition(fieldX, fieldY);
    categoryField.setFillColor(sf::Color(62, 62, 70));
    categoryField.setOutlineThickness(2);
    categoryField.setOutlineColor(sf::Color(90, 90, 120));
    
    categoryFieldText.setFont(font);
    categoryFieldText.setString("");
    categoryFieldText.setCharacterSize(18);
    categoryFieldText.setFillColor(sf::Color(255, 255, 255, 220));
    categoryFieldText.setPosition(fieldX + 12, fieldY + 12);
    
    fieldY += fieldSpacing;
    coverPathLabel.setFont(font);
    coverPathLabel.setString("Cover filename:");
    coverPathLabel.setCharacterSize(18);
    coverPathLabel.setFillColor(sf::Color(200, 200, 200));
    coverPathLabel.setPosition(fieldX, fieldY + labelOffset);
    
    coverPathField.setSize(sf::Vector2f(fieldWidth, fieldHeight));
    coverPathField.setPosition(fieldX, fieldY);
    coverPathField.setFillColor(sf::Color(62, 62, 70));
    coverPathField.setOutlineThickness(2);
    coverPathField.setOutlineColor(sf::Color(90, 90, 120));
    
    coverPathFieldText.setFont(font);
    coverPathFieldText.setString("");
    coverPathFieldText.setCharacterSize(18);
    coverPathFieldText.setFillColor(sf::Color(255, 255, 255, 220));
    coverPathFieldText.setPosition(fieldX + 12, fieldY + 12);
    
    fieldY += fieldSpacing;
    descriptionLabel.setFont(font);
    descriptionLabel.setString("Description:");
    descriptionLabel.setCharacterSize(18);
    descriptionLabel.setFillColor(sf::Color(200, 200, 200));
    descriptionLabel.setPosition(fieldX, fieldY + labelOffset);
    
    descriptionField.setSize(sf::Vector2f(fieldWidth, fieldHeight * 2));
    descriptionField.setPosition(fieldX, fieldY);
    descriptionField.setFillColor(sf::Color(62, 62, 70));
    descriptionField.setOutlineThickness(2);
    descriptionField.setOutlineColor(sf::Color(90, 90, 120));
    
    descriptionFieldText.setFont(font);
    descriptionFieldText.setString("");
    descriptionFieldText.setCharacterSize(18);
    descriptionFieldText.setFillColor(sf::Color(255, 255, 255, 220));
    descriptionFieldText.setPosition(fieldX + 12, fieldY + 12);
    
    fieldY += fieldSpacing + fieldHeight;
    genresLabel.setFont(font);
    genresLabel.setString("Genres (comma separated):");
    genresLabel.setCharacterSize(18);
    genresLabel.setFillColor(sf::Color(200, 200, 200));
    genresLabel.setPosition(fieldX, fieldY + labelOffset);
    
    genresField.setSize(sf::Vector2f(fieldWidth, fieldHeight));
    genresField.setPosition(fieldX, fieldY);
    genresField.setFillColor(sf::Color(62, 62, 70));
    genresField.setOutlineThickness(2);
    genresField.setOutlineColor(sf::Color(90, 90, 120));
    
    genresFieldText.setFont(font);
    genresFieldText.setString("");
    genresFieldText.setCharacterSize(18);
    genresFieldText.setFillColor(sf::Color(255, 255, 255, 220));
    genresFieldText.setPosition(fieldX + 12, fieldY + 12);
    
    fieldY += fieldSpacing;
    tagsLabel.setFont(font);
    tagsLabel.setString("Tags (comma separated):");
    tagsLabel.setCharacterSize(18);
    tagsLabel.setFillColor(sf::Color(200, 200, 200));
    tagsLabel.setPosition(fieldX, fieldY + labelOffset);
    
    tagsField.setSize(sf::Vector2f(fieldWidth, fieldHeight));
    tagsField.setPosition(fieldX, fieldY);
    tagsField.setFillColor(sf::Color(62, 62, 70));
    tagsField.setOutlineThickness(2);
    tagsField.setOutlineColor(sf::Color(90, 90, 120));
    
    tagsFieldText.setFont(font);
    tagsFieldText.setString("");
    tagsFieldText.setCharacterSize(18);
    tagsFieldText.setFillColor(sf::Color(255, 255, 255, 220));
    tagsFieldText.setPosition(fieldX + 12, fieldY + 12);
    
    fieldY += fieldSpacing;
    submitButton.setSize(sf::Vector2f(220, 54));
    submitButton.setPosition(fieldX + (fieldWidth - 220) / 2, fieldY);
    submitButton.setFillColor(sf::Color(0, 180, 60));
    submitButton.setOutlineThickness(2);
    submitButton.setOutlineColor(sf::Color(0, 120, 40));
    
    submitButtonText.setFont(font);
    submitButtonText.setString("Add");
    submitButtonText.setCharacterSize(22);
    submitButtonText.setFillColor(sf::Color::White);
    sf::FloatRect submitBounds = submitButtonText.getLocalBounds();
    submitButtonText.setPosition(
        submitButton.getPosition().x + (submitButton.getSize().x - submitBounds.width) / 2,
        submitButton.getPosition().y + (submitButton.getSize().y - submitBounds.height) / 2 - 10
    );
    
    cursor.setSize(sf::Vector2f(2, 28));
    cursor.setFillColor(sf::Color(255, 255, 255, 200));
}

void MainWindow::updateLayout() {
    logo.setPosition(20, 20);
    float searchBarX = (window.getSize().x - SEARCH_BAR_WIDTH) / 2;
    searchBar.setPosition(searchBarX, 30);
    sf::FloatRect searchBounds = searchText.getLocalBounds();
    searchText.setPosition(searchBarX + 15, 30 + (SEARCH_BAR_HEIGHT - searchBounds.height) / 2 - searchBounds.top);
    float buttonX = window.getSize().x - BUTTON_WIDTH * 2 - 40;
    loginButton.setPosition(buttonX, 30);
    registerButton.setPosition(buttonX + BUTTON_WIDTH + 20, 30);
    sf::FloatRect loginBounds = loginText.getLocalBounds();
    loginText.setPosition(buttonX + (BUTTON_WIDTH - loginBounds.width) / 2, 30 + (BUTTON_HEIGHT - loginBounds.height) / 2 - loginBounds.top);
    sf::FloatRect registerBounds = registerText.getLocalBounds();
    registerText.setPosition(buttonX + BUTTON_WIDTH + 20 + (BUTTON_WIDTH - registerBounds.width) / 2, 30 + (BUTTON_HEIGHT - registerBounds.height) / 2 - registerBounds.top);
    float categoryY = 100;
    for (size_t i = 0; i < categoryButtons.size(); ++i) {
        categoryButtons[i].setPosition(20, categoryY);
        sf::FloatRect textBounds = categoryTexts[i].getLocalBounds();
        categoryTexts[i].setPosition(20 + (CATEGORY_WIDTH - textBounds.width) / 2, categoryY + (CATEGORY_HEIGHT - textBounds.height) / 2 - textBounds.top);
        categoryY += CATEGORY_HEIGHT + 10;
    }
    clearSearchButton.setPosition(searchBar.getPosition().x + SEARCH_BAR_WIDTH - 36, searchBar.getPosition().y + 6);
    clearSearchText.setPosition(clearSearchButton.getPosition().x + (clearSearchButton.getRadius() * 2 - clearSearchText.getLocalBounds().width) / 2, clearSearchButton.getPosition().y + (clearSearchButton.getRadius() * 2 - clearSearchText.getLocalBounds().height) / 2 - clearSearchText.getLocalBounds().top);
}

void MainWindow::updateContentGrid() {
    contentItems.clear();
    contentTitles.clear();
    contentYears.clear();
    contentGenres.clear();
    contentCovers.clear();
    
    if (selectedCategory == 0 || selectedCategory == -1) {
        contentList = database.getAllContent();
    } else if (selectedCategory > 0 && selectedCategory < categories.size()) {
        contentList = database.getContentByCategory(categories[selectedCategory]);
    } else {
        contentList = database.getAllContent();
    }
    
    float gridStartX = CATEGORY_WIDTH + 40;
    float gridStartY = 100;
    float itemWidth = 160;
    float itemHeight = 260;
    float gap = 20;
    
    float availableWidth = window.getSize().x - CATEGORY_WIDTH - 60;
    int itemsPerRow = std::max(1, static_cast<int>(availableWidth / (itemWidth + gap)));
    
    for (size_t i = 0; i < contentList.size(); ++i) {
        const auto& content = contentList[i];
        float x = gridStartX + (i % itemsPerRow) * (itemWidth + gap);
        float y = gridStartY + (i / itemsPerRow) * (itemHeight + gap);
        sf::RectangleShape itemRect;
        itemRect.setSize(sf::Vector2f(itemWidth, itemHeight));
        itemRect.setPosition(x, y);
        itemRect.setOutlineThickness(0);
        itemRect.setFillColor(sf::Color(32, 32, 32));
        contentItems.push_back(itemRect);
        
        sf::Texture* coverTexture = new sf::Texture();
        std::string coverPath = "resources/images/covers/" + content.coverPath;
        bool loaded = false;
        if (!coverTexture->loadFromFile(coverPath)) {
            coverPath = "build/resources/images/covers/" + content.coverPath;
            loaded = coverTexture->loadFromFile(coverPath);
        } else {
            loaded = true;
        }
        
        sf::Sprite coverSprite;
        if (loaded) {
            coverSprite.setTexture(*coverTexture);
            float scaleX = 140.0f / coverTexture->getSize().x;
            float scaleY = 200.0f / coverTexture->getSize().y;
            coverSprite.setScale(scaleX, scaleY);
        } else {
            delete coverTexture;
            coverTexture = new sf::Texture();
            sf::Image placeholder;
            placeholder.create(140, 200, sf::Color(80, 80, 80));
            coverTexture->loadFromImage(placeholder);
            coverSprite.setTexture(*coverTexture);
        }
        
        coverSprite.setPosition(x + 10, y + 10);
        contentCovers.push_back(coverSprite);
        
        sf::Text title;
        title.setFont(font);
        title.setCharacterSize(16);
        title.setFillColor(sf::Color::White);
        title.setString(content.title);
        
        wrapText(title, itemWidth - 20, font);
        
        title.setPosition(x + 10, y + 220);
        contentTitles.push_back(title);
        
        sf::Text year;
        year.setFont(font);
        year.setString(std::to_string(content.year));
        year.setCharacterSize(14);
        year.setFillColor(sf::Color(180, 180, 180));
        
        sf::FloatRect titleBounds = title.getGlobalBounds();
        float yearY = titleBounds.top + titleBounds.height + 5;
        year.setPosition(x + 10, yearY);
        contentYears.push_back(year);
        
        sf::Text genres;
        genres.setFont(font);
        std::string genresStr;
        for (size_t j = 0; j < content.genres.size(); ++j) {
            genresStr += content.genres[j];
            if (j + 1 < content.genres.size()) genresStr += ", ";
        }
        
        genres.setString(genresStr);
        
        wrapText(genres, itemWidth - 20, font);
        
        genres.setCharacterSize(12);
        genres.setFillColor(sf::Color(180, 180, 180));
        
        sf::FloatRect yearBounds = year.getGlobalBounds();
        float genresY = yearBounds.top + yearBounds.height + 5;
        genres.setPosition(x + 10, genresY);
        contentGenres.push_back(genres);
    }
}

void MainWindow::handleEvent(const sf::Event& event) {
    if (authWindow && authWindow->isVisible()) {
        authWindow->handleEvent(event);
        
        if (event.type == sf::Event::MouseButtonPressed && 
            event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            
            float authWidth = 400.0f;
            float authHeight = 300.0f;
            float authX = (window.getSize().x - authWidth) / 2;
            float authY = (window.getSize().y - authHeight) / 2;
            sf::FloatRect authBounds(authX, authY, authWidth, authHeight);
            
            if (!authBounds.contains(mousePos)) {
                authWindow->hide();
            }
        }
        return;
    }
    
    if (contentDetails && contentDetails->isVisible()) {
        contentDetails->handleEvent(event);
        
        if (event.type == sf::Event::MouseButtonPressed && 
            event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            
            float detailsWidth = window.getSize().x - 100.0f;
            float detailsHeight = window.getSize().y - 100.0f;
            float detailsX = 50.0f;
            float detailsY = 50.0f;
            sf::FloatRect detailsBounds(detailsX, detailsY, detailsWidth, detailsHeight);
            
            if (!detailsBounds.contains(mousePos)) {
                contentDetails->hide();
            }
        }
        return;
    }
    
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        if (showAddContentPanel) {
            toggleAddContentPanel();
            return;
        }
        if (isAdminPanelVisible) {
            isAdminPanelVisible = false;
            return;
        }
        if (showImportPanel) {
            showImportPanel = false;
            isImportTextActive = false;
            return;
        }
    }
    
    if (showAddContentPanel) {
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                
                if (addContentPanelClose.getGlobalBounds().contains(mousePos)) {
                    toggleAddContentPanel();
                    return;
                }
                
                if (!addContentPanel.getGlobalBounds().contains(mousePos)) {
                    toggleAddContentPanel();
                    return;
                }
                
                if (titleField.getGlobalBounds().contains(mousePos)) {
                    handleAddContentFieldClick(0);
                } else if (yearField.getGlobalBounds().contains(mousePos)) {
                    handleAddContentFieldClick(1);
                } else if (categoryField.getGlobalBounds().contains(mousePos)) {
                    handleAddContentFieldClick(2);
                } else if (coverPathField.getGlobalBounds().contains(mousePos)) {
                    handleAddContentFieldClick(3);
                } else if (descriptionField.getGlobalBounds().contains(mousePos)) {
                    handleAddContentFieldClick(4);
                } else if (genresField.getGlobalBounds().contains(mousePos)) {
                    handleAddContentFieldClick(5);
                } else if (tagsField.getGlobalBounds().contains(mousePos)) {
                    handleAddContentFieldClick(6);
                } else if (submitButton.getGlobalBounds().contains(mousePos)) {
                    handleAddContentFormSubmit();
                } else {
                    activeField = -1;
                    cursorVisible = false;
                }
            }
        } else if (event.type == sf::Event::TextEntered) {
            if (activeField >= 0 && event.text.unicode < 128) {
                std::string currentText = "";
                
                if (activeField == 0) {
                    currentText = titleFieldText.getString();
                } else if (activeField == 1) {
                    currentText = yearFieldText.getString();
                } else if (activeField == 2) {
                    currentText = categoryFieldText.getString();
                } else if (activeField == 3) {
                    currentText = coverPathFieldText.getString();
                } else if (activeField == 4) {
                    currentText = descriptionFieldText.getString();
                } else if (activeField == 5) {
                    currentText = genresFieldText.getString();
                } else if (activeField == 6) {
                    currentText = tagsFieldText.getString();
                }
                
                if (event.text.unicode == 8 && !currentText.empty()) {
                    currentText = currentText.substr(0, currentText.size() - 1);
                }
                else if (event.text.unicode >= 32) {
                    currentText += static_cast<char>(event.text.unicode);
                }
                
                if (activeField == 0) {
                    titleFieldText.setString(currentText);
                } else if (activeField == 1) {
                    yearFieldText.setString(currentText);
                } else if (activeField == 2) {
                    categoryFieldText.setString(currentText);
                } else if (activeField == 3) {
                    coverPathFieldText.setString(currentText);
                } else if (activeField == 4) {
                    descriptionFieldText.setString(currentText);
                } else if (activeField == 5) {
                    genresFieldText.setString(currentText);
                } else if (activeField == 6) {
                    tagsFieldText.setString(currentText);
                }
                
                updateCursorPosition();
            }
        } else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Tab) {
                activeField = (activeField + 1) % 7;
                cursorVisible = true;
                cursorBlinkTime = 0;
                updateCursorPosition();
            } else if (event.key.code == sf::Keyboard::Return) {
                if (activeField == 6) {
                    handleAddContentFormSubmit();
                } else {
                    activeField = (activeField + 1) % 7;
                    cursorVisible = true;
                    cursorBlinkTime = 0;
                    updateCursorPosition();
                }
            } else if (event.key.code == sf::Keyboard::V && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                if (activeField >= 0) {
                    if (OpenClipboard(NULL)) {
                        HANDLE hData = GetClipboardData(CF_TEXT);
                        if (hData != NULL) {
                            char* pszText = static_cast<char*>(GlobalLock(hData));
                            if (pszText != NULL) {
                                std::string currentText = "";
                                
                                if (activeField == 0) {
                                    currentText = titleFieldText.getString();
                                    currentText += pszText;
                                    titleFieldText.setString(currentText);
                                } else if (activeField == 1) {
                                    currentText = yearFieldText.getString();
                                    currentText += pszText;
                                    yearFieldText.setString(currentText);
                                } else if (activeField == 2) {
                                    currentText = categoryFieldText.getString();
                                    currentText += pszText;
                                    categoryFieldText.setString(currentText);
                                } else if (activeField == 3) {
                                    currentText = coverPathFieldText.getString();
                                    currentText += pszText;
                                    coverPathFieldText.setString(currentText);
                                } else if (activeField == 4) {
                                    currentText = descriptionFieldText.getString();
                                    currentText += pszText;
                                    descriptionFieldText.setString(currentText);
                                } else if (activeField == 5) {
                                    currentText = genresFieldText.getString();
                                    currentText += pszText;
                                    genresFieldText.setString(currentText);
                                } else if (activeField == 6) {
                                    currentText = tagsFieldText.getString();
                                    currentText += pszText;
                                    tagsFieldText.setString(currentText);
                                }
                                
                                GlobalUnlock(hData);
                                updateCursorPosition();
                            }
                        }
                        CloseClipboard();
                    }
                }
            } else if (event.key.code == sf::Keyboard::C && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                if (activeField >= 0) {
                    std::string currentText = "";
                    
                    if (activeField == 0) {
                        currentText = titleFieldText.getString();
                    } else if (activeField == 1) {
                        currentText = yearFieldText.getString();
                    } else if (activeField == 2) {
                        currentText = categoryFieldText.getString();
                    } else if (activeField == 3) {
                        currentText = coverPathFieldText.getString();
                    } else if (activeField == 4) {
                        currentText = descriptionFieldText.getString();
                    } else if (activeField == 5) {
                        currentText = genresFieldText.getString();
                    } else if (activeField == 6) {
                        currentText = tagsFieldText.getString();
                    }
                    
                    if (!currentText.empty()) {
                        if (OpenClipboard(NULL)) {
                            EmptyClipboard();
                            HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, currentText.size() + 1);
                            if (hg != NULL) {
                                memcpy(GlobalLock(hg), currentText.c_str(), currentText.size() + 1);
                                GlobalUnlock(hg);
                                SetClipboardData(CF_TEXT, hg);
                            }
                            CloseClipboard();
                        }
                    }
                }
            }
        }
        
        return;
    }
    
    if (showImportPanel) {
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                
                if (importPanelClose.getGlobalBounds().contains(mousePos)) {
                    showImportPanel = false;
                    isImportTextActive = false;
                    return;
                }
                
                if (!importPanel.getGlobalBounds().contains(mousePos)) {
                    showImportPanel = false;
                    isImportTextActive = false;
                    return;
                }
                
                if (loadFileButton.getGlobalBounds().contains(mousePos)) {
                    loadFromTextFile();
                    return;
                }
                
                if (importTextField.getGlobalBounds().contains(mousePos)) {
                    isImportTextActive = true;
                    cursorVisible = true;
                    cursorBlinkTime = 0;
                } else {
                    isImportTextActive = false;
                }
                
                if (importButton.getGlobalBounds().contains(mousePos)) {
                    handleImportData();
                    return;
                }
            }
        } else if (event.type == sf::Event::TextEntered && isImportTextActive) {
            if (event.text.unicode == 8) {
                std::string currentText = importTextContent.getString().toAnsiString();
                if (!currentText.empty()) {
                    currentText.pop_back();
                    importTextContent.setString(currentText);
                }
            } else if (event.text.unicode == 13) {
                std::string currentText = importTextContent.getString().toAnsiString();
                currentText += "\n";
                importTextContent.setString(currentText);
            } else if (event.text.unicode >= 32) {
                importTextContent.setString(importTextContent.getString() + static_cast<char>(event.text.unicode));
            }
        } else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::V && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                if (isImportTextActive) {
                    if (OpenClipboard(NULL)) {
                        HANDLE hData = GetClipboardData(CF_TEXT);
                        if (hData != NULL) {
                            char* pszText = static_cast<char*>(GlobalLock(hData));
                            if (pszText != NULL) {
                                std::string currentText = importTextContent.getString().toAnsiString();
                                currentText += pszText;
                                importTextContent.setString(currentText);
                                GlobalUnlock(hData);
                            }
                        }
                        CloseClipboard();
                    }
                }
            } else if (event.key.code == sf::Keyboard::C && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                if (isImportTextActive) {
                    std::string currentText = importTextContent.getString().toAnsiString();
                    if (!currentText.empty()) {
                        if (OpenClipboard(NULL)) {
                            EmptyClipboard();
                            HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, currentText.size() + 1);
                            if (hg != NULL) {
                                memcpy(GlobalLock(hg), currentText.c_str(), currentText.size() + 1);
                                GlobalUnlock(hg);
                                SetClipboardData(CF_TEXT, hg);
                            }
                            CloseClipboard();
                        }
                    }
                }
            }
        }
        
        return;
    }

    if (isAdminPanelVisible && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mouse(event.mouseButton.x, event.mouseButton.y);
        
        sf::FloatRect adminPanelRect(50, 50, window.getSize().x - 100, window.getSize().y - 100);
        if (!adminPanelRect.contains(mouse)) {
            isAdminPanelVisible = false;
            return;
        }
        
        sf::FloatRect closeRect(window.getSize().x - 80, 60, 30, 30);
        if (closeRect.contains(mouse)) {
            isAdminPanelVisible = false;
            return;
        }
        
        sf::FloatRect addBtnRect(70, 100, 180, 36);
        if (addBtnRect.contains(mouse)) {
            toggleAddContentPanel();
            isAdminPanelVisible = false;
            return;
        }
        
        sf::FloatRect importBtnRect(270, 100, 200, 36);
        if (importBtnRect.contains(mouse)) {
            showImportPanel = true;
            isAdminPanelVisible = false;
            return;
        }
        
        float rowY = 190;
        float rowHeight = 40;
        for (size_t i = 0; i < contentList.size(); ++i) {
            sf::FloatRect editRect(600, rowY + 5, 80, 30);
            if (editRect.contains(mouse)) {
                editingContentId = contentList[i].id;
                titleFieldText.setString(contentList[i].title);
                yearFieldText.setString(std::to_string(contentList[i].year));
                categoryFieldText.setString(contentList[i].category);
                coverPathFieldText.setString(contentList[i].coverPath);
                descriptionFieldText.setString(contentList[i].description);
                
                std::string genresStr = "";
                for (size_t g = 0; g < contentList[i].genres.size(); ++g) {
                    genresStr += contentList[i].genres[g];
                    if (g + 1 < contentList[i].genres.size()) genresStr += ", ";
                }
                genresFieldText.setString(genresStr);
                
                std::string tagsStr = "";
                for (size_t t = 0; t < contentList[i].tags.size(); ++t) {
                    tagsStr += contentList[i].tags[t];
                    if (t + 1 < contentList[i].tags.size()) tagsStr += ", ";
                }
                tagsFieldText.setString(tagsStr);
                
                showAddContentPanel = true;
                isEditing = true;
                activeField = -1;
                cursorVisible = false;
                isAdminPanelVisible = false;
                return;
            }
            
            sf::FloatRect delRect(690, rowY + 5, 80, 30);
            if (delRect.contains(mouse)) {
                database.deleteContent(contentList[i].id);
                updateContentGrid();
                return;
            }
            
            rowY += rowHeight;
        }
    }
    
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            
            if (searchBar.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                isSearchActive = true;
                if (searchText.getString() == "Search...") {
                    searchText.setString("");
                }
            } else {
                isSearchActive = false;
                if (searchText.getString().isEmpty()) {
                    searchText.setString("Search...");
                }
            }
            for (size_t i = 0; i < categoryButtons.size(); ++i) {
                if (categoryButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    handleCategoryClick(i);
                    break;
                }
            }
            for (size_t i = 0; i < contentItems.size(); ++i) {
                if (contentItems[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    handleContentClick(i);
                    break;
                }
            }
            if (loginButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                handleLoginClick();
            }
            if (registerButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                if (registerText.getString() == "Logout") {
                    currentUsername = "";
                    loginText.setString("Login");
                    registerText.setString("Register");
                } else {
                    handleRegisterClick();
                }
            }
            if (clearSearchButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                searchText.setString("");
                handleSearch("");
            }
        }
    }
    else if (event.type == sf::Event::TextEntered && isSearchActive) {
        wchar_t ch = static_cast<wchar_t>(event.text.unicode);
        if ((ch >= L'0' && ch <= L'9') ||
            (ch >= L'A' && ch <= L'Z') || (ch >= L'a' && ch <= L'z') ||
            (ch >= 0x0410 && ch <= 0x044F) || (ch == 0x0401) || (ch == 0x0451) ||
            (ch == L'.' || ch == L',' || ch == L'!' || ch == L'?' || ch == L' ')) {
            searchText.setString(searchText.getString() + static_cast<wchar_t>(ch));
            handleSearch(searchText.getString());
        } else if (event.text.unicode == '\b' && !searchText.getString().isEmpty()) {
            std::wstring current = searchText.getString().toWideString();
            current.pop_back();
            searchText.setString(current);
            handleSearch(searchText.getString());
        }
    }

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::P &&
            sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
            sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
            handleAddContentClick();
        }
        if (event.key.code == sf::Keyboard::D &&
            sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
            sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
            toggleAdminPanel();
        }
    }
}

void MainWindow::update() {
    if (authWindow) {
        authWindow->update(0.016f);
    }
    if (contentDetails) {
        contentDetails->update(0.016f);
    }
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    if (loginButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
        loginButton.setFillColor(sf::Color(64, 64, 64));
    } else {
        loginButton.setFillColor(sf::Color(32, 32, 32));
    }
    if (registerButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
        registerButton.setFillColor(sf::Color(64, 64, 64));
    } else {
        registerButton.setFillColor(sf::Color(32, 32, 32));
    }
    for (size_t i = 0; i < categoryButtons.size(); ++i) {
        if (categoryButtons[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            categoryButtons[i].setFillColor(sf::Color(64, 64, 64));
        } else {
            categoryButtons[i].setFillColor(selectedCategory == i ? sf::Color(64, 64, 64) : sf::Color(32, 32, 32));
        }
    }
    if (clearSearchButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
        clearSearchButton.setFillColor(sf::Color(64, 64, 64));
    } else {
        clearSearchButton.setFillColor(sf::Color(32, 32, 32));
    }
    
    if (activeField >= 0) {
        cursorBlinkTime += 0.05;
        if (cursorBlinkTime >= 0.5) {
            cursorVisible = !cursorVisible;
            cursorBlinkTime = 0;
        }
    }
}

void MainWindow::render() {
    window.draw(background);
    window.draw(logo);
    window.draw(searchBar);
    window.draw(searchText);
    window.draw(clearSearchButton);
    window.draw(clearSearchText);
    window.draw(loginButton);
    window.draw(loginText);
    window.draw(registerButton);
    window.draw(registerText);
    
    for (size_t i = 0; i < categoryButtons.size(); ++i) {
        window.draw(categoryButtons[i]);
        window.draw(categoryTexts[i]);
    }
    for (size_t i = 0; i < contentItems.size(); ++i) {
        window.draw(contentItems[i]);
        window.draw(contentCovers[i]);
        window.draw(contentTitles[i]);
        window.draw(contentYears[i]);
        window.draw(contentGenres[i]);
    }
    
    if (showAddContentPanel) {
        window.draw(addContentPanel);
        window.draw(addContentPanelTitle);
        window.draw(addContentPanelClose);
        window.draw(addContentPanelCloseText);
        
        window.draw(titleLabel);
        window.draw(titleField);
        window.draw(titleFieldText);
        
        window.draw(yearLabel);
        window.draw(yearField);
        window.draw(yearFieldText);
        
        window.draw(categoryLabel);
        window.draw(categoryField);
        window.draw(categoryFieldText);
        
        window.draw(coverPathLabel);
        window.draw(coverPathField);
        window.draw(coverPathFieldText);
        
        window.draw(descriptionLabel);
        window.draw(descriptionField);
        window.draw(descriptionFieldText);
        
        window.draw(genresLabel);
        window.draw(genresField);
        window.draw(genresFieldText);
        
        window.draw(tagsLabel);
        window.draw(tagsField);
        window.draw(tagsFieldText);
        
        window.draw(submitButton);
        window.draw(submitButtonText);
        
        if (cursorVisible) {
            window.draw(cursor);
        }
    }
    
    if (contentDetails) {
        contentDetails->render();
    }
    if (authWindow) {
        authWindow->render();
    }

    if (isAdminPanelVisible) {
        sf::RectangleShape adminBg;
        adminBg.setSize(sf::Vector2f(window.getSize().x - 100, window.getSize().y - 100));
        adminBg.setPosition(50, 50);
        adminBg.setFillColor(sf::Color(24, 24, 24, 240));
        adminBg.setOutlineThickness(2);
        adminBg.setOutlineColor(sf::Color(100, 200, 255));
        window.draw(adminBg);

        sf::Text panelTitle;
        panelTitle.setFont(font);
        panelTitle.setString("Database Management");
        panelTitle.setCharacterSize(24);
        panelTitle.setFillColor(sf::Color(255, 255, 255));
        sf::FloatRect panelTitleBounds = panelTitle.getLocalBounds();
        panelTitle.setPosition(
            50 + (window.getSize().x - 100 - panelTitleBounds.width) / 2,
            60
        );
        window.draw(panelTitle);

        sf::RectangleShape closeBtn;
        closeBtn.setSize(sf::Vector2f(30, 30));
        closeBtn.setPosition(window.getSize().x - 80, 60);
        closeBtn.setFillColor(sf::Color(180, 80, 80));
        window.draw(closeBtn);
        
        sf::Text closeText;
        closeText.setFont(font);
        closeText.setString("X");
        closeText.setCharacterSize(18);
        closeText.setFillColor(sf::Color::White);
        closeText.setPosition(window.getSize().x - 73, 62);
        window.draw(closeText);

        sf::RectangleShape addBtn;
        addBtn.setSize(sf::Vector2f(180, 36));
        addBtn.setPosition(70, 100);
        addBtn.setFillColor(sf::Color(80, 180, 80));
        window.draw(addBtn);
        
        sf::Text addText;
        addText.setFont(font);
        addText.setString("Add Content");
        addText.setCharacterSize(18);
        addText.setFillColor(sf::Color::White);
        addText.setPosition(85, 108);
        window.draw(addText);

        sf::RectangleShape importBtn;
        importBtn.setSize(sf::Vector2f(200, 36));
        importBtn.setPosition(270, 100);
        importBtn.setFillColor(sf::Color(80, 140, 220));
        window.draw(importBtn);
        
        sf::Text importText;
        importText.setFont(font);
        importText.setString("Import from File");
        importText.setCharacterSize(18);
        importText.setFillColor(sf::Color::White);
        importText.setPosition(290, 108);
        window.draw(importText);

        sf::Text countText;
        countText.setFont(font);
        countText.setString("Total records: " + std::to_string(contentList.size()));
        countText.setCharacterSize(16);
        countText.setFillColor(sf::Color(200, 200, 200));
        countText.setPosition(window.getSize().x - 250, 108);
        window.draw(countText);

        sf::RectangleShape headerBg;
        headerBg.setSize(sf::Vector2f(window.getSize().x - 140, 36));
        headerBg.setPosition(70, 150);
        headerBg.setFillColor(sf::Color(40, 80, 120));
        window.draw(headerBg);

        sf::Text idHeader;
        idHeader.setFont(font);
        idHeader.setString("ID");
        idHeader.setCharacterSize(18);
        idHeader.setFillColor(sf::Color::White);
        idHeader.setPosition(80, 158);
        window.draw(idHeader);

        sf::Text titleHeader;
        titleHeader.setFont(font);
        titleHeader.setString("Title");
        titleHeader.setCharacterSize(18);
        titleHeader.setFillColor(sf::Color::White);
        titleHeader.setPosition(120, 158);
        window.draw(titleHeader);

        sf::Text yearHeader;
        yearHeader.setFont(font);
        yearHeader.setString("Year");
        yearHeader.setCharacterSize(18);
        yearHeader.setFillColor(sf::Color::White);
        yearHeader.setPosition(370, 158);
        window.draw(yearHeader);

        sf::Text categoryHeader;
        categoryHeader.setFont(font);
        categoryHeader.setString("Category");
        categoryHeader.setCharacterSize(18);
        categoryHeader.setFillColor(sf::Color::White);
        categoryHeader.setPosition(450, 158);
        window.draw(categoryHeader);

        sf::Text actionsHeader;
        actionsHeader.setFont(font);
        actionsHeader.setString("Actions");
        actionsHeader.setCharacterSize(18);
        actionsHeader.setFillColor(sf::Color::White);
        actionsHeader.setPosition(600, 158);
        window.draw(actionsHeader);

        float rowY = 190;
        float rowHeight = 40;
        for (size_t i = 0; i < contentList.size(); ++i) {
            const auto& c = contentList[i];
            
            sf::RectangleShape rowBg;
            rowBg.setSize(sf::Vector2f(window.getSize().x - 140, rowHeight));
            rowBg.setPosition(70, rowY);
            rowBg.setFillColor(i % 2 == 0 ? sf::Color(32, 32, 32) : sf::Color(40, 40, 40));
            window.draw(rowBg);

            sf::Text idText;
            idText.setFont(font);
            idText.setString(std::to_string(c.id));
            idText.setCharacterSize(16);
            idText.setFillColor(sf::Color(180, 180, 180));
            idText.setPosition(80, rowY + 10);
            window.draw(idText);

            sf::Text titleText;
            std::string titleStr = c.title;
            if (titleStr.length() > 30) {
                titleStr = titleStr.substr(0, 27) + "...";
            }
            titleText.setString(titleStr);
            titleText.setCharacterSize(16);
            titleText.setFillColor(sf::Color(220, 220, 220));
            titleText.setPosition(120, rowY + 10);
            window.draw(titleText);

            sf::Text yearText;
            yearText.setFont(font);
            yearText.setString(std::to_string(c.year));
            yearText.setCharacterSize(16);
            yearText.setFillColor(sf::Color(220, 220, 220));
            yearText.setPosition(370, rowY + 10);
            window.draw(yearText);

            sf::Text categoryText;
            categoryText.setFont(font);
            categoryText.setString(c.category);
            categoryText.setCharacterSize(16);
            categoryText.setFillColor(sf::Color(220, 220, 220));
            categoryText.setPosition(450, rowY + 10);
            window.draw(categoryText);

            sf::RectangleShape editBtn;
            editBtn.setSize(sf::Vector2f(80, 30));
            editBtn.setPosition(600, rowY + 5);
            editBtn.setFillColor(sf::Color(80, 180, 80));
            window.draw(editBtn);
            
            sf::Text editText;
            editText.setFont(font);
            editText.setString("Edit");
            editText.setCharacterSize(14);
            editText.setFillColor(sf::Color::White);
            editText.setPosition(607, rowY + 12);
            window.draw(editText);

            sf::RectangleShape delBtn;
            delBtn.setSize(sf::Vector2f(80, 30));
            delBtn.setPosition(690, rowY + 5);
            delBtn.setFillColor(sf::Color(180, 80, 80));
            window.draw(delBtn);
            
            sf::Text delText;
            delText.setFont(font);
            delText.setString("Delete");
            delText.setCharacterSize(14);
            delText.setFillColor(sf::Color::White);
            delText.setPosition(700, rowY + 12);
            window.draw(delText);

            rowY += rowHeight;
        }
    }

    if (showImportPanel) {
        window.draw(importPanel);
        window.draw(importPanelTitle);
        window.draw(importPanelClose);
        window.draw(importPanelCloseText);
        window.draw(importTextLabel);
        window.draw(loadFileButton);
        window.draw(loadFileButtonText);
        
        sf::Text formatHelpText;
        formatHelpText.setFont(font);
        formatHelpText.setString(
            "title=Movie Title\n"
            "year=2023\n"
            "category=Movies\n"
            "cover=filename.jpg\n"
            "description=Movie description\n"
            "genres=Genre1, Genre2, Genre3\n"
            "tags=tag1, tag2, tag3\n"
            "\n"
            "title=Next movie title\n"
            "year=2022\n"
            "...\n"
        );
        formatHelpText.setCharacterSize(14);
        formatHelpText.setFillColor(sf::Color(180, 180, 180));
        formatHelpText.setPosition(importPanel.getPosition().x + 20, importPanel.getPosition().y + 170);
        window.draw(formatHelpText);
        
        window.draw(importTextField);
        window.draw(importTextContent);
        window.draw(importButton);
        window.draw(importButtonText);
        
        if (cursorVisible && isImportTextActive) {
            sf::RectangleShape importCursor;
            importCursor.setSize(sf::Vector2f(2, 24));
            importCursor.setFillColor(sf::Color::White);
            sf::FloatRect bounds = importTextContent.getGlobalBounds();
            importCursor.setPosition(bounds.left + bounds.width + 2, importTextContent.getPosition().y);
            window.draw(importCursor);
        }
    }
}

void MainWindow::handleSearch(const std::string& query) {
    if (query.empty()) {
        updateContentGrid();
        return;
    }
    contentItems.clear();
    contentTitles.clear();
    contentYears.clear();
    contentGenres.clear();
    contentCovers.clear();
    contentList = database.searchContent(query);
    
    float gridStartX = CATEGORY_WIDTH + 40;
    float gridStartY = 100;
    float itemWidth = 160;
    float itemHeight = 260;
    float gap = 20;
    
    float availableWidth = window.getSize().x - CATEGORY_WIDTH - 60;
    int itemsPerRow = std::max(1, static_cast<int>(availableWidth / (itemWidth + gap)));
    
    for (size_t i = 0; i < contentList.size(); ++i) {
        const auto& content = contentList[i];
        float x = gridStartX + (i % itemsPerRow) * (itemWidth + gap);
        float y = gridStartY + (i / itemsPerRow) * (itemHeight + gap);
        sf::RectangleShape itemRect;
        itemRect.setSize(sf::Vector2f(itemWidth, itemHeight));
        itemRect.setPosition(x, y);
        itemRect.setOutlineThickness(0);
        itemRect.setFillColor(sf::Color(32, 32, 32));
        contentItems.push_back(itemRect);
        
        sf::Texture* coverTexture = new sf::Texture();
        std::string coverPath = "resources/images/covers/" + content.coverPath;
        bool loaded = false;
        if (!coverTexture->loadFromFile(coverPath)) {
            coverPath = "build/resources/images/covers/" + content.coverPath;
            loaded = coverTexture->loadFromFile(coverPath);
        } else {
            loaded = true;
        }
        
        sf::Sprite coverSprite;
        if (loaded) {
            coverSprite.setTexture(*coverTexture);
            float scaleX = 140.0f / coverTexture->getSize().x;
            float scaleY = 200.0f / coverTexture->getSize().y;
            coverSprite.setScale(scaleX, scaleY);
        } else {
            delete coverTexture;
            coverTexture = new sf::Texture();
            sf::Image placeholder;
            placeholder.create(140, 200, sf::Color(80, 80, 80));
            coverTexture->loadFromImage(placeholder);
            coverSprite.setTexture(*coverTexture);
        }
        
        coverSprite.setPosition(x + 10, y + 10);
        contentCovers.push_back(coverSprite);
        
        sf::Text title;
        title.setFont(font);
        title.setCharacterSize(16);
        title.setFillColor(sf::Color::White);
        title.setString(content.title);
        
        wrapText(title, itemWidth - 20, font);
        
        title.setPosition(x + 10, y + 220);
        contentTitles.push_back(title);
        
        sf::Text year;
        year.setFont(font);
        year.setString(std::to_string(content.year));
        year.setCharacterSize(14);
        year.setFillColor(sf::Color(180, 180, 180));
        
        sf::FloatRect titleBounds = title.getGlobalBounds();
        float yearY = titleBounds.top + titleBounds.height + 5;
        year.setPosition(x + 10, yearY);
        contentYears.push_back(year);
        
        sf::Text genres;
        genres.setFont(font);
        std::string genresStr;
        for (size_t j = 0; j < content.genres.size(); ++j) {
            genresStr += content.genres[j];
            if (j + 1 < content.genres.size()) genresStr += ", ";
        }
        
        genres.setString(genresStr);
        
        wrapText(genres, itemWidth - 20, font);
        
        genres.setCharacterSize(12);
        genres.setFillColor(sf::Color(180, 180, 180));
        
        sf::FloatRect yearBounds = year.getGlobalBounds();
        float genresY = yearBounds.top + yearBounds.height + 5;
        genres.setPosition(x + 10, genresY);
        contentGenres.push_back(genres);
    }
}

void MainWindow::handleCategoryClick(int index) {
    if (selectedCategory == index) {
        selectedCategory = 0;
    } else {
        selectedCategory = index;
    }
    updateContentGrid();
}

void MainWindow::handleContentClick(int index) {
    if (index >= 0 && index < contentList.size()) {
        contentDetails->show(contentList[index].id);
    }
}

void MainWindow::handleLoginClick() {
    if (authWindow) {
        authWindow->show();
    }
}

void MainWindow::handleRegisterClick() {
    if (authWindow) {
        authWindow->show();
    }
}

void MainWindow::onLoginSuccess(const std::string& username) {
    currentUsername = username;
    loginText.setString(username);
    registerText.setString("Logout");
}

void MainWindow::toggleAddContentPanel() {
    showAddContentPanel = !showAddContentPanel;
    activeField = -1;
    cursorVisible = false;
    
    if (showAddContentPanel) {
        titleFieldText.setString("");
        yearFieldText.setString("");
        categoryFieldText.setString("");
        coverPathFieldText.setString("");
        descriptionFieldText.setString("");
        genresFieldText.setString("");
        tagsFieldText.setString("");
    }
}

void MainWindow::handleAddContentClick() {
    toggleAddContentPanel();
    if (showAddContentPanel) {
        isAdminPanelVisible = false;
    }
}

void MainWindow::updateCursorPosition() {
    if (activeField == 0) {
        sf::FloatRect bounds = titleFieldText.getGlobalBounds();
        cursor.setPosition(bounds.left + bounds.width + 2, titleFieldText.getPosition().y);
    } else if (activeField == 1) {
        sf::FloatRect bounds = yearFieldText.getGlobalBounds();
        cursor.setPosition(bounds.left + bounds.width + 2, yearFieldText.getPosition().y);
    } else if (activeField == 2) {
        sf::FloatRect bounds = categoryFieldText.getGlobalBounds();
        cursor.setPosition(bounds.left + bounds.width + 2, categoryFieldText.getPosition().y);
    } else if (activeField == 3) {
        sf::FloatRect bounds = coverPathFieldText.getGlobalBounds();
        cursor.setPosition(bounds.left + bounds.width + 2, coverPathFieldText.getPosition().y);
    } else if (activeField == 4) {
        sf::FloatRect bounds = descriptionFieldText.getGlobalBounds();
        cursor.setPosition(bounds.left + bounds.width + 2, descriptionFieldText.getPosition().y);
    } else if (activeField == 5) {
        sf::FloatRect bounds = genresFieldText.getGlobalBounds();
        cursor.setPosition(bounds.left + bounds.width + 2, genresFieldText.getPosition().y);
    } else if (activeField == 6) {
        sf::FloatRect bounds = tagsFieldText.getGlobalBounds();
        cursor.setPosition(bounds.left + bounds.width + 2, tagsFieldText.getPosition().y);
    }
}

void MainWindow::handleAddContentFieldClick(int fieldIndex) {
    activeField = fieldIndex;
    cursorVisible = true;
    cursorBlinkTime = 0;
    updateCursorPosition();
}

void MainWindow::handleAddContentFormSubmit() {
    if (titleFieldText.getString().isEmpty() || 
        yearFieldText.getString().isEmpty() || 
        categoryFieldText.getString().isEmpty()) {
        return;
    }
    
    Content newContent;
    newContent.title = titleFieldText.getString();
    
    try {
        newContent.year = std::stoi(yearFieldText.getString().toAnsiString());
    } catch (const std::exception& e) {
        return;
    }
    
    newContent.category = categoryFieldText.getString();
    newContent.coverPath = coverPathFieldText.getString();
    newContent.description = descriptionFieldText.getString();
    
    std::string genresStr = genresFieldText.getString().toAnsiString();
    std::stringstream genresStream(genresStr);
    std::string genre;
    while (std::getline(genresStream, genre, ',')) {
        genre.erase(0, genre.find_first_not_of(" "));
        genre.erase(genre.find_last_not_of(" ") + 1);
        if (!genre.empty()) {
            newContent.genres.push_back(genre);
        }
    }
    
    std::string tagsStr = tagsFieldText.getString().toAnsiString();
    std::stringstream tagsStream(tagsStr);
    std::string tag;
    while (std::getline(tagsStream, tag, ',')) {
        tag.erase(0, tag.find_first_not_of(" "));
        tag.erase(tag.find_last_not_of(" ") + 1);
        if (!tag.empty()) {
            newContent.tags.push_back(tag);
        }
    }
    
    database.addContent(newContent);
    updateContentGrid();
    toggleAddContentPanel();
}

void MainWindow::toggleAdminPanel() {
    isAdminPanelVisible = !isAdminPanelVisible;
}

void MainWindow::wrapText(sf::Text& text, float maxWidth, const sf::Font& font) {
    std::wstring original = text.getString().toWideString();
    std::wstring wrappedText;
    std::wstring currentLine;
    size_t startPos = 0;
    size_t spacePos = 0;
    bool firstWord = true;
    
    if (original.empty()) {
        return;
    }
    
    while (startPos < original.length()) {
        spacePos = original.find(L' ', startPos);
        if (spacePos == std::wstring::npos) {
            spacePos = original.length();
        }
        
        std::wstring word = original.substr(startPos, spacePos - startPos);
        
        std::wstring testLine;
        if (firstWord) {
            testLine = word;
            firstWord = false;
        } else {
            testLine = currentLine + L" " + word;
        }
        
        sf::Text testText;
        testText.setFont(font);
        testText.setCharacterSize(text.getCharacterSize());
        testText.setString(testLine);
        
        if (testText.getLocalBounds().width > maxWidth) {
            if (!currentLine.empty()) {
                if (!wrappedText.empty()) wrappedText += L"\n";
                wrappedText += currentLine;
                currentLine.clear();
                firstWord = true;
                
                testText.setString(word);
                if (testText.getLocalBounds().width > maxWidth) {
                    std::wstring partialWord;
                    for (size_t i = 0; i < word.length(); ++i) {
                        std::wstring testPart = partialWord + word[i];
                        testText.setString(testPart);
                        
                        if (testText.getLocalBounds().width > maxWidth) {
                            if (!wrappedText.empty()) wrappedText += L"\n";
                            wrappedText += partialWord;
                            partialWord = word[i];
                        } else {
                            partialWord = testPart;
                        }
                    }
                    
                    currentLine = partialWord;
                } else {
                    currentLine = word;
                }
            } else {
                testText.setString(word);
                if (testText.getLocalBounds().width > maxWidth) {
                    std::wstring partialWord;
                    for (size_t i = 0; i < word.length(); ++i) {
                        std::wstring testPart = partialWord + word[i];
                        testText.setString(testPart);
                        
                        if (testText.getLocalBounds().width > maxWidth) {
                            if (!wrappedText.empty()) wrappedText += L"\n";
                            wrappedText += partialWord;
                            partialWord = word[i];
                        } else {
                            partialWord = testPart;
                        }
                    }
                    
                    currentLine = partialWord;
                } else {
                    currentLine = word;
                }
            }
        } else {
            currentLine = testLine;
        }
        
        startPos = spacePos + 1;
        
        if (startPos >= original.length() || spacePos == original.length()) {
            if (!currentLine.empty()) {
                if (!wrappedText.empty()) wrappedText += L"\n";
                wrappedText += currentLine;
            }
            break;
        }
    }
    
    text.setString(wrappedText);
}

void MainWindow::handleImportData() {
    std::string importText = importTextContent.getString().toAnsiString();
    if (importText.empty()) {
        return;
    }
    
    std::stringstream ss(importText);
    std::string line;
    
    Content currentContent;
    bool hasContent = false;
    std::vector<Content> importedContents;
    
    while (std::getline(ss, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) {
            if (hasContent && !currentContent.title.empty()) {
                importedContents.push_back(currentContent);
                currentContent = Content();
                hasContent = false;
            }
            continue;
        }
        
        if (line.find("title=") == 0 && hasContent && !currentContent.title.empty()) {
            importedContents.push_back(currentContent);
            currentContent = Content();
            hasContent = false;
        }
        
        size_t equalPos = line.find('=');
        if (equalPos != std::string::npos) {
            std::string key = line.substr(0, equalPos);
            std::string value = line.substr(equalPos + 1);
            
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            if (key == "title") {
                currentContent.title = value;
                hasContent = true;
            } else if (key == "year") {
                try {
                    currentContent.year = std::stoi(value);
                } catch (const std::exception& e) {
                    currentContent.year = 2023;
                }
            } else if (key == "category") {
                currentContent.category = value;
            } else if (key == "cover") {
                currentContent.coverPath = value;
            } else if (key == "description") {
                currentContent.description = value;
            } else if (key == "genres") {
                currentContent.genres.clear();
                std::string genresStr = value;
                std::stringstream genresStream(genresStr);
                std::string genre;
                while (std::getline(genresStream, genre, ',')) {
                    genre.erase(0, genre.find_first_not_of(" \t"));
                    genre.erase(genre.find_last_not_of(" \t") + 1);
                    if (!genre.empty()) {
                        currentContent.genres.push_back(genre);
                    }
                }
            } else if (key == "tags") {
                currentContent.tags.clear();
                std::string tagsStr = value;
                std::stringstream tagsStream(tagsStr);
                std::string tag;
                while (std::getline(tagsStream, tag, ',')) {
                    tag.erase(0, tag.find_first_not_of(" \t"));
                    tag.erase(tag.find_last_not_of(" \t") + 1);
                    if (!tag.empty()) {
                        currentContent.tags.push_back(tag);
                    }
                }
            }
        }
    }
    
    if (hasContent && !currentContent.title.empty()) {
        importedContents.push_back(currentContent);
    }
    
    for (const auto& content : importedContents) {
        database.addContent(content);
    }
    
    updateContentGrid();
    
    showImportPanel = false;
    isImportTextActive = false;
    importTextContent.setString("");
}

void MainWindow::loadFromTextFile() {
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileNameA(&ofn) == TRUE) {
        std::ifstream file(ofn.lpstrFile);
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            importTextContent.setString(buffer.str());
            file.close();
        }
    }
} 