#pragma once
#include "DataManager.h"
#include <memory>
#include <string>

// Forward declarations for ImGui
struct GLFWwindow;

/**
 * @file Application.h
 * @brief Main application class with ImGui interface
 * 
 * Handles:
 * - ImGui window and rendering
 * - User interface for country exploration
 * - Integration with DataManager
 */

class Application {
public:
    Application();
    ~Application();
    
    /**
     * @brief Initialize the application
     * @return true if initialization succeeded
     */
    bool initialize();
    
    /**
     * @brief Run the main application loop
     */
    void run();
    
    /**
     * @brief Cleanup and shutdown
     */
    void shutdown();
    
private:
    // Window
    GLFWwindow* window_;
    bool initialized_;
    
    // Data manager
    std::unique_ptr<DataManager> dataManager_;
    
    // Current state
    const Country* currentCountry_;
    std::string searchQuery_;
    std::string selectedRegion_;
    std::vector<Country> searchResults_;
    
    // UI state
    int activeTab_;  // 0 = Random, 1 = Search, 2 = Favorites, 3 = Visited
    bool showStats_;
    
    /**
     * @brief Initialize GLFW and create window
     */
    bool initWindow();
    
    /**
     * @brief Initialize ImGui
     */
    bool initImGui();
    
    /**
     * @brief Render the user interface
     */
    void renderUI();
    
    /**
     * @brief Render main menu bar
     */
    void renderMenuBar();
    
    /**
     * @brief Render random country tab
     */
    void renderRandomTab();
    
    /**
     * @brief Render search tab
     */
    void renderSearchTab();
    
    /**
     * @brief Render favorites tab
     */
    void renderFavoritesTab();
    
    /**
     * @brief Render visited countries tab
     */
    void renderVisitedTab();
    
    /**
     * @brief Render country details
     * @param country Country to display
     * @param showActions Whether to show favorite/visited buttons
     */
    void renderCountryDetails(const Country* country, bool showActions = true);
    
    /**
     * @brief Render loading indicator
     */
    void renderLoadingIndicator();
    
    /**
     * @brief Render statistics window
     */
    void renderStatsWindow();
    
    /**
     * @brief Callback for data loading completion
     */
    void onDataLoaded(bool success, const std::string& message);
};
