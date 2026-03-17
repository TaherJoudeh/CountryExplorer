# Country Explorer

A C++ desktop application for discovering and exploring countries around the world. It fetches real-time data from the [REST Countries API](https://restcountries.com/) and presents it through an interactive GUI built with [Dear ImGui](https://github.com/ocornut/imgui).

---

## Features

- **Random Country Discovery** — explore a random country with one click
- **Search & Filter** — find countries by name or filter by region
- **Favorites** — bookmark countries you're interested in
- **Travel Tracker** — keep track of countries you've visited
- **Persistent Storage** — favorites and visited lists are saved automatically as JSON
- **Real-time Data** — fetches up-to-date information from REST Countries API

## Tech Stack

| Component | Technology |
|-----------|-----------|
| Language | C++17 |
| Build System | CMake 3.15+ |
| GUI | Dear ImGui 1.90.1 |
| Windowing | GLFW 3.3.9 |
| Graphics | OpenGL 3.3 |
| HTTP | cpp-httplib |
| JSON | nlohmann/json 3.11.3 |

## Prerequisites

**Windows:** Visual Studio 2019+ (with C++ Desktop Development), CMake 3.15+, Git

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get install build-essential cmake git libgl1-mesa-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

**Linux (Fedora):**
```bash
sudo dnf install cmake gcc-c++ git mesa-libGL-devel libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel
```

**macOS:**
```bash
xcode-select --install
brew install cmake git
```

## Building

### 1. Clone & install dependencies

```bash
git clone https://github.com/TaherJoudeh/CountryExplorer
cd CountryExplorer

# Linux / macOS
./setup_linux.sh

# Windows
setup_windows.bat
```

The setup script clones the four required libraries into `libs/`.

### 2. Build

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

Or open the project folder in Visual Studio / VS Code with the CMake Tools extension.

### 3. Run

```bash
# Linux / macOS
./build/bin/CountryExplorer

# Windows
.\build\bin\Release\CountryExplorer.exe
```

## Project Structure

```
CountryExplorer/
├── src/
│   ├── main.cpp            # Entry point
│   ├── Application.cpp     # ImGui interface & main loop
│   ├── Country.cpp         # Country data model
│   ├── DataManager.cpp     # Data management, file I/O, search
│   └── HTTPClient.cpp      # Async HTTP client (threaded)
├── include/
│   ├── Application.h
│   ├── Country.h
│   ├── DataManager.h
│   └── HTTPClient.h
├── libs/                   # Downloaded by setup script (git-ignored)
├── data/                   # Auto-created at runtime
├── CMakeLists.txt
├── setup_linux.sh
├── setup_windows.bat
└── README.md
```

## Architecture

The application is structured around four main classes:

- **`HTTPClient`** — performs async HTTP GET requests on a background thread using `std::thread` and `std::atomic` flags, so the UI never blocks.
- **`DataManager`** — owns the country collection (`std::vector`, `std::unordered_map`), handles JSON parsing, file-based persistence, and search/filter logic. Shared data is protected by `std::mutex`.
- **`Country`** — plain data struct representing a single country (name, capital, population, region, languages, currencies, flag emoji, etc.).
- **`Application`** — sets up the GLFW/OpenGL window, runs the Dear ImGui render loop, and provides the tab-based UI (Random, Search, Favorites, Visited).

## Usage

| Tab | What it does |
|-----|-------------|
| **Random** | Click "Get Random Country" to discover a new country. Mark it as favorite or visited. |
| **Search** | Type a name for real-time filtering, or pick a region from the dropdown. |
| **Favorites** | View and manage your bookmarked countries. |
| **Visited** | View and manage countries you've been to. |

The menu bar offers **File → Reload / Save / Exit** and **View → Statistics**.

## API

This project uses the free, public [REST Countries API](https://restcountries.com/) — no API key required.

Endpoint: `https://restcountries.com/v3.1/all`

## Troubleshooting

- **Build errors** — make sure you ran the setup script first. Try deleting `build/` and rebuilding.
- **No data at runtime** — check your internet connection and firewall settings.
- **Corrupted user data** — delete `data/favorites.json` and `data/visited.json`, then reload from the File menu.

## License

This project is licensed under the [MIT License](LICENSE).

## Acknowledgments

- [REST Countries API](https://restcountries.com/) for free country data
- [Dear ImGui](https://github.com/ocornut/imgui) for the GUI framework
- [cpp-httplib](https://github.com/yhirose/cpp-httplib) for the HTTP client
- [nlohmann/json](https://github.com/nlohmann/json) for JSON parsing
- [GLFW](https://www.glfw.org/) for window management


