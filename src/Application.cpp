#include "Application.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <iostream>

Application::Application() 
    : window_(nullptr)
    , initialized_(false)
    , currentCountry_(nullptr)
    , activeTab_(0)
    , showStats_(false)
    , selectedRegion_("All")
{
    dataManager_ = std::make_unique<DataManager>();
}

Application::~Application() {
    shutdown();
}

bool Application::initialize() {
    if (!initWindow()) {
        return false;
    }
    
    if (!initImGui()) {
        return false;
    }
    
    // Start loading countries
    dataManager_->loadCountries([this](bool success, const std::string& message) {
        onDataLoaded(success, message);
    });
    
    initialized_ = true;
    return true;
}

bool Application::initWindow() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // GL 3.3 + GLSL 330
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    window_ = glfwCreateWindow(1280, 720, "Country Explorer - Advanced C++ Project", nullptr, nullptr);
    if (!window_) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1); // Enable vsync
    
    return true;
}

bool Application::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Setup style
    ImGui::StyleColorsDark();
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    return true;
}

void Application::run() {
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();
        
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        // Render UI
        renderUI();
        
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window_, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window_);
    }
}

void Application::shutdown() {
    if (initialized_) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    
    if (window_) {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
}

void Application::renderUI() {
    renderMenuBar();
    
    // Main window
    ImGui::SetNextWindowPos(ImVec2(0, 20));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - 20));
    ImGui::Begin("Country Explorer", nullptr, 
                 ImGuiWindowFlags_NoTitleBar | 
                 ImGuiWindowFlags_NoResize | 
                 ImGuiWindowFlags_NoMove | 
                 ImGuiWindowFlags_NoCollapse);
    
    // Show loading indicator if loading
    if (dataManager_->isLoading()) {
        renderLoadingIndicator();
    } else {
        // Tabs
        if (ImGui::BeginTabBar("MainTabs")) {
            if (ImGui::BeginTabItem("Random Country")) {
                activeTab_ = 0;
                renderRandomTab();
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Search")) {
                activeTab_ = 1;
                renderSearchTab();
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Favorites")) {
                activeTab_ = 2;
                renderFavoritesTab();
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Visited")) {
                activeTab_ = 3;
                renderVisitedTab();
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
    }
    
    ImGui::End();
    
    // Stats window
    if (showStats_) {
        renderStatsWindow();
    }
}

void Application::renderMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Reload Data")) {
                dataManager_->loadCountries([this](bool success, const std::string& message) {
                    onDataLoaded(success, message);
                });
            }
            if (ImGui::MenuItem("Save")) {
                dataManager_->saveUserData();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                glfwSetWindowShouldClose(window_, true);
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Statistics", nullptr, &showStats_);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                // Could show about dialog
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void Application::renderRandomTab() {
    ImGui::Text("Discover a random country!");
    ImGui::Spacing();
    
    if (ImGui::Button("Get Random Country", ImVec2(200, 40))) {
        currentCountry_ = dataManager_->getRandomCountry();
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    if (currentCountry_) {
        renderCountryDetails(currentCountry_);
    } else {
        ImGui::Text("Click the button above to explore a random country!");
    }
}

void Application::renderSearchTab() {
    // Search bar
    char searchBuf[256];
    strncpy(searchBuf, searchQuery_.c_str(), sizeof(searchBuf) - 1);
    searchBuf[sizeof(searchBuf) - 1] = '\0';
    
    ImGui::Text("Search for countries:");
    ImGui::SetNextItemWidth(300);
    if (ImGui::InputText("##search", searchBuf, sizeof(searchBuf))) {
        searchQuery_ = searchBuf;
        if (!searchQuery_.empty()) {
            searchResults_ = dataManager_->searchCountries(searchQuery_);
        } else {
            searchResults_.clear();
        }
    }
    
    ImGui::SameLine();
    
    // Region filter
    std::vector<std::string> regions = dataManager_->getRegions();
    if (ImGui::BeginCombo("Region", selectedRegion_.c_str())) {
        if (ImGui::Selectable("All", selectedRegion_ == "All")) {
            selectedRegion_ = "All";
            searchResults_.clear();
        }
        
        for (const auto& region : regions) {
            bool isSelected = (selectedRegion_ == region);
            if (ImGui::Selectable(region.c_str(), isSelected)) {
                selectedRegion_ = region;
                searchResults_ = dataManager_->filterByRegion(region);
            }
        }
        ImGui::EndCombo();
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Results
    if (!searchResults_.empty()) {
        ImGui::Text("Found %zu countries:", searchResults_.size());
        ImGui::Spacing();
        
        if (ImGui::BeginChild("SearchResults", ImVec2(0, 0), true)) {
            for (size_t i = 0; i < searchResults_.size(); ++i) {
                ImGui::PushID(static_cast<int>(i));
                
                if (ImGui::Selectable(searchResults_[i].name.c_str())) {
                    currentCountry_ = &searchResults_[i];
                    ImGui::OpenPopup("CountryDetails");
                }
                
                ImGui::PopID();
            }
            ImGui::EndChild();
        }
    } else if (!searchQuery_.empty() || selectedRegion_ != "All") {
        ImGui::Text("No countries found.");
    } else {
        ImGui::Text("Enter a country name or select a region to search.");
    }
    
    // Details popup
    if (ImGui::BeginPopupModal("CountryDetails", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (currentCountry_) {
            renderCountryDetails(currentCountry_);
        }
        
        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Application::renderFavoritesTab() {
    auto favorites = dataManager_->getFavorites();
    
    ImGui::Text("Your favorite countries (%zu)", favorites.size());
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    if (favorites.empty()) {
        ImGui::Text("You haven't favorited any countries yet.");
        ImGui::Text("Mark countries as favorites from the Random or Search tabs!");
    } else {
        if (ImGui::BeginChild("FavoritesList", ImVec2(0, 0), true)) {
            for (size_t i = 0; i < favorites.size(); ++i) {
                ImGui::PushID(static_cast<int>(i));
                
                ImGui::Text("%s %s", favorites[i].flagEmoji.c_str(), favorites[i].name.c_str());
                ImGui::SameLine();
                
                if (ImGui::SmallButton("Remove")) {
                    dataManager_->setFavorite(favorites[i].name, false);
                }
                
                ImGui::PopID();
            }
            ImGui::EndChild();
        }
    }
}

void Application::renderVisitedTab() {
    auto visited = dataManager_->getVisited();
    
    ImGui::Text("Countries you've visited (%zu)", visited.size());
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    if (visited.empty()) {
        ImGui::Text("You haven't marked any countries as visited yet.");
        ImGui::Text("Mark countries you've been to from the Random or Search tabs!");
    } else {
        if (ImGui::BeginChild("VisitedList", ImVec2(0, 0), true)) {
            for (size_t i = 0; i < visited.size(); ++i) {
                ImGui::PushID(static_cast<int>(i));
                
                ImGui::Text("%s %s", visited[i].flagEmoji.c_str(), visited[i].name.c_str());
                ImGui::SameLine();
                
                if (ImGui::SmallButton("Unmark")) {
                    dataManager_->setVisited(visited[i].name, false);
                }
                
                ImGui::PopID();
            }
            ImGui::EndChild();
        }
    }
}

void Application::renderCountryDetails(const Country* country, bool showActions) {
    if (!country) return;
    
    ImGui::BeginChild("CountryInfo", ImVec2(0, 0), false);
    
    // Header with flag
    ImGui::Text("%s", country->flagEmoji.c_str());
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.3f, 0.8f, 1.0f, 1.0f), "%s", country->name.c_str());
    
    if (!country->officialName.empty() && country->officialName != country->name) {
        ImGui::TextWrapped("Official: %s", country->officialName.c_str());
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Basic info
    ImGui::Columns(2, nullptr, false);
    
    ImGui::Text("Capital:");
    ImGui::NextColumn();
    ImGui::Text("%s", country->capital.empty() ? "N/A" : country->capital.c_str());
    ImGui::NextColumn();
    
    ImGui::Text("Region:");
    ImGui::NextColumn();
    ImGui::Text("%s", country->region.c_str());
    ImGui::NextColumn();
    
    if (!country->subregion.empty()) {
        ImGui::Text("Subregion:");
        ImGui::NextColumn();
        ImGui::Text("%s", country->subregion.c_str());
        ImGui::NextColumn();
    }
    
    ImGui::Text("Population:");
    ImGui::NextColumn();
    ImGui::Text("%s", country->getFormattedPopulation().c_str());
    ImGui::NextColumn();
    
    ImGui::Text("Area:");
    ImGui::NextColumn();
    ImGui::Text("%s", country->getFormattedArea().c_str());
    ImGui::NextColumn();
    
    ImGui::Text("Languages:");
    ImGui::NextColumn();
    ImGui::TextWrapped("%s", country->getLanguagesString().c_str());
    ImGui::NextColumn();
    
    ImGui::Text("Currencies:");
    ImGui::NextColumn();
    ImGui::TextWrapped("%s", country->getCurrenciesString().c_str());
    ImGui::NextColumn();
    
    if (!country->timezones.empty()) {
        ImGui::Text("Timezone:");
        ImGui::NextColumn();
        ImGui::Text("%s", country->timezones.c_str());
        ImGui::NextColumn();
    }
    
    ImGui::Columns(1);
    
    // Actions
    if (showActions) {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // Copy country name for mutation
        std::string countryName = country->name;
        
        bool isFav = country->isFavorite;
        if (ImGui::Checkbox("Favorite", &isFav)) {
            dataManager_->setFavorite(countryName, isFav);
        }
        
        ImGui::SameLine();
        
        bool isVis = country->isVisited;
        if (ImGui::Checkbox("Visited", &isVis)) {
            dataManager_->setVisited(countryName, isVis);
        }
    }
    
    ImGui::EndChild();
}

void Application::renderLoadingIndicator() {
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) * 0.5f);
    ImGui::SetCursorPosY((ImGui::GetWindowHeight() - 100) * 0.5f);
    
    ImGui::Text("Loading countries...");
    
    // Simple spinner
    static float progress = 0.0f;
    progress += 0.01f;
    if (progress > 1.0f) progress = 0.0f;
    
    ImGui::ProgressBar(progress, ImVec2(200, 0), "");
}

void Application::renderStatsWindow() {
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Statistics", &showStats_)) {
        const auto& countries = dataManager_->getAllCountries();
        auto favorites = dataManager_->getFavorites();
        auto visited = dataManager_->getVisited();
        
        ImGui::Text("Total Countries: %zu", countries.size());
        ImGui::Text("Favorites: %zu", favorites.size());
        ImGui::Text("Visited: %zu", visited.size());
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // Region breakdown
        auto regions = dataManager_->getRegions();
        ImGui::Text("Countries by Region:");
        for (const auto& region : regions) {
            auto regionCountries = dataManager_->filterByRegion(region);
            ImGui::Text("  %s: %zu", region.c_str(), regionCountries.size());
        }
    }
    ImGui::End();
}

void Application::onDataLoaded(bool success, const std::string& message) {
    if (success) {
        std::cout << "Success: " << message << std::endl;
    } else {
        std::cerr << "Error: " << message << std::endl;
    }
}
