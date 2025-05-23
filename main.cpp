#include <iostream> // For input/output
#include <vector>   // For dynamic arrays (cities, adjacency matrices)
#include <string>   // For city names and file operations
#include <fstream>  // For file input/output
#include <sstream>  // For string stream operations (parsing lines)
#include <iomanip>  // For formatting output (setw, fixed, setprecision)
#include <map>      // For mapping city names to their numerical indices
#include <limits>   // For numeric_limits (clearing input buffer)

using namespace std; // Using the standard namespace for brevity

// Global data structures for easy access across functions
vector<string> cities;      // Stores city names, index + 1 is city ID
map<string, int> cityNameToIndex;        // Maps city name to its 0-based index
vector<vector<int>> roadsMatrix;         // Adjacency matrix for roads (0 or 1)
vector<vector<double>> budgetMatrix;     // Adjacency matrix for road budgets

// --- Helper Functions ---

// Function to resize matrices when a new city is added
void resizeMatrices() {
    int newSize = cities.size();
    roadsMatrix.resize(newSize, vector<int>(newSize, 0));
    budgetMatrix.resize(newSize, vector<double>(newSize, 0.0));

    // Re-initialize new rows/cols to 0 (important if growing existing)
    for (int i = 0; i < newSize; ++i) {
        roadsMatrix[i].resize(newSize, 0);
        budgetMatrix[i].resize(newSize, 0.0);
    }
}

// Get city index by name
int getCityIndex(const string& cityName) {
    if (cityNameToIndex.count(cityName)) {
        return cityNameToIndex[cityName];
    }
    return -1; // Not found
}

// Save cities to cities.txt
void saveCitiesToFile() {
    ofstream outFile("cities.txt");
    if (!outFile.is_open()) {
        cerr << "Error: Could not open cities.txt for writing.\n";
        return;
    }
    outFile << "Index Cityname\n"; // Header
    for (size_t i = 0; i < cities.size(); ++i) {
        outFile << i + 1 << " " << cities[i] << "\n";
    }
    outFile.close();
    cout << "Cities saved to cities.txt.\n";
}

// Save roads and budgets to roads.txt
void saveRoadsToFile() {
    ofstream outFile("roads.txt");
    if (!outFile.is_open()) {
        cerr << "Error: Could not open roads.txt for writing.\n";
        return;
    }
    outFile << "Nbr Road Budget\n"; // Header
    int road_nbr = 1;
    for (size_t i = 0; i < cities.size(); ++i) {
        for (size_t j = i + 1; j < cities.size(); ++j) { // Only half matrix
            if (roadsMatrix[i][j] == 1) {
                outFile << road_nbr++ << ". " << cities[i] << "-" << cities[j]
                        << " " << fixed << setprecision(2) << budgetMatrix[i][j] << "\n";
            }
        }
    }
    outFile.close();
    cout << "Roads and budgets saved to roads.txt.\n";
}

// --- Menu Functions ---

// Menu 1: Add new City(ies)
void addNewCities() {
    int numCitiesToAdd;
    cout << "Enter the number of cities to add: ";
    while (!(cin >> numCitiesToAdd) || numCitiesToAdd <= 0) {
        cout << "Invalid input. Please enter a positive number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer

    for (int i = 0; i < numCitiesToAdd; ++i) {
        string cityName;
        cout << "Enter name of city " << cities.size() + 1 << ": ";
        getline(cin, cityName);
        
        // Simple check for duplicate names (can be improved)
        if (getCityIndex(cityName) != -1) {
            cout << "City '" << cityName << "' already exists. Skipping.\n";
            i--; // Decrement to re-prompt for this city
            continue;
        }

        cities.push_back(cityName);
        cityNameToIndex[cityName] = cities.size() - 1; // Map new city to its index
        resizeMatrices(); // Resize matrices for new city
        cout << "City '" << cityName << "' added with index " << cities.size() << ".\n";
    }
    saveCitiesToFile(); // Save after adding cities
}

// Menu 2: Add roads between cities
void addRoads() {
    string city1Name, city2Name;
    cout << "Enter the name of the first City: ";
    getline(cin, city1Name);
    cout << "Enter the name of the second City: ";
    getline(cin, city2Name);

    int idx1 = getCityIndex(city1Name);
    int idx2 = getCityIndex(city2Name);

    if (idx1 != -1 && idx2 != -1 && idx1 != idx2) {
        roadsMatrix[idx1][idx2] = 1;
        roadsMatrix[idx2][idx1] = 1; // Roads are bidirectional
        cout << "Road added between " << city1Name << " and " << city2Name << ".\n";
    } else {
        cout << "Error: One or both cities not found, or same city.\n";
    }
    saveRoadsToFile(); // Save after adding roads
}

// Menu 3: Add the budget for roads
void addBudgetForRoads() {
    string city1Name, city2Name;
    double budget;
    cout << "Enter the name of the first City: ";
    getline(cin, city1Name);
    cout << "Enter the name of the second City: ";
    getline(cin, city2Name);

    int idx1 = getCityIndex(city1Name);
    int idx2 = getCityIndex(city2Name);

    if (idx1 != -1 && idx2 != -1 && idx1 != idx2) {
        if (roadsMatrix[idx1][idx2] == 1) { // Check if road exists
            cout << "Enter the budget for the road (in Billion Frw): ";
            while (!(cin >> budget) || budget < 0) {
                cout << "Invalid input. Please enter a non-negative number: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer

            budgetMatrix[idx1][idx2] = budget;
            budgetMatrix[idx2][idx1] = budget; // Budget is bidirectional
            cout << "Budget added for the road between " << city1Name << " and " << city2Name << ".\n";
        } else {
            cout << "Error: No road exists between " << city1Name << " and " << city2Name << ".\n";
        }
    } else {
        cout << "Error: One or both cities not found, or same city.\n";
    }
    saveRoadsToFile(); // Save after adding budget
}

// Menu 4: Edit city name
void editCity() {
    int indexToEdit;
    cout << "Enter the index for the city to edit: ";
    while (!(cin >> indexToEdit) || indexToEdit <= 0 || indexToEdit > cities.size()) {
        cout << "Invalid index. Please enter a number between 1 and " << cities.size() << ": ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer

    string newName;
    cout << "Enter the new name for City " << indexToEdit << ": ";
    getline(cin, newName);

    // Update map and vector
    cityNameToIndex.erase(cities[indexToEdit - 1]); // Remove old mapping
    cities[indexToEdit - 1] = newName;
    cityNameToIndex[newName] = indexToEdit - 1; // Add new mapping

    cout << "City updated successfully.\n";
    saveCitiesToFile(); // Save after editing city
}

// Menu 5: Search for a city using index
void searchCityByIndex() {
    int indexToSearch;
    cout << "Enter the index of the city to search: ";
    while (!(cin >> indexToSearch) || indexToSearch <= 0 || indexToSearch > cities.size()) {
        cout << "Invalid index. Please enter a number between 1 and " << cities.size() << ": ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer

    cout << "City at index " << indexToSearch << ": " << cities[indexToSearch - 1] << endl;
}

// Menu 6: Display cities
void displayCities() {
    cout << "\nCities:\n---------------------\n";
    if (cities.empty()) {
        cout << "No cities recorded yet.\n";
        return;
    }
    for (size_t i = 0; i < cities.size(); ++i) {
        cout << i + 1 << ". " << cities[i] << "\n";
    }
}

// Menu 7: Display roads adjacency matrix
void displayRoadsMatrix() {
    if (cities.empty()) {
        cout << "No cities to display roads for. Add cities first.\n";
        return;
    }
    displayCities(); // Show cities before roads
    cout << "\nRoads Adjacency Matrix\n----------------------\n";
    // Print header (city names)
    cout << setw(15) << ""; // Space for row headers
    for (const auto& city : cities) {
        cout << setw(3) << left << city.substr(0, 3); // Abbreviate city names
    }
    cout << "\n";

    for (size_t i = 0; i < roadsMatrix.size(); ++i) {
        cout << setw(15) << left << cities[i];
        for (size_t j = 0; j < roadsMatrix[i].size(); ++j) {
            cout << setw(3) << roadsMatrix[i][j];
        }
        cout << "\n";
    }
}

// Menu 8: Display all recorded data (cities, roads, budgets)
void displayAllData() {
    displayCities();
    displayRoadsMatrix();

    if (cities.empty()) {
        cout << "No cities to display budgets for.\n";
        return;
    }
    cout << "\nBudgets Adjacency Matrix\n------------------------\n";
    // Print header (city names)
    cout << setw(15) << ""; // Space for row headers
    for (const auto& city : cities) {
        cout << setw(7) << left << city.substr(0, 5); // Abbreviate city names
    }
    cout << "\n";

    for (size_t i = 0; i < budgetMatrix.size(); ++i) {
        cout << setw(15) << left << cities[i];
        for (size_t j = 0; j < budgetMatrix[i].size(); ++j) {
            cout << setw(7) << fixed << setprecision(1) << budgetMatrix[i][j];
        }
        cout << "\n";
    }
}

// --- Main Menu and Application Logic ---
void displayMainMenu() {
    cout << "\nROADS-BUDGET-PLAN-CONSOLE-APPLICATION\n";
    cout << "--------------------------------------\n";
    cout << "1. Add new City(ies)\n";
    cout << "2. Add roads between cities\n";
    cout << "3. Add the budget for roads\n";
    cout << "4. Edit city\n";
    cout << "5. Search for a city using index\n";
    cout << "6. Display cities\n";
    cout << "7. Display roads\n";
    cout << "8. Display recorded data on console\n";
    cout << "9. Exit the application\n";
    cout << "Enter your choice: ";
}

int main() {
    int choice;
    do {
        displayMainMenu();
        if (!(cin >> choice)) { // Input validation for menu choice
            cout << "Invalid input. Please enter a number.\n";
            cin.clear(); // Clear error flags
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            continue; // Go back to menu
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer after number input

        switch (choice) {
            case 1: addNewCities(); break;
            case 2: addRoads(); break;
            case 3: addBudgetForRoads(); break;
            case 4: editCity(); break;
            case 5: searchCityByIndex(); break;
            case 6: displayCities(); break;
            case 7: displayRoadsMatrix(); break;
            case 8: displayAllData(); break;
            case 9: cout << "Exiting application. Goodbye!\n"; break;
            default: cout << "Invalid choice. Please try again.\n"; break;
        }
    } while (choice != 9);

    return 0;
}