#ifndef SEARCH_ANALYSIS_HPP
#define SEARCH_ANALYSIS_HPP
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

#include "patient.hpp"
#include "Sortinganalysis.hpp"

struct SearchCriteria {
    bool        useAgeRange;
    int         minAge, maxAge;
    bool        useCareType;
    std::string careType;

    SearchCriteria()
        : useAgeRange(false), minAge(0), maxAge(0), useCareType(false) {}
};

inline bool matchesCriteria(const Patient& p, const SearchCriteria& c) {
    if (c.useAgeRange && (p.age < c.minAge || p.age > c.maxAge)) return false;
    if (c.useCareType && p.careType != c.careType)               return false;
    return true;
}

struct SearchStats {
    std::string label;
    long   comparisons;
    double timeMs;
    int    matches;
};

typedef std::chrono::high_resolution_clock SearchClock;

inline SearchStats linearSearch(const Patient arr[], int n, const SearchCriteria& crit,
                                Patient results[]) {
    SearchStats s;
    s.label = "Linear (unsorted)";
    s.comparisons = 0;
    s.matches = 0;

    SearchClock::time_point start = SearchClock::now();
    for (int i = 0; i < n; i++) {
        s.comparisons++;
        if (matchesCriteria(arr[i], crit)) {
            results[s.matches++] = arr[i];
        }
    }
    SearchClock::time_point end = SearchClock::now();

    s.timeMs = std::chrono::duration<double, std::milli>(end - start).count();
    return s;
}

inline int binarySearchByKey(const Patient sorted[], int n, double (*keyOf)(const Patient&),
                             double target, SearchStats& stats) {
    stats.label = "Binary (sorted)";
    stats.comparisons = 0;
    stats.matches = 0;

    SearchClock::time_point start = SearchClock::now();
    int found = -1;
    int low = 0, high = n - 1;
    while (low <= high && found < 0) {
        int mid = low + (high - low) / 2;
        double key = keyOf(sorted[mid]);
        stats.comparisons++;
        if (key == target)     found = mid;
        else if (key < target) low = mid + 1;
        else                   high = mid - 1;
    }
    SearchClock::time_point end = SearchClock::now();

    stats.timeMs = std::chrono::duration<double, std::milli>(end - start).count();
    if (found >= 0) stats.matches = 1;
    return found;
}

// If your Patient struct already has an ageGroup field, use that instead.
inline std::string ageGroupOf(const Patient& p) {
    if (p.age >= 61) return "61-100 Senior";
    return "Other";
}

inline void printSearchTable(const SearchStats stats[], int count) {
    std::cout << std::endl;
    std::cout << "Step 7 - Search performance (array)" << std::endl;
    std::cout << std::left  << std::setw(22) << "SearchType";
    std::cout << std::right << std::setw(14) << "Comparisons";
    std::cout << std::right << std::setw(12) << "Time(ms)";
    std::cout << std::right << std::setw(10) << "Matches" << std::endl;
    std::cout << std::string(58, '-') << std::endl;
    for (int i = 0; i < count; i++) {
        std::cout << std::left  << std::setw(22) << stats[i].label;
        std::cout << std::right << std::setw(14) << stats[i].comparisons;
        std::cout << std::fixed << std::setprecision(4);
        std::cout << std::right << std::setw(12) << stats[i].timeMs;
        std::cout << std::right << std::setw(10) << stats[i].matches << std::endl;
    }
}

inline void printMatchRows(const Patient rows[], int count) {
    std::cout << "Matches for Age 61-100 + CareType=Emergency (" << count << " found):" << std::endl;
    std::cout << std::left  << std::setw(10) << "PatientID"
              << std::left  << std::setw(5)  << "Age"
              << std::left  << std::setw(21) << "AgeGroup"
              << std::left  << std::setw(20) << "CareType"
              << std::right << std::setw(5)  << "Hours"
              << std::right << std::setw(9)  << "Rate"
              << std::right << std::setw(8)  << "Visits"
              << std::right << std::setw(14) << "Cost(RM)" << std::endl;
    std::cout << std::string(92, '-') << std::endl;

    for (int i = 0; i < count; i++) {
        std::cout << std::left  << std::setw(10) << rows[i].patientID
                  << std::left  << std::setw(5)  << rows[i].age
                  << std::left  << std::setw(21) << ageGroupOf(rows[i])
                  << std::left  << std::setw(20) << rows[i].careType
                  << std::right << std::fixed << std::setprecision(0)
                  << std::setw(5) << rows[i].lengthOfStay
                  << std::setprecision(2)
                  << std::setw(9) << rows[i].baseCostPerHour
                  << std::setw(8) << static_cast<int>(rows[i].daysVisitsPerYear)
                  << std::setw(14) << getMedicalCost(rows[i]) << std::endl;
    }
}

inline void runSearchExperiment(const Patient patients[], int count, const std::string& title) {
    std::cout << "\n-- " << title << " --\n";

    Patient* results = new Patient[count];
    Patient* sorted  = new Patient[count];

    SearchCriteria crit;
    crit.useAgeRange = true;  crit.minAge = 61;  crit.maxAge = 100;
    crit.useCareType = true;  crit.careType = "Emergency";

    SearchStats a[2];
    a[0] = linearSearch(patients, count, crit, results);
    const int matches = a[0].matches;

    for (int i = 0; i < count; i++) sorted[i] = patients[i];
    long cmp = 0, mv = 0;
    mergeSort(sorted, count, byAgeAsc, cmp, mv);

    int hit = binarySearchByKey(sorted, count, ageOf, 65.0, a[1]);

    printSearchTable(a, 2);
    printMatchRows(results, matches);
    if (hit >= 0) std::cout << "Binary search found Age==65: " << sorted[hit].patientID << std::endl;
    else          std::cout << "Binary search: no exact Age==65 match in this list" << std::endl;

    delete[] sorted;
    delete[] results;
}

inline void searchAnalysisMenu(const Patient facilityA[], int countA,
                               const Patient facilityB[], int countB,
                               const Patient facilityC[], int countC,
                               const Patient combined[], int combinedCount) {
    std::cout << "\n=== SEARCHING EXPERIMENT (Step 7) ===\n";
    runSearchExperiment(facilityA, countA, "Dataset 1 - Facility A");
    runSearchExperiment(facilityB, countB, "Dataset 2 - Facility B");
    runSearchExperiment(facilityC, countC, "Dataset 3 - Facility C");
    runSearchExperiment(combined, combinedCount, "ALL DATASETS COMBINED");
}

#endif