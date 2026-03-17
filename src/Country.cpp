#include "Country.h"
#include <sstream>
#include <iomanip>
#include <locale>

std::string Country::getFormattedPopulation() const {
    std::ostringstream oss;
    oss.imbue(std::locale(""));
    oss << std::fixed << population;
    return oss.str();
}

std::string Country::getFormattedArea() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << area << " km²";
    return oss.str();
}

std::string Country::getLanguagesString() const {
    if (languages.empty()) return "N/A";
    
    std::string result;
    for (size_t i = 0; i < languages.size(); ++i) {
        result += languages[i];
        if (i < languages.size() - 1) result += ", ";
    }
    return result;
}

std::string Country::getCurrenciesString() const {
    if (currencies.empty()) return "N/A";
    
    std::string result;
    for (size_t i = 0; i < currencies.size(); ++i) {
        result += currencies[i];
        if (i < currencies.size() - 1) result += ", ";
    }
    return result;
}
