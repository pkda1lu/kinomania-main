#include "Database.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>

Database::Database() : dbPath("resources/data/kinomania.json") {}

Database::~Database() {
    saveData();
}

bool Database::initialize() {
    loadData();
    return true;
}

void Database::loadData() {
    std::ifstream file(dbPath);
    if (file.is_open()) {
        file >> data;
    } else {
        // Initialize empty database structure
        data = {
            {"users", nlohmann::json::array()},
            {"content", nlohmann::json::array()},
            {"comments", nlohmann::json::array()},
            {"ratings", nlohmann::json::array()}
        };
    }
}

void Database::saveData() {
    std::ofstream file(dbPath);
    if (file.is_open()) {
        file << std::setw(4) << data;
    }
}

int Database::getNextId(const std::string& collection) {
    int maxId = 0;
    for (const auto& item : data[collection]) {
        maxId = std::max(maxId, item["id"].get<int>());
    }
    return maxId + 1;
}

std::string Database::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool Database::addContent(const Content& content) {
    nlohmann::json contentJson = {
        {"id", getNextId("content")},
        {"title", content.title},
        {"cover_path", content.coverPath},
        {"year", content.year},
        {"category", content.category},
        {"genres", content.genres},
        {"tags", content.tags},
        {"description", content.description},
        {"created_at", getCurrentTimestamp()}
    };
    
    data["content"].push_back(contentJson);
    saveData();
    return true;
}

bool Database::updateContent(const Content& content) {
    for (auto& item : data["content"]) {
        if (item["id"] == content.id) {
            item["title"] = content.title;
            item["cover_path"] = content.coverPath;
            item["year"] = content.year;
            item["category"] = content.category;
            item["genres"] = content.genres;
            item["tags"] = content.tags;
            item["description"] = content.description;
            saveData();
            return true;
        }
    }
    return false;
}

bool Database::deleteContent(int id) {
    auto& content = data["content"];
    content.erase(
        std::remove_if(content.begin(), content.end(),
            [id](const nlohmann::json& item) { return item["id"] == id; }),
        content.end()
    );
    
    // Also remove associated comments and ratings
    auto& comments = data["comments"];
    comments.erase(
        std::remove_if(comments.begin(), comments.end(),
            [id](const nlohmann::json& item) { return item["content_id"] == id; }),
        comments.end()
    );
    
    auto& ratings = data["ratings"];
    ratings.erase(
        std::remove_if(ratings.begin(), ratings.end(),
            [id](const nlohmann::json& item) { return item["content_id"] == id; }),
        ratings.end()
    );
    
    saveData();
    return true;
}

std::vector<Content> Database::getAllContent() {
    std::vector<Content> result;
    for (const auto& item : data["content"]) {
        Content content;
        content.id = item["id"];
        content.title = item["title"];
        content.coverPath = item["cover_path"];
        content.year = item["year"];
        content.category = item["category"];
        content.description = item["description"];
        content.genres = item["genres"].get<std::vector<std::string>>();
        content.tags = item["tags"].get<std::vector<std::string>>();
        result.push_back(content);
    }
    return result;
}

std::vector<Content> Database::getContentByCategory(const std::string& category) {
    std::vector<Content> result;
    for (const auto& item : data["content"]) {
        if (item["category"] == category) {
            Content content;
            content.id = item["id"];
            content.title = item["title"];
            content.coverPath = item["cover_path"];
            content.year = item["year"];
            content.category = item["category"];
            content.description = item["description"];
            content.genres = item["genres"].get<std::vector<std::string>>();
            content.tags = item["tags"].get<std::vector<std::string>>();
            result.push_back(content);
        }
    }
    return result;
}

Content Database::getContentById(int id) {
    Content content;
    for (const auto& item : data["content"]) {
        if (item["id"] == id) {
            content.id = item["id"];
            content.title = item["title"];
            content.coverPath = item["cover_path"];
            content.year = item["year"];
            content.category = item["category"];
            content.description = item["description"];
            content.genres = item["genres"].get<std::vector<std::string>>();
            content.tags = item["tags"].get<std::vector<std::string>>();
            break;
        }
    }
    return content;
}

bool Database::addComment(const Comment& comment) {
    nlohmann::json commentJson = {
        {"id", getNextId("comments")},
        {"content_id", comment.contentId},
        {"user_id", comment.userId},
        {"username", comment.username},
        {"text", comment.text},
        {"rating", comment.rating},
        {"created_at", getCurrentTimestamp()}
    };
    
    data["comments"].push_back(commentJson);
    saveData();
    return true;
}

std::vector<Comment> Database::getCommentsForContent(int contentId) {
    std::vector<Comment> result;
    for (const auto& item : data["comments"]) {
        if (item["content_id"] == contentId) {
            Comment comment;
            comment.id = item["id"];
            comment.contentId = item["content_id"];
            comment.userId = item["user_id"];
            comment.username = item["username"];
            comment.text = item["text"];
            comment.rating = item["rating"];
            comment.timestamp = item["created_at"];
            result.push_back(comment);
        }
    }
    return result;
}

float Database::getAverageRating(int contentId) {
    float sum = 0;
    int count = 0;
    
    for (const auto& item : data["ratings"]) {
        if (item["content_id"] == contentId) {
            sum += item["rating"].get<float>();
            count++;
        }
    }
    
    return count > 0 ? sum / count : 0;
}

bool Database::authenticateUser(const std::string& username, const std::string& password) {
    for (const auto& user : data["users"]) {
        if (user["username"] == username && user["password"] == password) {
            return true;
        }
    }
    return false;
}

bool Database::registerUser(const std::string& username, const std::string& password, const std::string& email) {
    // Check if username or email already exists
    for (const auto& user : data["users"]) {
        if (user["username"] == username || user["email"] == email) {
            return false;
        }
    }
    
    nlohmann::json userJson = {
        {"id", getNextId("users")},
        {"username", username},
        {"password", password},
        {"email", email},
        {"created_at", getCurrentTimestamp()}
    };
    
    data["users"].push_back(userJson);
    saveData();
    return true;
}

bool Database::isContentEmpty() {
    return data["content"].empty();
}

void Database::fillWithSampleData() {
    // Add sample content
    Content movie1 = {
        0, // id will be set by addContent
        "The Shawshank Redemption",
        "shawshank.jpg",
        1994,
        "movie",
        {"Drama", "Crime"},
        {"prison", "friendship", "hope"},
        "Two imprisoned men bond over a number of years, finding solace and eventual redemption through acts of common decency."
    };
    addContent(movie1);
    
    Content movie2 = {
        0,
        "The Godfather",
        "godfather.jpg",
        1972,
        "movie",
        {"Crime", "Drama"},
        {"mafia", "family", "crime"},
        "The aging patriarch of an organized crime dynasty transfers control of his clandestine empire to his reluctant son."
    };
    addContent(movie2);
    
    // Add test card rd2
    Content rd2 = {
        0,
        "Red Dead Redemption 2",
        "rd2.jpg",
        2018,
        "game",
        {"Action", "Adventure"},
        {"western", "open world", "rockstar"},
        "America, 1899. The end of the wild west era has begun. After a robbery goes wrong, Arthur Morgan and the Van der Linde gang must flee."
    };
    addContent(rd2);
    
    // Add sample users
    registerUser("admin", "admin123", "admin@example.com");
    registerUser("user1", "user123", "user1@example.com");
}

std::vector<Content> Database::searchContent(const std::string& query) {
    std::vector<Content> result;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    for (const auto& item : data["content"]) {
        std::string title = item["title"];
        std::transform(title.begin(), title.end(), title.begin(), ::tolower);
        
        if (title.find(lowerQuery) != std::string::npos) {
            Content content;
            content.id = item["id"];
            content.title = item["title"];
            content.coverPath = item["cover_path"];
            content.year = item["year"];
            content.category = item["category"];
            content.description = item["description"];
            content.genres = item["genres"].get<std::vector<std::string>>();
            content.tags = item["tags"].get<std::vector<std::string>>();
            result.push_back(content);
        }
    }
    return result;
}

void Database::saveRating(int contentId, int rating) {
    nlohmann::json ratingJson = {
        {"content_id", contentId},
        {"rating", rating},
        {"created_at", getCurrentTimestamp()}
    };
    
    data["ratings"].push_back(ratingJson);
    saveData();
}

void Database::saveComment(int contentId, const std::string& text, int rating) {
    Comment comment;
    comment.contentId = contentId;
    comment.text = text;
    comment.rating = rating;
    addComment(comment);
} 