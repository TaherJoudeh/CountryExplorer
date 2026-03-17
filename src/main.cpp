#include "Application.h"
#include <iostream>

/**
 * @file main.cpp
 * @brief Entry point for Country Explorer application
 * 
 * Advanced C++ Project - Course Requirements:
 * ✓ STL: vector, unordered_map, fstream, filesystem
 * ✓ Threads: std::thread for async HTTP requests
 * ✓ Synchronization: std::atomic, std::mutex
 * ✓ Third-party libraries: httplib, nlohmann/json, imgui
 * ✓ HTTP Client: Downloads JSON from REST Countries API
 * ✓ GUI: ImGui-based user interface
 * 
 * @author Your Name
 * @date 2025
 */

int main() {
    try {
        Application app;
        
        std::cout << "==================================" << std::endl;
        std::cout << "   Country Explorer v1.0" << std::endl;
        std::cout << "   Advanced C++ Project" << std::endl;
        std::cout << "==================================" << std::endl;
        std::cout << std::endl;
        
        if (!app.initialize()) {
            std::cerr << "Failed to initialize application!" << std::endl;
            return 1;
        }
        
        std::cout << "Application initialized successfully!" << std::endl;
        std::cout << "Loading country data from REST Countries API..." << std::endl;
        std::cout << std::endl;
        
        app.run();
        
        std::cout << std::endl;
        std::cout << "Application closing..." << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
