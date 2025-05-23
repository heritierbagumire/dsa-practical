#include <iostream> // For input/output
#include <vector>   // For dynamic arrays (cities, adjacency matrices)
#include <string>   // For city names and file operations
#include <fstream>  // For file input/output
#include <sstream>  // For string stream operations (parsing lines)
#include <iomanip>  // For formatting output (setw, fixed, setprecision)
#include <map>      // For mapping city names to their numerical indices
#include <limits>   // For numeric_limits (clearing input buffer)

using namespace std;
// Global data structures for easy access across functions
std::vector<std::string> cities;                   // Stores city names, index + 1 is city ID
std::map<std::string, int> cityNameToIndex;        // Maps city name to its 0-based index
std::vector<std::vector<int>> roadsMatrix;         // Adjacency matrix for roads (0 or 1)
std::vector<std::vector<double>> budgetMatrix;     // Adjacency matrix for road budgets

// --- Helper Functions ---

// Function to resize matrices when a new city is added
void resizeMatrices() {
    int newSize = cities.size();
    roadsMatrix.resize(newSize, std::vector<int>(newSize, 0));
    budgetMatrix.resize(newSize, std::vector<double>(newSize, 0.0));

    // Re-initialize new rows/cols to 0 (important if growing existing)
    for (int i = 0; i < newSize; ++i) {
        roadsMatrix[i].resize(newSize, 0);
        budgetMatrix[i].resize(newSize, 0.0);
    }
}

// Get city index by name
int getCityIndex(const std::string& cityName) {
    if (cityNameToIndex.count(cityName)) {
        return cityNameToIndex[cityName];
    }
    return -1; // Not found
}

// Save cities to cities.txt
void saveCitiesToFile() {
    std::ofstream outFile("cities.txt");
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open cities.txt for writing.\n";
        return;
    }
    outFile << "Index Cityname\n"; // Header
    for (size_t i = 0; i < cities.size(); ++i) {
        outFile << i + 1 << " " << cities[i] << "\n";
    }
    outFile.close();
    std::cout << "Cities saved to cities.txt.\n";
}

// Save roads and budgets to roads.txt
void saveRoadsToFile() {
    std::ofstream outFile("roads.txt");
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open roads.txt for writing.\n";
        return;
    }
    outFile << "Nbr Road Budget\n"; // Header
    int road_nbr = 1;
    for (size_t i = 0; i < cities.size(); ++i) {
        for (size_t j = i + 1; j < cities.size(); ++j) { // Only half matrix
            if (roadsMatrix[i][j] == 1) {
                outFile << road_nbr++ << ". " << cities[i] << "-" << cities[j]
                        << " " << std::fixed << std::setprecision(2) << budgetMatrix[i][j] << "\n";
            }
        }
    }
    outFile.close();
    std::cout << "Roads and budgets saved to roads.txt.\n";
}

// --- Menu Functions ---

// Menu 1: Add new City(ies)
void addNewCities() {
    int numCitiesToAdd;
    std::cout << "Enter the number of cities to add: ";
    while (!(std::cin >> numCitiesToAdd) || numCitiesToAdd <= 0) {
        std::cout << "Invalid input. Please enter a positive number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

    for (int i = 0; i < numCitiesToAdd; ++i) {
        std::string cityName;
        std::cout << "Enter name of city " << cities.size() + 1 << ": ";
        std::getline(std::cin, cityName);
        
        // Simple check for duplicate names (can be improved)
        if (getCityIndex(cityName) != -1) {
            std::cout << "City '" << cityName << "' already exists. Skipping.\n";
            i--; // Decrement to re-prompt for this city
            continue;
        }

        cities.push_back(cityName);
        cityNameToIndex[cityName] = cities.size() - 1; // Map new city to its index
        resizeMatrices(); // Resize matrices for new city
        std::cout << "City '" << cityName << "' added with index " << cities.size() << ".\n";
    }
    saveCitiesToFile(); // Save after adding cities
}

// Menu 2: Add roads between cities
void addRoads() {
    std::string city1Name, city2Name;
    std::cout << "Enter the name of the first City: ";
    std::getline(std::cin, city1Name);
    std::cout << "Enter the name of the second City: ";
    std::getline(std::cin, city2Name);

    int idx1 = getCityIndex(city1Name);
    int idx2 = getCityIndex(city2Name);

    if (idx1 != -1 && idx2 != -1 && idx1 != idx2) {
        roadsMatrix[idx1][idx2] = 1;
        roadsMatrix[idx2][idx1] = 1; // Roads are bidirectional
        std::cout << "Road added between " << city1Name << " and " << city2Name << ".\n";
    } else {
        std::cout << "Error: One or both cities not found, or same city.\n";
    }
    saveRoadsToFile(); // Save after adding roads
}

// Menu 3: Add the budget for roads
void addBudgetForRoads() {
    std::string city1Name, city2Name;
    double budget;
    std::cout << "Enter the name of the first City: ";
    std::getline(std::cin, city1Name);
    std::cout << "Enter the name of the second City: ";
    std::getline(std::cin, city2Name);

    int idx1 = getCityIndex(city1Name);
    int idx2 = getCityIndex(city2Name);

    if (idx1 != -1 && idx2 != -1 && idx1 != idx2) {
        if (roadsMatrix[idx1][idx2] == 1) { // Check if road exists
            std::cout << "Enter the budget for the road (in Billion Frw): ";
            while (!(std::cin >> budget) || budget < 0) {
                std::cout << "Invalid input. Please enter a non-negative number: ";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

            budgetMatrix[idx1][idx2] = budget;
            budgetMatrix[idx2][idx1] = budget; // Budget is bidirectional
            std::cout << "Budget added for the road between " << city1Name << " and " << city2Name << ".\n";
        } else {
            std::cout << "Error: No road exists between " << city1Name << " and " << city2Name << ".\n";
        }
    } else {
        std::cout << "Error: One or both cities not found, or same city.\n";
    }
    saveRoadsToFile(); // Save after adding budget
}

// Menu 4: Edit city name
void editCity() {
    int indexToEdit;
    std::cout << "Enter the index for the city to edit: ";
    while (!(std::cin >> indexToEdit) || indexToEdit <= 0 || indexToEdit > cities.size()) {
        std::cout << "Invalid index. Please enter a number between 1 and " << cities.size() << ": ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

    std::string newName;
    std::cout << "Enter the new name for City " << indexToEdit << ": ";
    std::getline(std::cin, newName);

    // Update map and vector
    cityNameToIndex.erase(cities[indexToEdit - 1]); // Remove old mapping
    cities[indexToEdit - 1] = newName;
    cityNameToIndex[newName] = indexToEdit - 1; // Add new mapping

    std::cout << "City updated successfully.\n";
    saveCitiesToFile(); // Save after editing city
}

// Menu 5: Search for a city using index
void searchCityByIndex() {
    int indexToSearch;
    std::cout << "Enter the index of the city to search: ";
    while (!(std::cin >> indexToSearch) || indexToSearch <= 0 || indexToSearch > cities.size()) {
        std::cout << "Invalid index. Please enter a number between 1 and " << cities.size() << ": ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer

    std::cout << "City at index " << indexToSearch << ": " << cities[indexToSearch - 1] << std::endl;
}

// Menu 6: Display cities
void displayCities() {
    std::cout << "\nCities:\n---------------------\n";
    if (cities.empty()) {
        std::cout << "No cities recorded yet.\n";
        return;
    }
    for (size_t i = 0; i < cities.size(); ++i) {
        std::cout << i + 1 << ". " << cities[i] << "\n";
    }
}

// Menu 7: Display roads adjacency matrix
void displayRoadsMatrix() {
    if (cities.empty()) {
        std::cout << "No cities to display roads for. Add cities first.\n";
        return;
    }
    std::cout << "\nRoads Adjacency Matrix\n----------------------\n";
    // Print header (city names)
    std::cout << std::setw(15) << ""; // Space for row headers
    for (const auto& city : cities) {
        std::cout << std::setw(3) << std::left << city.substr(0, 3); // Abbreviate city names
    }
    std::cout << "\n";

    for (size_t i = 0; i < roadsMatrix.size(); ++i) {
        std::cout << std::setw(15) << std::left << cities[i];
        for (size_t j = 0; j < roadsMatrix[i].size(); ++j) {
            std::cout << std::setw(3) << roadsMatrix[i][j];
        }
        std::cout << "\n";
    }
}

// Menu 8: Display all recorded data (cities, roads, budgets)
void displayAllData() {
    displayCities();
    displayRoadsMatrix();

    if (cities.empty()) {
        std::cout << "No cities to display budgets for.\n";
        return;
    }
    std::cout << "\nBudgets Adjacency Matrix\n------------------------\n";
    // Print header (city names)
    std::cout << std::setw(15) << ""; // Space for row headers
    for (const auto& city : cities) {
        std::cout << std::setw(7) << std::left << city.substr(0, 5); // Abbreviate city names
    }
    std::cout << "\n";

    for (size_t i = 0; i < budgetMatrix.size(); ++i) {
        std::cout << std::setw(15) << std::left << cities[i];
        for (size_t j = 0; j < budgetMatrix[i].size(); ++j) {
            std::cout << std::setw(7) << std::fixed << std::setprecision(1) << budgetMatrix[i][j];
        }
        std::cout << "\n";
    }
}

// --- Main Menu and Application Logic ---
void displayMainMenu() {
    std::cout << "\nROADS-BUDGET-PLAN-CONSOLE-APPLICATION\n";
    std::cout << "--------------------------------------\n";
    std::cout << "1. Add new City(ies)\n";
    std::cout << "2. Add roads between cities\n";
    std::cout << "3. Add the budget for roads\n";
    std::cout << "4. Edit city\n";
    std::cout << "5. Search for a city using index\n";
    std::cout << "6. Display cities\n";
    std::cout << "7. Display roads\n";
    std::cout << "8. Display recorded data on console\n";
    std::cout << "9. Exit the application\n";
    std::cout << "Enter your choice: ";
}

int main() {
    // Populate initial data as per the problem description (optional, can be done via menu)
    // For initial setup, we will add cities first, then roads and budgets.
    // The menu system will handle adding new ones.

    // Initial setup with predefined cities (Menu 1 will add them dynamically)
    // cities = {"Kigali", "Huye", "Muhanga", "Musanze", "Nyagatare", "Rubavu", "Rusizi"};
    // for(size_t i=0; i<cities.size(); ++i) cityNameToIndex[cities[i]] = i;
    // resizeMatrices(); // Initialize matrices for existing cities

    int choice;
    do {
        displayMainMenu();
        if (!(std::cin >> choice)) { // Input validation for menu choice
            std::cout << "Invalid input. Please enter a number.\n";
            std::cin.clear(); // Clear error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            continue; // Go back to menu
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer after number input

        switch (choice) {
            case 1: addNewCities(); break;
            case 2: addRoads(); break;
            case 3: addBudgetForRoads(); break;
            case 4: editCity(); break;
            case 5: searchCityByIndex(); break;
            case 6: displayCities(); break;
            case 7: displayRoadsMatrix(); break;
            case 8: displayAllData(); break;
            case 9: std::cout << "Exiting application. Goodbye!\n"; break;
            default: std::cout << "Invalid choice. Please try again.\n"; break;
        }
    } while (choice != 9);

    return 0;
}