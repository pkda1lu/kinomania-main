#include "UI/MainWindow.hpp"

void MainWindow::updateContentGrid() {
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
        std::string coverPath = "resources/covers/" + content.coverPath;
        if (!coverTexture->loadFromFile(coverPath)) {
            coverPath = "build/resources/covers/" + content.coverPath;
            if (!coverTexture->loadFromFile(coverPath)) {
                coverTexture = nullptr;
            }
        }
        
        sf::Sprite coverSprite;
        if (coverTexture) {
            coverSprite.setTexture(*coverTexture);
            float scaleX = 140.0f / coverTexture->getSize().x;
            float scaleY = 200.0f / coverTexture->getSize().y;
            coverSprite.setScale(scaleX, scaleY);
        } else {
            sf::RectangleShape* placeholderRect = new sf::RectangleShape(sf::Vector2f(140, 200));
            placeholderRect->setFillColor(sf::Color(60, 60, 60));
            placeholderRect->setPosition(x + 10, y + 10);
            window.draw(*placeholderRect);
            delete placeholderRect;
        }
        
        coverSprite.setPosition(x + 10, y + 10);
        contentCovers.push_back(coverSprite);
    }
} 