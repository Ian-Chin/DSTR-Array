#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "patient.hpp"
#include "ageGroupAnalysis.hpp"
#include "billingAnalysis.hpp"
#include "careTypeAnalysis.hpp"
#include "crossDatasetAnalysis.hpp"
#include "Sortinganalysis.hpp"
#include "Searchanalysis.hpp"

using namespace std;

const int DATASET_COUNT = 3;
const int LIST_COUNT    = DATASET_COUNT + 1;

static void trimLine(string& s) {
    const size_t first = s.find_first_not_of(" \t\r\n");
    if (first == string::npos) { s.clear(); return; }
    const size_t last = s.find_last_not_of(" \t\r\n");
    s = s.substr(first, last - first + 1);
}

static bool readLine(const string& prompt, string& out) {
    cout << prompt;
    if (!getline(cin, out)) return false;
    trimLine(out);
    return true;
}

static int readInt(const string& prompt, int lo, int hi, int onEOF) {
    string line;
    while (readLine(prompt, line)) {
        stringstream ss(line);
        int value = 0;
        char extra = 0;
        if ((ss >> value) && !(ss >> extra) && value >= lo && value <= hi) {
            return value;
        }
        cout << "  Invalid input. Enter a whole number from "
             << lo << " to " << hi << ".\n";
    }
    return onEOF;
}

// Wipe the console so each menu and result starts on a fresh screen.
static void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void pause() {
    string ignored;
    cout << "\n(press Enter to return to the menu) ";
    getline(cin, ignored);
}

// Splits one CSV line into exactly `expected` trimmed fields.
static bool splitCSV(const string& line, string out[], int expected) {
    stringstream ss(line);
    string field;
    int i = 0;

    while (i < expected && getline(ss, field, ',')) {
        trimLine(field);
        out[i++] = field;
    }
    if (i != expected) return false;
    return !getline(ss, field);
}

// Same validation rules as the linked list loader, so both load the same records.
bool loadDataset(const string& filename, Patient patients[], int& count) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "ERROR: cannot open \"" << filename << "\"\n";
        return false;
    }

    const int COLS = 6;
    string line;
    string field[COLS];
    count = 0;

    if (!getline(file, line)) {   // header
        cerr << "ERROR: \"" << filename << "\" is empty\n";
        return false;
    }

    while (getline(file, line) && count < MAX_PATIENTS) {
        trimLine(line);
        if (line.empty()) continue;
        if (!splitCSV(line, field, COLS)) continue;

        Patient p;
        p.patientID = field[0];
        p.careType  = field[2];
        try {
            p.age               = stoi(field[1]);
            p.lengthOfStay      = stoi(field[3]);   // whole hours, as in the linked list version
            p.baseCostPerHour   = stod(field[4]);
            p.daysVisitsPerYear = stoi(field[5]);
        } catch (...) {
            continue;
        }

        if (p.patientID.empty() || p.age < 0 || p.lengthOfStay <= 0 ||
            p.baseCostPerHour <= 0.0 || p.daysVisitsPerYear <= 0) {
            continue;
        }

        careTypeRegistry().indexOf(p.careType);
        patients[count++] = p;
    }

    return true;
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


static bool loadAll(const string files[], Patient* lists[], int counts[]) {
    counts[DATASET_COUNT] = 0;
    for (int i = 0; i < DATASET_COUNT; i++) {
        if (!loadDataset(files[i], lists[i], counts[i])) return false;

        for (int j = 0; j < counts[i]; j++) {
            lists[DATASET_COUNT][counts[DATASET_COUNT]++] = lists[i][j];
        }
    }
    return true;
}

static int pickList(const string labels[]) {
    cout << "Select data source\n";
    for (int i = 0; i < DATASET_COUNT; i++) {
        cout << "  " << (i + 1) << ". " << labels[i] << "\n";
    }
    cout << "  4. " << labels[DATASET_COUNT] << "\n"
         << "  0. Back\n";

    const int choice = readInt("Choice: ", 0, LIST_COUNT, 0);
    return choice - 1;
}

static void printLegend() {
    const string full[NUM_AGE_GROUPS] = {
        "Pediatrics & Adolescents",
        "Young Adults / University Students",
        "Working Adults (Early Career)",
        "Working Adults (Late Career)",
        "Senior Citizens / Geriatric Care"
    };
    cout << "\nAge group legend\n";
    cout << string(60, '-') << "\n";
    for (int g = 0; g < NUM_AGE_GROUPS; g++) {
        cout << left << setw(22) << getAgeGroupName(g) << full[g] << "\n";
    }
}

static void crossDatasetComparison(Patient* lists[], const int counts[],
                                   const string labels[], const string shortLabels[]) {
    DatasetTotals totals[LIST_COUNT];
    for (int i = 0; i < LIST_COUNT; i++) totals[i] = collectDatasetTotals(lists[i], counts[i]);

    printDatasetComparison(totals, labels, DATASET_COUNT, totals[DATASET_COUNT]);
    printCrossMatrix(totals, shortLabels, DATASET_COUNT, totals[DATASET_COUNT], true);
    printCrossMatrix(totals, shortLabels, DATASET_COUNT, totals[DATASET_COUNT], false);
}

static void fullReport(Patient* lists[], const int counts[],
                       const string labels[], const string shortLabels[]) {
    printLegend();

    cout << "\n=== SAMPLE RECORDS (" << labels[0] << ", first 10) ===\n";
    printMatchRows(lists[0], counts[0], 10);

    cout << "\n=== AGE GROUP ANALYSIS (top care type, total & average cost) ===\n";
    for (int i = 0; i < LIST_COUNT; i++) printAgeGroupReport(lists[i], counts[i], labels[i]);

    cout << "\n=== CARE TYPE ANALYSIS (total cost per care type) ===\n";
    for (int i = 0; i < LIST_COUNT; i++) printCareTypeReport(lists[i], counts[i], labels[i]);

    cout << "\n=== CROSS-DATASET COMPARISON ===\n";
    crossDatasetComparison(lists, counts, labels, shortLabels);
}

int main() {
    const string FILES[DATASET_COUNT] = {
        "dataset/dataset1 facility_a.csv",
        "dataset/dataset2 facility_b.csv",
        "dataset/dataset3_facility_c.csv"
    };
    const string LABELS[LIST_COUNT] = {
        "Dataset 1 - Facility A",
        "Dataset 2 - Facility B",
        "Dataset 3 - Facility C",
        "ALL DATASETS COMBINED"
    };
    const string SHORT_LABELS[DATASET_COUNT] = {
        "Dataset 1", "Dataset 2", "Dataset 3"
    };

    // static: keeps ~1400 Patient records off the stack
    static Patient facilityA[MAX_PATIENTS], facilityB[MAX_PATIENTS], facilityC[MAX_PATIENTS];
    static Patient combined[MAX_PATIENTS * DATASET_COUNT];
    Patient* lists[LIST_COUNT] = { facilityA, facilityB, facilityC, combined };
    int counts[LIST_COUNT] = {0};

    if (!loadAll(FILES, lists, counts)) {
        cerr << "Loading failed - check that the dataset folder sits next "
                "to the executable.\n";
        return 1;
    }

    bool running = true;
    while (running) {
        clearScreen();
        cout << "============================================================\n"
             << "  Array Menu\n"
             << "============================================================\n"
             << "  1. Age group legend\n"
             << "  2. Browse records\n"
             << "  3. Age group analysis\n"
             << "  4. Care type analysis\n"
             << "  5. Cross-dataset comparison\n"
             << "  6. Full report (everything above)\n"
             << "  7. Sorting experiment\n"
             << "  8. Searching experiment\n"
             << "  0. Exit\n";

        const int choice = readInt("Enter a Choice: ", 0, 8, 0);
        clearScreen();

        switch (choice) {
            case 1:
                printLegend();
                pause();
                break;

            case 2: {
                const int which = pickList(LABELS);
                if (which < 0) break;
                const int rows = readInt("How many records to display (1-50): ", 1, 50, 10);
                clearScreen();
                cout << "\n" << LABELS[which] << "  (" << counts[which] << " records)\n";
                printMatchRows(lists[which], counts[which], rows);
                pause();
                break;
            }

            case 3: {
                const int which = pickList(LABELS);
                if (which < 0) break;
                clearScreen();
                printAgeGroupReport(lists[which], counts[which], LABELS[which]);
                pause();
                break;
            }

            case 4: {
                const int which = pickList(LABELS);
                if (which < 0) break;
                clearScreen();
                printCareTypeReport(lists[which], counts[which], LABELS[which]);
                pause();
                break;
            }

            case 5:
                crossDatasetComparison(lists, counts, LABELS, SHORT_LABELS);
                pause();
                break;

            case 6:
                fullReport(lists, counts, LABELS, SHORT_LABELS);
                pause();
                break;

            case 7:
                cout << "\n=== SORTING EXPERIMENT ===\n";
                for (int i = 0; i < LIST_COUNT; i++) runSortingExperiment(lists[i], counts[i], LABELS[i]);
                pause();
                break;

            case 8:
                cout << "\n=== SEARCHING EXPERIMENT ===\n";
                for (int i = 0; i < LIST_COUNT; i++) runSearchExperiment(lists[i], counts[i], LABELS[i]);
                pause();
                break;

            case 0:
            default:
                running = false;
                break;
        }
    }

    cout << "\nExiting.\n";
    return 0;
}
