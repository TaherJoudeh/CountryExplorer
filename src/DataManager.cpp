#include "DataManager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <random>
#include <iostream>

using json = nlohmann::json;
namespace fs = std::filesystem;

DataManager::DataManager() : loading_(false) {
    favoritesFilePath_ = "data/favorites.json";
    visitedFilePath_ = "data/visited.json";
    ensureDataDirectory();
}

DataManager::~DataManager() {
    saveUserData();
}

void DataManager::loadCountries(DataCallback callback) {
    if (loading_.load()) {
        if (callback) callback(false, "Already loading");
        return;
    }
    
    loading_.store(true);
    
    // API endpoint for all countries
    std::string url = "https://restcountries.com/v3.1/all";
    
    httpClient_.getAsync(url, [this, callback](bool success, const std::string& response) {
        if (success) {
            bool parsed = parseCountriesJSON(response);
            loading_.store(false);
            
            if (parsed) {
                loadUserData(); // Load saved favorites and visited
                if (callback) callback(true, "Loaded " + std::to_string(countries_.size()) + " countries");
            } else {
                if (callback) callback(false, "Failed to parse JSON");
            }
        } else {
            loading_.store(false);
            if (callback) callback(false, "HTTP request failed");
        }
    });
}

bool DataManager::parseCountriesJSON(const std::string& jsonString) {
    try {
        std::lock_guard<std::mutex> lock(dataMutex_);
        
        json countriesJson = json::parse(jsonString);
        countries_.clear();
        countryMap_.clear();
        
        for (const auto& item : countriesJson) {
            Country country;
            
            // Basic info
            country.name = item.value("name", json::object()).value("common", "Unknown");
            country.officialName = item.value("name", json::object()).value("official", "");
            
            // Capital
            if (item.contains("capital") && item["capital"].is_array() && !item["capital"].empty()) {
                country.capital = item["capital"][0];
            }
            
            // Region
            country.region = item.value("region", "");
            country.subregion = item.value("subregion", "");
            
            // Population and area
            country.population = item.value("population", 0);
            country.area = item.value("area", 0.0);
            
            // Flag
            if (item.contains("flags") && item["flags"].is_object()) {
                country.flagUrl = item["flags"].value("png", "");
                country.flagEmoji = item.value("flag", "");
            }
            
            // Languages
            if (item.contains("languages") && item["languages"].is_object()) {
                for (auto& [key, value] : item["languages"].items()) {
                    country.languages.push_back(value.get<std::string>());
                }
            }
            
            // Currencies
            if (item.contains("currencies") && item["currencies"].is_object()) {
                for (auto& [key, value] : item["currencies"].items()) {
                    std::string currencyName = value.value("name", key);
                    country.currencies.push_back(currencyName);
                }
            }
            
            // Borders
            if (item.contains("borders") && item["borders"].is_array()) {
                for (const auto& border : item["borders"]) {
                    country.borders.push_back(border.get<std::string>());
                }
            }
            
            // Coordinates
            if (item.contains("latlng") && item["latlng"].is_array() && item["latlng"].size() >= 2) {
                country.latitude = item["latlng"][0];
                country.longitude = item["latlng"][1];
            }
            
            // Country codes
            country.cca2 = item.value("cca2", "");
            country.cca3 = item.value("cca3", "");
            
            // Timezones
            if (item.contains("timezones") && item["timezones"].is_array() && !item["timezones"].empty()) {
                country.timezones = item["timezones"][0];
            }
            
            // Add to vector and map
            countryMap_[country.name] = countries_.size();
            countries_.push_back(country);
        }
        
        return true;
        
    } catch (const json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

const std::vector<Country>& DataManager::getAllCountries() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return countries_;
}

const Country* DataManager::getRandomCountry() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (countries_.empty()) return nullptr;
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, static_cast<int>(countries_.size() - 1));
    
    return &countries_[dis(gen)];
}

std::vector<Country> DataManager::searchCountries(const std::string& query) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::vector<Country> results;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    for (const auto& country : countries_) {
        std::string lowerName = country.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        
        if (lowerName.find(lowerQuery) != std::string::npos) {
            results.push_back(country);
        }
    }
    
    return results;
}

std::vector<Country> DataManager::filterByRegion(const std::string& region) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::vector<Country> results;
    for (const auto& country : countries_) {
        if (country.region == region) {
            results.push_back(country);
        }
    }
    
    return results;
}

std::vector<std::string> DataManager::getRegions() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::vector<std::string> regions;
    for (const auto& country : countries_) {
        if (!country.region.empty() && 
            std::find(regions.begin(), regions.end(), country.region) == regions.end()) {
            regions.push_back(country.region);
        }
    }
    
    std::sort(regions.begin(), regions.end());
    return regions;
}

void DataManager::setFavorite(const std::string& countryName, bool isFavorite) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    auto it = countryMap_.find(countryName);
    if (it != countryMap_.end()) {
        countries_[it->second].isFavorite = isFavorite;
    }
}

void DataManager::setVisited(const std::string& countryName, bool isVisited) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    auto it = countryMap_.find(countryName);
    if (it != countryMap_.end()) {
        countries_[it->second].isVisited = isVisited;
    }
}

std::vector<Country> DataManager::getFavorites() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::vector<Country> favorites;
    for (const auto& country : countries_) {
        if (country.isFavorite) {
            favorites.push_back(country);
        }
    }
    
    return favorites;
}

std::vector<Country> DataManager::getVisited() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::vector<Country> visited;
    for (const auto& country : countries_) {
        if (country.isVisited) {
            visited.push_back(country);
        }
    }
    
    return visited;
}

void DataManager::saveUserData() {
    try {
        std::lock_guard<std::mutex> lock(dataMutex_);
        
        // Save favorites
        json favoritesJson = json::array();
        for (const auto& country : countries_) {
            if (country.isFavorite) {
                favoritesJson.push_back(country.name);
            }
        }
        
        std::ofstream favFile(favoritesFilePath_);
        if (favFile.is_open()) {
            favFile << favoritesJson.dump(2);
            favFile.close();
        }
        
        // Save visited
        json visitedJson = json::array();
        for (const auto& country : countries_) {
            if (country.isVisited) {
                visitedJson.push_back(country.name);
            }
        }
        
        std::ofstream visFile(visitedFilePath_);
        if (visFile.is_open()) {
            visFile << visitedJson.dump(2);
            visFile.close();
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving user data: " << e.what() << std::endl;
    }
}

void DataManager::loadUserData() {
    try {
        std::lock_guard<std::mutex> lock(dataMutex_);
        
        // Load favorites
        if (fs::exists(favoritesFilePath_)) {
            std::ifstream favFile(favoritesFilePath_);
            if (favFile.is_open()) {
                json favoritesJson;
                favFile >> favoritesJson;
                favFile.close();
                
                for (const auto& name : favoritesJson) {
                    std::string countryName = name.get<std::string>();
                    auto it = countryMap_.find(countryName);
                    if (it != countryMap_.end()) {
                        countries_[it->second].isFavorite = true;
                    }
                }
            }
        }
        
        // Load visited
        if (fs::exists(visitedFilePath_)) {
            std::ifstream visFile(visitedFilePath_);
            if (visFile.is_open()) {
                json visitedJson;
                visFile >> visitedJson;
                visFile.close();
                
                for (const auto& name : visitedJson) {
                    std::string countryName = name.get<std::string>();
                    auto it = countryMap_.find(countryName);
                    if (it != countryMap_.end()) {
                        countries_[it->second].isVisited = true;
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading user data: " << e.what() << std::endl;
    }
}

bool DataManager::isLoading() const {
    return loading_.load();
}

Country* DataManager::getCountryByName(const std::string& name) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    auto it = countryMap_.find(name);
    if (it != countryMap_.end()) {
        return &countries_[it->second];
    }
    
    return nullptr;
}

void DataManager::ensureDataDirectory() {
    try {
        fs::create_directories("data");
    } catch (const std::exception& e) {
        std::cerr << "Error creating data directory: " << e.what() << std::endl;
    }
}
