#include "UI/ContentDetails.hpp"
#include <iostream>
#include <sstream>

ContentDetails::ContentDetails(sf::RenderWindow& window, Database& db)
    : window(window)
    , database(db)
    , m_isVisible(false)
    , content({})
    , currentRating(0)
    , cursorVisible(false)
    , cursorBlinkTime(0.0f)
    , isCommentActive(false)
{
    loadResources();
    initializeUI();
}

ContentDetails::~ContentDetails() {}

void ContentDetails::loadResources() {
    if (!font.loadFromFile("resources/fonts/arial.ttf")) {
        std::cerr << "Error loading font in ContentDetails" << std::endl;
        if (!font.loadFromFile("build/resources/fonts/arial.ttf")) {
            throw std::runtime_error("Failed to load any font");
        }
    }
    backgroundColor = sf::Color(18, 18, 18, 250);
    textColor = sf::Color(240, 240, 240);
    accentColor = sf::Color(0, 245, 208);
}

void ContentDetails::initializeUI() {
    background.setSize(sf::Vector2f(window.getSize()));
    background.setFillColor(backgroundColor);
    
    closeButton.setSize(sf::Vector2f(40, 40));
    closeButton.setFillColor(sf::Color(64, 64, 64));
    closeButton.setPosition(window.getSize().x - 60, 20);
    
    closeText.setFont(font);
    closeText.setString("X");
    closeText.setCharacterSize(24);
    closeText.setFillColor(textColor);
    closeText.setPosition(closeButton.getPosition().x + (closeButton.getSize().x - closeText.getLocalBounds().width) / 2 - 5, closeButton.getPosition().y);
    
    coverImage.setSize(sf::Vector2f(600, 889));
    coverImage.setFillColor(sf::Color(48, 48, 48));
    coverImage.setOutlineThickness(2);
    coverImage.setOutlineColor(sf::Color(64, 64, 64));
    coverImage.setPosition(50, 100);
    
    titleText.setFont(font);
    titleText.setCharacterSize(28);
    titleText.setFillColor(textColor);
    
    genreText.setFont(font);
    genreText.setCharacterSize(18);
    genreText.setFillColor(sf::Color(180, 180, 180));
    
    yearText.setFont(font);
    yearText.setCharacterSize(18);
    yearText.setFillColor(sf::Color(180, 180, 180));
    
    descriptionText.setFont(font);
    descriptionText.setCharacterSize(16);
    descriptionText.setFillColor(textColor);
    
    ratingTitle.setFont(font);
    ratingTitle.setString("Your Rating:");
    ratingTitle.setCharacterSize(20);
    ratingTitle.setFillColor(textColor);
    ratingTitle.setPosition(400, 400);
    
    for (int i = 0; i < 5; ++i) {
        sf::CircleShape star;
        star.setRadius(20);
        star.setFillColor(sf::Color(64, 64, 64));
        star.setPosition(400 + i * 50, 430);
        ratingStars[i] = star;
        
        sf::Text starText;
        starText.setFont(font);
        starText.setString(std::to_string(i + 1));
        starText.setCharacterSize(20);
        starText.setFillColor(textColor);
        starText.setPosition(415 + i * 50, 435);
        ratingTexts[i] = starText;
    }
    
    commentTitle.setFont(font);
    commentTitle.setString("Write a Comment:");
    commentTitle.setCharacterSize(20);
    commentTitle.setFillColor(textColor);
    commentTitle.setPosition(400, 500);
    
    commentInput.setSize(sf::Vector2f(600, 100));
    commentInput.setFillColor(sf::Color(48, 48, 48));
    commentInput.setOutlineThickness(2);
    commentInput.setOutlineColor(sf::Color(64, 64, 64));
    commentInput.setPosition(400, 530);
    
    commentText.setFont(font);
    commentText.setString("");
    commentText.setCharacterSize(16);
    commentText.setFillColor(textColor);
    commentText.setPosition(410, 540);
    
    submitButton.setSize(sf::Vector2f(200, 40));
    submitButton.setFillColor(accentColor);
    submitButton.setPosition(400, 650);
    
    submitText.setFont(font);
    submitText.setString("Submit");
    submitText.setCharacterSize(16);
    submitText.setFillColor(sf::Color::Black);
    submitText.setPosition(submitButton.getPosition().x + (submitButton.getSize().x - submitText.getLocalBounds().width) / 2, submitButton.getPosition().y + 10);
    
    commentsTitle.setFont(font);
    commentsTitle.setString("Comments:");
    commentsTitle.setCharacterSize(20);
    commentsTitle.setFillColor(textColor);
    commentsTitle.setPosition(50, 580);
    
    cursor.setSize(sf::Vector2f(2, 20));
    cursor.setFillColor(sf::Color::White);
    cursor.setPosition(commentText.getPosition());
}

void ContentDetails::show(int contentId) {
    m_isVisible = true;
    content = database.getContentById(contentId);
    currentRating = 0;
    isCommentActive = false;
    commentText.setString("");
    
    if (!coverTexture.loadFromFile("resources/images/covers/" + content.coverPath)) {
        std::cout << "Failed to load cover image: " << content.coverPath << std::endl;
        
        if (!coverTexture.loadFromFile("build/resources/images/covers/" + content.coverPath)) {
            std::cout << "Also failed with build prefix" << std::endl;
            sf::Image placeholder;
            placeholder.create(600, 889, sf::Color(48, 48, 48));
            coverTexture.loadFromImage(placeholder);
            coverImage.setTexture(&coverTexture);
        } else {
            coverImage.setTexture(&coverTexture);
            coverImage.setFillColor(sf::Color::White);
        }
    } else {
        coverImage.setTexture(&coverTexture);
        coverImage.setFillColor(sf::Color::White);
    }
    
    sf::Vector2u texSize = coverTexture.getSize();
    float targetW = 600.f, targetH = 889.f;
    float scale = std::min(targetW / texSize.x, targetH / texSize.y);
    float scaledW = texSize.x * scale;
    float scaledH = texSize.y * scale;
    coverImage.setTextureRect(sf::IntRect(0, 0, texSize.x, texSize.y));
    coverImage.setScale(scale, scale);
    float offsetX = (targetW - scaledW) / 2.f;
    float offsetY = (targetH - scaledH) / 2.f;
    coverImage.setPosition(50 + offsetX, 100 + offsetY);
    
    titleText.setString(content.title);
    
    std::stringstream genresStream;
    for (size_t i = 0; i < content.genres.size(); ++i) {
        genresStream << content.genres[i];
        if (i < content.genres.size() - 1) genresStream << ", ";
    }
    genreText.setString(genresStream.str());
    
    yearText.setString(std::to_string(content.year));
    
    descriptionText.setString(content.description);
    
    float avgRating = database.getAverageRating(content.id);
    averageRatingText.setFont(font);
    averageRatingText.setString("Average Rating: " + std::to_string(avgRating).substr(0, 3) + "/5.0");
    averageRatingText.setCharacterSize(16);
    averageRatingText.setFillColor(sf::Color(180, 180, 180));
    
    updateUI();
    loadComments();
}

void ContentDetails::hide() {
    m_isVisible = false;
    commentObjects.clear();
}

void ContentDetails::updateUI() {
    background.setSize(sf::Vector2f(window.getSize()));
    closeButton.setPosition(window.getSize().x - 60, 20);
    closeText.setPosition(closeButton.getPosition().x + (closeButton.getSize().x - closeText.getLocalBounds().width) / 2 - 5, closeButton.getPosition().y);
    
    float coverX = 50;
    float coverY = 60;
    coverImage.setPosition(coverX, coverY);
    float textBlockX = 670; // ближе к обложке (50 + 600 + 20)
    float textBlockY = 60;  // выше
    
    titleText.setPosition(textBlockX, textBlockY);
    wrapText(titleText, window.getSize().x - textBlockX - 50, font);
    
    sf::FloatRect titleBounds = titleText.getGlobalBounds();
    float genreY = titleBounds.top + titleBounds.height + 15;
    genreText.setPosition(textBlockX, genreY);
    wrapText(genreText, window.getSize().x - textBlockX - 50, font);
    
    sf::FloatRect genreBounds = genreText.getGlobalBounds();
    float yearY = genreBounds.top + genreBounds.height + 15;
    yearText.setPosition(textBlockX, yearY);
    
    sf::FloatRect yearBounds = yearText.getGlobalBounds();
    float descY = yearBounds.top + yearBounds.height + 20;
    descriptionText.setPosition(textBlockX, descY);
    wrapText(descriptionText, window.getSize().x - textBlockX - 50, font);
    
    sf::FloatRect descBounds = descriptionText.getGlobalBounds();
    float ratingY = descBounds.top + descBounds.height + 30;
    
    ratingTitle.setPosition(textBlockX, ratingY);
    averageRatingText.setPosition(textBlockX + 200, ratingY);
    
    float starY = ratingY + 30;
    for (int i = 0; i < 5; ++i) {
        ratingStars[i].setPosition(textBlockX + i * 50, starY);
        ratingTexts[i].setPosition(textBlockX + 15 + i * 50, starY + 5);
    }
    
    commentTitle.setPosition(textBlockX, starY + 60);
    commentInput.setPosition(textBlockX, starY + 90);
    commentText.setPosition(textBlockX + 10, starY + 100);
    submitButton.setPosition(textBlockX, starY + 210);
    submitText.setPosition(submitButton.getPosition().x + (submitButton.getSize().x - submitText.getLocalBounds().width) / 2, submitButton.getPosition().y + 10);
    
    commentsTitle.setPosition(50, 580);
    float commentY = 620;
    for (auto& comment : commentObjects) {
        comment.panel.setPosition(50, commentY);
        comment.username.setPosition(60, commentY + 10);
        comment.rating.setPosition(60, commentY + 30);
        comment.text.setPosition(60, commentY + 55);
        commentY += comment.panel.getSize().y + 10;
    }
}

void ContentDetails::handleEvent(const sf::Event& event) {
    if (!m_isVisible) return;
    
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        hide();
        currentRating = 0;
        commentText.setString("");
        isCommentActive = false;
        return;
    }
    
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        
        if (closeButton.getGlobalBounds().contains(mousePos)) {
            hide();
            currentRating = 0;
            commentText.setString("");
            isCommentActive = false;
            return;
        }
        
        for (int i = 0; i < 5; ++i) {
            if (ratingStars[i].getGlobalBounds().contains(mousePos)) {
                currentRating = i + 1;
                break;
            }
        }
        
        if (submitButton.getGlobalBounds().contains(mousePos)) {
            if (currentRating > 0 && !commentText.getString().isEmpty()) {
                database.saveComment(content.id, commentText.getString().toAnsiString(), currentRating);
                database.saveRating(content.id, currentRating);
                loadComments();
                commentText.setString("");
                currentRating = 0;
                isCommentActive = false;
            }
        }
        
        if (commentInput.getGlobalBounds().contains(mousePos)) {
            isCommentActive = true;
            cursorVisible = true;
            cursorBlinkTime = 0.0f;
        } else {
            isCommentActive = false;
        }
    } else if (event.type == sf::Event::TextEntered && isCommentActive) {
        if (event.text.unicode == 8) {
            if (!commentText.getString().isEmpty()) {
                std::string str = commentText.getString().toAnsiString();
                str.pop_back();
                commentText.setString(str);
            }
        } else if (event.text.unicode >= 32) {
            wchar_t ch = static_cast<wchar_t>(event.text.unicode);
            if ((ch >= L'A' && ch <= L'Z') || (ch >= L'a' && ch <= L'z') || 
                (ch >= L'0' && ch <= L'9') || 
                (ch >= 0x0410 && ch <= 0x044F) || (ch == 0x0401) || (ch == 0x0451) ||
                ch == L' ' || ch == L'.' || ch == L',' || ch == L'!' || ch == L'?' || 
                ch == L':' || ch == L';' || ch == L'-' || ch == L'+') {
                commentText.setString(commentText.getString() + static_cast<wchar_t>(ch));
            }
        }
        
        updateCursorPosition();
    }
}

void ContentDetails::update(float deltaTime) {
    if (!m_isVisible) return;
    
    cursorBlinkTime += deltaTime;
    if (cursorBlinkTime >= 0.8f) {
        cursorBlinkTime = 0;
        cursorVisible = !cursorVisible;
    }
    
    for (int i = 0; i < 5; ++i) {
        if (i < currentRating) {
            ratingStars[i].setFillColor(accentColor);
        } else {
            ratingStars[i].setFillColor(sf::Color(64, 64, 64));
        }
    }
    
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    if (submitButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
        submitButton.setFillColor(sf::Color(0, 220, 180));
    } else {
        submitButton.setFillColor(accentColor);
    }
    
    float avgRating = database.getAverageRating(content.id);
    averageRatingText.setString("Average Rating: " + std::to_string(avgRating).substr(0, 3) + "/5.0");
}

void ContentDetails::render() {
    if (!m_isVisible) return;
    
    window.draw(background);
    window.draw(coverImage);
    window.draw(closeButton);
    window.draw(closeText);
    window.draw(titleText);
    window.draw(genreText);
    window.draw(yearText);
    window.draw(descriptionText);
    window.draw(ratingTitle);
    window.draw(averageRatingText);
    
    for (int i = 0; i < 5; ++i) {
        window.draw(ratingStars[i]);
        window.draw(ratingTexts[i]);
    }
    
    window.draw(commentTitle);
    window.draw(commentInput);
    window.draw(commentText);
    window.draw(submitButton);
    window.draw(submitText);
    
    if (isCommentActive && cursorVisible) {
        window.draw(cursor);
    }
    
    window.draw(commentsTitle);
    for (const auto& comment : commentObjects) {
        window.draw(comment.panel);
        window.draw(comment.username);
        window.draw(comment.rating);
        window.draw(comment.text);
    }
}

void ContentDetails::updateCursorPosition() {
    sf::FloatRect bounds = commentText.getLocalBounds();
    cursor.setPosition(commentText.getPosition().x + bounds.width + 2, commentText.getPosition().y);
}

void ContentDetails::loadComments() {
    commentObjects.clear();
    
    auto comments = database.getCommentsForContent(content.id);
    float commentY = 620;
    
    for (const auto& comment : comments) {
        CommentObject obj;
        
        obj.panel.setSize(sf::Vector2f(300, 100));
        obj.panel.setFillColor(sf::Color(48, 48, 48));
        obj.panel.setOutlineThickness(1);
        obj.panel.setOutlineColor(sf::Color(64, 64, 64));
        obj.panel.setPosition(50, commentY);
        
        obj.username.setFont(font);
        obj.username.setString(comment.username);
        obj.username.setCharacterSize(14);
        obj.username.setFillColor(sf::Color(180, 180, 180));
        obj.username.setPosition(60, commentY + 10);
        
        obj.rating.setFont(font);
        obj.rating.setString("Rating: " + std::to_string(comment.rating) + "/5");
        obj.rating.setCharacterSize(14);
        obj.rating.setFillColor(accentColor);
        obj.rating.setPosition(60, commentY + 30);
        
        obj.text.setFont(font);
        obj.text.setString(comment.text);
        obj.text.setCharacterSize(14);
        obj.text.setFillColor(textColor);
        obj.text.setPosition(60, commentY + 55);
        
        commentY += obj.panel.getSize().y + 10;
        commentObjects.push_back(obj);
    }
}

bool ContentDetails::isVisible() const {
    return m_isVisible;
}

void ContentDetails::wrapText(sf::Text& text, float maxWidth, const sf::Font& font) {
    std::wstring str = text.getString().toWideString();
    
    if (str.empty())
        return;
    
    std::wstring wrappedText;
    std::wstring line;
    std::wstring word;
    
    for (size_t i = 0; i < str.size(); ++i) {
        wchar_t character = str[i];
        
        if (character == L' ' || character == L'\n') {
            sf::Text testText;
            testText.setFont(font);
            testText.setCharacterSize(text.getCharacterSize());
            testText.setString(line + (line.empty() ? L"" : L" ") + word);
            
            if (testText.getLocalBounds().width > maxWidth && !line.empty()) {
                wrappedText += line + L'\n';
                line = word;
            } else {
                if (!line.empty()) line += L' ';
                line += word;
            }
            
            if (character == L'\n') {
                wrappedText += line + L'\n';
                line.clear();
            }
            
            word.clear();
        } else {
            word += character;
            
            sf::Text wordTest;
            wordTest.setFont(font);
            wordTest.setCharacterSize(text.getCharacterSize());
            wordTest.setString(word);
            
            if (wordTest.getLocalBounds().width > maxWidth) {
                if (!line.empty()) {
                    wrappedText += line + L'\n';
                    line.clear();
                }
                
                std::wstring wordWithoutLast = word.substr(0, word.length() - 1);
                wrappedText += wordWithoutLast + L'\n';
                
                word = character;
            }
        }
    }
    
    if (!word.empty()) {
        sf::Text testText;
        testText.setFont(font);
        testText.setCharacterSize(text.getCharacterSize());
        testText.setString(line + (line.empty() ? L"" : L" ") + word);
        
        if (testText.getLocalBounds().width > maxWidth && !line.empty()) {
            wrappedText += line + L'\n';
            wrappedText += word;
        } else {
            if (!line.empty()) line += L' ';
            wrappedText += line + word;
        }
    } else if (!line.empty()) {
        wrappedText += line;
    }
    
    text.setString(wrappedText);
} 