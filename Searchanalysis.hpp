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
    bool        useStayOver;
    double      minStay;        

    SearchCriteria()
        : useAgeRange(false), minAge(0), maxAge(0), useCareType(false),
          useStayOver(false), minStay(0.0) {}
};

inline bool matchesCriteria(const Patient& p, const SearchCriteria& c) {
    if (c.useAgeRange && (p.age < c.minAge || p.age > c.maxAge)) return false;
    if (c.useCareType && p.careType != c.careType)               return false;
    if (c.useStayOver && !(p.lengthOfStay > c.minStay))          return false;
    return true;
}

// Results of one search run
struct SearchStats {
    std::string label;
    long   comparisons;
    double timeMs;
    int    matches;
    long   extraBytes;   
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
    s.extraBytes = static_cast<long>(s.matches) * static_cast<long>(sizeof(Patient));
    return s;
}


inline int binaryFirstGreater(const Patient sorted[], int n, double (*keyOf)(const Patient&),
                              double threshold, SearchStats& stats) {
    stats.label = "Binary (boundary)";
    stats.comparisons = 0;
    stats.extraBytes = 0;

    SearchClock::time_point start = SearchClock::now();
    int low = 0, high = n;                        
    while (low < high) {
        int mid = low + (high - low) / 2;         
        stats.comparisons++;
        if (keyOf(sorted[mid]) > threshold) high = mid;
        else                                low = mid + 1;
    }
    SearchClock::time_point end = SearchClock::now();

    stats.timeMs = std::chrono::duration<double, std::milli>(end - start).count();
    stats.matches = n - low;
    return low;
}


inline int binarySearchByKey(const Patient sorted[], int n, double (*keyOf)(const Patient&),
                             double target, SearchStats& stats) {
    stats.label = "Binary (exact)";
    stats.comparisons = 0;
    stats.matches = 0;
    stats.extraBytes = 0;

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

inline SearchStats rangeSearchSortedByAge(const Patient sortedByAge[], int n,
                                          const SearchCriteria& crit, Patient results[]) {
    SearchStats s;
    s.label = "Binary+scan (sorted)";
    s.matches = 0;

    SearchClock::time_point start = SearchClock::now();

    SearchStats bound;
    int i = binaryFirstGreater(sortedByAge, n, ageOf, crit.minAge - 1, bound);
    s.comparisons = bound.comparisons;

    for (; i < n; i++) {
        s.comparisons++;
        if (sortedByAge[i].age > crit.maxAge) break;     
        if (matchesCriteria(sortedByAge[i], crit)) {
            results[s.matches++] = sortedByAge[i];
        }
    }
    SearchClock::time_point end = SearchClock::now();

    s.timeMs = std::chrono::duration<double, std::milli>(end - start).count();
    s.extraBytes = static_cast<long>(s.matches) * static_cast<long>(sizeof(Patient));
    return s;
}

inline void printSearchTable(const SearchStats stats[], int count) {
    std::cout << std::left  << std::setw(24) << "Search Type"
              << std::right << std::setw(13) << "Comparisons"
              << std::setw(12) << "Time(ms)"
              << std::setw(10) << "Matches"
              << std::setw(14) << "ExtraMem(B)" << "\n";
    std::cout << std::string(73, '-') << "\n";
    for (int i = 0; i < count; i++) {
        std::cout << std::left  << std::setw(24) << stats[i].label
                  << std::right << std::setw(13) << stats[i].comparisons
                  << std::fixed << std::setprecision(5)
                  << std::setw(12) << stats[i].timeMs
                  << std::setw(10) << stats[i].matches
                  << std::setw(14) << stats[i].extraBytes << "\n";
    }
}

inline void printMatchRows(const Patient rows[], int count, int maxRows) {
    std::cout << std::left
              << std::setw(12) << "Patient ID"
              << std::setw(8)  << "Age"
              << std::setw(20) << "Care Type"
              << std::setw(12) << "Stay(hr)"
              << std::setw(12) << "Cost/hr"
              << std::setw(12) << "Visits/Year"
              << std::setw(14) << "Total Cost" << "\n";
    std::cout << std::string(90, '-') << "\n";

    for (int i = 0; i < count && i < maxRows; i++) {
        std::cout << std::left
                  << std::setw(12) << rows[i].patientID
                  << std::setw(8)  << rows[i].age
                  << std::setw(20) << rows[i].careType
                  << std::fixed << std::setprecision(2)
                  << std::setw(12) << rows[i].lengthOfStay
                  << std::setw(12) << rows[i].baseCostPerHour
                  << std::setw(12) << rows[i].daysVisitsPerYear
                  << std::setw(14) << getMedicalCost(rows[i]) << "\n";
    }
    if (count > maxRows) {
        std::cout << "... (" << (count - maxRows) << " more records)\n";
    }
}

inline void runSearchExperiment(const Patient patients[], int count, const std::string& title) {
    std::cout << "\n------------------------------------------------------------\n";
    std::cout << title << " (" << count << " records)\n";
    std::cout << "------------------------------------------------------------\n";

    Patient* results = new Patient[count];
    Patient* sorted  = new Patient[count];
    SearchCriteria critA;
    critA.useAgeRange = true;  critA.minAge = 61;  critA.maxAge = 100;
    critA.useCareType = true;  critA.careType = "Emergency";

    SearchStats a[3];
    a[0] = linearSearch(patients, count, critA, results);
    const int matchesA = a[0].matches;

    for (int i = 0; i < count; i++) sorted[i] = patients[i];
    long cmp = 0, mv = 0;
    mergeSort(sorted, count, byAgeAsc, cmp, mv);

    Patient* resultsSorted = new Patient[count];
    a[1] = rangeSearchSortedByAge(sorted, count, critA, resultsSorted);
    int hit = binarySearchByKey(sorted, count, ageOf, 65.0, a[2]);

    std::cout << "\nQuery A: Age 61-100 + Care Type = Emergency\n";
    printSearchTable(a, 3);
    std::cout << "\nMatching records (linear search result):\n";
    printMatchRows(results, matchesA, 10);
    if (hit >= 0) std::cout << "Binary search found a patient aged 65: " << sorted[hit].patientID << "\n";
    else          std::cout << "Binary search: no patient aged exactly 65 in this dataset\n";

    
    SearchCriteria critB;
    critB.useStayOver = true;  critB.minStay = 24.0;

    SearchStats b[2];
    b[0] = linearSearch(patients, count, critB, results);
    const int matchesB = b[0].matches;

    for (int i = 0; i < count; i++) sorted[i] = patients[i];
    mergeSort(sorted, count, byStayAsc, cmp, mv);
    int first = binaryFirstGreater(sorted, count, stayOf, 24.0, b[1]);

    std::cout << "\nQuery B: Length of Stay > 24 hours\n";
    printSearchTable(b, 2);
    std::cout << "\nMatching records (linear search result):\n";
    printMatchRows(results, matchesB, 10);
    std::cout << "Binary search boundary index = " << first << " (" << b[1].matches
              << " records from this index onwards match)\n";

    delete[] resultsSorted;
    delete[] sorted;
    delete[] results;
}

inline void searchAnalysisMenu(const Patient facilityA[], int countA,
                               const Patient facilityB[], int countB,
                               const Patient facilityC[], int countC,
                               const Patient combined[], int combinedCount) {
    std::cout << "\n=== SEARCHING (ARRAY) ===\n";
    runSearchExperiment(facilityA, countA, "FACILITY A");
    runSearchExperiment(facilityB, countB, "FACILITY B");
    runSearchExperiment(facilityC, countC, "FACILITY C");
    runSearchExperiment(combined, combinedCount, "COMBINED (A + B + C)");
}

#endif