#include <iostream>
#include "Application.hpp"

int main() {
    try {
        Application app;
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
} 