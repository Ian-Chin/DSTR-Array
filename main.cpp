#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "patient.hpp"
#include "ageGroupAnalysis.hpp"
#include "billingAnalysis.hpp"
#include "careTypeAnalysis.hpp"
#include "Sortinganalysis.hpp"
#include "Searchanalysis.hpp"

using namespace std;

void displayMenu() {
    cout << "\n";
    cout << "================================================\n";
    cout << "               METROHEALTH SYSTEM\n";
    cout << "               ARRAY IMPLEMENTATION\n";
    cout << "================================================\n";
    cout << "1. Load and Display Facility A\n";
    cout << "2. Load and Display Facility B\n";
    cout << "3. Load and Display Facility C\n";
    cout << "4. Load and Display All Datasets\n";
    cout << "5. Caretype analysis\n";
    cout << "6. Age group analysis\n";
    cout << "7. Total bill cost\n";
    cout << "8. Dataset Summary\n";
    cout << "9. Sorting Analysis (insertion & merge sort)\n";
    cout << "10. Searching Analysis (linear & binary search)\n";
    cout << "11. Exit\n";
    cout << "================================================\n";
    cout << "Enter your choice: ";
}

bool loadDataset(const string& filename, Patient patients[], int& count) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Unable to open file: " << filename << endl;
        cout << "Please check if the file exists in your 'dataset' folder and matches the spelling.\n";
        return false;
    }

    count = 0;
    string line;
    getline(file, line); // Skip header

    while (getline(file, line) && count < MAX_PATIENTS) {
        if (line.empty()) {
            continue;
        }

        stringstream stream(line);
        string value;

        getline(stream, patients[count].patientID, ',');
        getline(stream, value, ',');
        patients[count].age = stoi(value);
        getline(stream, patients[count].careType, ',');
        getline(stream, value, ',');
        patients[count].lengthOfStay = stod(value);
        getline(stream, value, ',');
        patients[count].baseCostPerHour = stod(value);
        getline(stream, value, ',');
        patients[count].daysVisitsPerYear = stoi(value);

        count++;
    }

    return true;
}

void displayDataset(const Patient patients[], int count, const string& facilityName) {
    cout << "\n========== " << facilityName << " ==========\n";
    cout << left
         << setw(12) << "Patient ID"
         << setw(8) << "Age"
         << setw(20) << "Care Type"
         << setw(12) << "Stay(hr)"
         << setw(12) << "Cost/hr"
         << setw(12) << "Visits/Year" << endl;
    cout << string(76, '-') << endl;

    for (int i = 0; i < count; i++) {
        cout << left
             << setw(12) << patients[i].patientID
             << setw(8) << patients[i].age
             << setw(20) << patients[i].careType
             << setw(12) << patients[i].lengthOfStay
             << setw(12) << fixed << setprecision(2) << patients[i].baseCostPerHour
             << setw(12) << patients[i].daysVisitsPerYear << endl;
    }

    cout << "Total patients: " << count << endl;
}

void loadAndDisplayFacility(const string& filename, const string& facilityName) {
    Patient patients[MAX_PATIENTS];
    int count = 0;
    if (loadDataset(filename, patients, count)) {
        displayDataset(patients, count, facilityName);
    }
}

void displayFacilitySummary(const Patient patients[], int count, const string& facilityName) {
    const string line(80, '=');

    double totalStayHours = 0.0;
    double totalMedicalCost = 0.0;

    string careTypes[20];
    int careTypeCounts[20] = {0};
    int careTypeNum = 0;

    for (int i = 0; i < count; i++) {
        totalStayHours += patients[i].lengthOfStay;
        totalMedicalCost += patients[i].lengthOfStay * patients[i].baseCostPerHour * patients[i].daysVisitsPerYear;

        bool found = false;
        for (int j = 0; j < careTypeNum; j++) {
            if (careTypes[j] == patients[i].careType) {
                careTypeCounts[j]++;
                found = true;
                break;
            }
        }
        if (!found && careTypeNum < 20) {
            careTypes[careTypeNum] = patients[i].careType;
            careTypeCounts[careTypeNum] = 1;
            careTypeNum++;
        }
    }

    int ageCounts[NUM_AGE_GROUPS];
    int outOfRange = countAgeGroups(patients, count, ageCounts);

    cout << "\n" << line << "\n";
    cout << "                DATASET SUMMARY - " << facilityName << "\n";
    cout << line << "\n";

    cout << left << setw(28) << "Total Patients"
         << ": " << count << "\n";
    cout << left << setw(28) << "Total Stay Hours"
         << ": " << defaultfloat << setprecision(6) << totalStayHours << " hrs\n";
    cout << left << setw(28) << "Total Medical Cost"
         << ": RM " << fixed << setprecision(2) << totalMedicalCost << "\n";

    cout << "\nAge Groups Present:\n";
    bool anyAgeGroup = false;
    for (int i = 0; i < NUM_AGE_GROUPS; i++) {
        if (ageCounts[i] > 0) {
            cout << "  - " << left << setw(38)
                 << getAgeGroupName(i) << ageCounts[i] << " patients\n";
            anyAgeGroup = true;
        }
    }
    if (outOfRange > 0) {
        cout << "  - " << left << setw(38) << "Unknown / Out of range"
             << outOfRange << " patients\n";
        anyAgeGroup = true;
    }
    if (!anyAgeGroup) {
        cout << "  (none)\n";
    }

    cout << "\nCare Types Available:\n";
    if (careTypeNum == 0) {
        cout << "  (none)\n";
    }
    for (int i = 0; i < careTypeNum; i++) {
        cout << "  - " << left << setw(38) << careTypes[i]
             << careTypeCounts[i] << " patients\n";
    }

    cout << line << "\n";
}

int main() {
    int choice = 0;

    do {
        displayMenu();
        
        // Fix for invalid input (e.g. typing letters like 'd')
        if (!(cin >> choice)) {
            cout << "\nInvalid input. Please enter a number between 1 and 11.\n";
            cin.clear();            // Clear error state
            cin.ignore(10000, '\n'); // Discard bad input from buffer
            continue;               // Jump back to start of loop (menu)
        }

        Patient facilityA[MAX_PATIENTS], facilityB[MAX_PATIENTS], facilityC[MAX_PATIENTS];
        int countA = 0, countB = 0, countC = 0;

        if (choice >= 5 && choice <= 10) {
            bool okA = loadDataset("dataset/dataset1 facility_a.csv", facilityA, countA);
            bool okB = loadDataset("dataset/dataset2 facility_b.csv", facilityB, countB);
            bool okC = loadDataset("dataset/dataset3_facility_c.csv", facilityC, countC);

            if (!okA || !okB || !okC) {
                cout << "\nOperation cancelled due to missing dataset file. Returning to menu...\n";
                continue;
            }
        }

        Patient combined[MAX_PATIENTS * 3];
        int combinedCount = 0;
        if (choice >= 5 && choice <= 10) {
            for (int i = 0; i < countA; i++) combined[combinedCount++] = facilityA[i];
            for (int i = 0; i < countB; i++) combined[combinedCount++] = facilityB[i];
            for (int i = 0; i < countC; i++) combined[combinedCount++] = facilityC[i];
        }

        switch (choice) {
            case 1:
                loadAndDisplayFacility("dataset/dataset1 facility_a.csv", "FACILITY A");
                break;

            case 2:
                loadAndDisplayFacility("dataset/dataset2 facility_b.csv", "FACILITY B");
                break;

            case 3:
                loadAndDisplayFacility("dataset/dataset3_facility_c.csv", "FACILITY C");
                break;

            case 4:
                loadAndDisplayFacility("dataset/dataset1 facility_a.csv", "FACILITY A");
                loadAndDisplayFacility("dataset/dataset2 facility_b.csv", "FACILITY B");
                loadAndDisplayFacility("dataset/dataset3_facility_c.csv", "FACILITY C");
                break;

            case 5:
                careTypeAnalysisMenu(facilityA, countA, facilityB, countB, facilityC, countC, combined, combinedCount);
                break;

            case 6:
                ageGroupAnalysisMenu(facilityA, countA, facilityB, countB, facilityC, countC, combined, combinedCount);
                break;

            case 7:
                billingAnalysisMenu(facilityA, countA, facilityB, countB, facilityC, countC, combined, combinedCount);
                break;

            case 8:
                displayFacilitySummary(facilityA, countA, "FACILITY A");
                displayFacilitySummary(facilityB, countB, "FACILITY B");
                displayFacilitySummary(facilityC, countC, "FACILITY C");
                displayFacilitySummary(combined, combinedCount, "COMBINED (A + B + C)");
                break;

            case 9:
                sortingAnalysisMenu(facilityA, countA, facilityB, countB, facilityC, countC, combined, combinedCount);
                break;

            case 10:
                searchAnalysisMenu(facilityA, countA, facilityB, countB, facilityC, countC, combined, combinedCount);
                break;

            case 11:
                cout << "\nExiting Array implementation...\n";
                break;

            default:
                cout << "\nInvalid choice. Please enter a choice between 1 and 11.\n";
        }
    } while (choice != 11);

    return 0;
}