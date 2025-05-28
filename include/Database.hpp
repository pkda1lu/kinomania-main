#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include <chrono>
#include <iomanip>
#include <sstream>

struct Content {
    int id;
    std::string title;
    std::string coverPath;
    int year;
    std::string category;
    std::vector<std::string> genres;
    std::vector<std::string> tags;
    std::string description;
};

struct Comment {
    int id;
    int contentId;
    int userId;
    std::string username;
    std::string text;
    int rating;
    std::string timestamp;
};

struct User {
    int id;
    std::string username;
    std::string password;
    std::string email;
    std::string createdAt;
};

class Database {
public:
    Database();
    ~Database();

    bool initialize();
    bool addContent(const Content& content);
    bool updateContent(const Content& content);
    bool deleteContent(int id);
    std::vector<Content> getAllContent();
    std::vector<Content> getContentByCategory(const std::string& category);
    Content getContentById(int id);
    
    bool addComment(const Comment& comment);
    std::vector<Comment> getCommentsForContent(int contentId);
    float getAverageRating(int contentId);
    
    bool authenticateUser(const std::string& username, const std::string& password);
    bool registerUser(const std::string& username, const std::string& password, const std::string& email);

    bool isContentEmpty();
    void fillWithSampleData();

    std::vector<Content> searchContent(const std::string& query);
    void saveRating(int contentId, int rating);
    void saveComment(int contentId, const std::string& text, int rating);

private:
    nlohmann::json data;
    std::string dbPath;
    
    void loadData();
    void saveData();
    int getNextId(const std::string& collection);
    std::string getCurrentTimestamp();
}; 