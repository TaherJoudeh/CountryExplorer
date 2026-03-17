#pragma once
#include "Country.h"
#include "HTTPClient.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <mutex>
#include <atomic>
#include <functional>

/**
 * @file DataManager.h
 * @brief Manages country data, favorites, and visited countries
 * 
 * This class demonstrates:
 * - STL containers: std::vector, std::unordered_map
 * - File I/O: std::fstream, std::filesystem
 * - Thread synchronization: std::mutex, std::atomic
 * - Async operations with callbacks
 */

class DataManager {
public:
    using DataCallback = std::function<void(bool success, const std::string& message)>;
    
    DataManager();
    ~DataManager();
    
    /**
     * @brief Load all countries from REST Countries API
     * @param callback Function to call when loading completes
     * 
     * Performs async HTTP request to fetch all countries.
     * Uses std::thread internally via HTTPClient.
     */
    void loadCountries(DataCallback callback);
    
    /**
     * @brief Get all countries
     * @return const reference to vector of all countries
     * 
     * Thread-safe access using std::mutex
     */
    const std::vector<Country>& getAllCountries() const;
    
    /**
     * @brief Get random country from the list
     * @return Pointer to random country, or nullptr if list is empty
     */
    const Country* getRandomCountry() const;
    
    /**
     * @brief Search countries by name
     * @param query Search string (case-insensitive)
     * @return Vector of matching countries
     */
    std::vector<Country> searchCountries(const std::string& query) const;
    
    /**
     * @brief Filter countries by region
     * @param region Region name (e.g., "Europe", "Asia")
     * @return Vector of countries in that region
     */
    std::vector<Country> filterByRegion(const std::string& region) const;
    
    /**
     * @brief Get all unique regions
     * @return Vector of region names
     */
    std::vector<std::string> getRegions() const;
    
    /**
     * @brief Mark country as favorite
     * @param countryName Country name
     * @param isFavorite true to add, false to remove
     */
    void setFavorite(const std::string& countryName, bool isFavorite);
    
    /**
     * @brief Mark country as visited
     * @param countryName Country name
     * @param isVisited true to mark visited, false to unmark
     */
    void setVisited(const std::string& countryName, bool isVisited);
    
    /**
     * @brief Get all favorite countries
     * @return Vector of favorite countries
     */
    std::vector<Country> getFavorites() const;
    
    /**
     * @brief Get all visited countries
     * @return Vector of visited countries
     */
    std::vector<Country> getVisited() const;
    
    /**
     * @brief Save favorites and visited to files
     * 
     * Uses std::fstream and std::filesystem
     */
    void saveUserData();
    
    /**
     * @brief Load favorites and visited from files
     * 
     * Uses std::fstream and std::filesystem
     */
    void loadUserData();
    
    /**
     * @brief Check if data is currently loading
     * @return true if loading, false otherwise
     */
    bool isLoading() const;
    
    /**
     * @brief Get country by name (exact match)
     * @param name Country name
     * @return Pointer to country, or nullptr if not found
     */
    Country* getCountryByName(const std::string& name);
    
private:
    // STL containers for data storage
    std::vector<Country> countries_;                    // All countries (std::vector)
    std::unordered_map<std::string, size_t> countryMap_; // Name -> index mapping (std::unordered_map)
    
    // Thread synchronization
    mutable std::mutex dataMutex_;                      // Protects country data (std::mutex)
    std::atomic<bool> loading_;                         // Loading status (std::atomic)
    
    // HTTP client
    HTTPClient httpClient_;
    
    // File paths (using std::filesystem)
    std::string favoritesFilePath_;
    std::string visitedFilePath_;
    
    /**
     * @brief Parse JSON response and populate countries vector
     * @param jsonString JSON response from API
     * @return true if parsing succeeded
     */
    bool parseCountriesJSON(const std::string& jsonString);
    
    /**
     * @brief Create data directory if it doesn't exist
     */
    void ensureDataDirectory();
};
