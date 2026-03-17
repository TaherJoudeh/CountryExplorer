#pragma once
#include <string>
#include <vector>

/**
 * @file Country.h
 * @brief Data structure representing a country with all its information
 * 
 * This structure holds comprehensive country data fetched from the REST Countries API.
 * Uses STL containers for storing multiple values (languages, currencies, etc.)
 */

struct Country {
    std::string name;              // Common name
    std::string officialName;      // Official name
    std::string capital;           // Capital city
    std::string region;            // Geographic region (Europe, Asia, etc.)
    std::string subregion;         // Sub-region
    long population;               // Population count
    double area;                   // Area in km²
    std::string flagUrl;           // URL to flag image (PNG)
    std::string flagEmoji;         // Flag emoji
    std::vector<std::string> languages;     // Spoken languages
    std::vector<std::string> currencies;    // Used currencies
    std::vector<std::string> borders;       // Bordering countries (codes)
    double latitude;               // Geographic coordinates
    double longitude;
    std::string cca2;              // 2-letter country code
    std::string cca3;              // 3-letter country code
    std::string timezones;         // Timezone info
    
    // Additional fields for app functionality
    bool isFavorite = false;       // User marked as favorite
    bool isVisited = false;        // User marked as visited
    
    /**
     * @brief Default constructor
     */
    Country() : population(0), area(0.0), latitude(0.0), longitude(0.0) {}
    
    /**
     * @brief Get formatted population string with commas
     * @return Population as formatted string (e.g., "1,234,567")
     */
    std::string getFormattedPopulation() const;
    
    /**
     * @brief Get formatted area string
     * @return Area as formatted string (e.g., "1,234.56 km²")
     */
    std::string getFormattedArea() const;
    
    /**
     * @brief Get all languages as comma-separated string
     * @return Languages joined by commas
     */
    std::string getLanguagesString() const;
    
    /**
     * @brief Get all currencies as comma-separated string
     * @return Currencies joined by commas
     */
    std::string getCurrenciesString() const;
};
