#ifndef SORTING_ANALYSIS_HPP
#define SORTING_ANALYSIS_HPP
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

#include "patient.hpp"

typedef bool (*PatientLess)(const Patient& a, const Patient& b);

inline double getMedicalCost(const Patient& p) {
    return p.lengthOfStay * p.baseCostPerHour * p.daysVisitsPerYear;
}

inline bool byAgeAsc(const Patient& a, const Patient& b)   { return a.age < b.age; }
inline bool byStayAsc(const Patient& a, const Patient& b)  { return a.lengthOfStay < b.lengthOfStay; }
inline bool byCostDesc(const Patient& a, const Patient& b) { return getMedicalCost(a) > getMedicalCost(b); }
inline double ageOf(const Patient& p)  { return p.age; }
inline double stayOf(const Patient& p) { return p.lengthOfStay; }


struct SortStats {
    std::string label;
    long   comparisons;
    long   moves;
    double timeMs;
    long   dataBytes;
    long   auxBytes;
};

inline void insertionSort(Patient arr[], int n, PatientLess less,
                          long& comparisons, long& moves) {
    comparisons = 0;
    moves = 0;

    for (int i = 1; i < n; i++) {
        Patient key = arr[i];         
        int j = i - 1;

        while (j >= 0) {
            comparisons++;
            if (!less(key, arr[j])) break;  
            arr[j + 1] = arr[j];            
            moves++;
            j--;
        }
        arr[j + 1] = key;
        moves++;
    }
}

inline void mergeHalves(Patient arr[], Patient temp[], int lo, int mid, int hi,
                        PatientLess less, long& comparisons, long& moves) {
    int i = lo, j = mid + 1, k = lo;

    while (i <= mid && j <= hi) {
        comparisons++;
        if (less(arr[j], arr[i])) temp[k++] = arr[j++];
        else                      temp[k++] = arr[i++];
        moves++;
    }
    while (i <= mid) { temp[k++] = arr[i++]; moves++; }
    while (j <= hi)  { temp[k++] = arr[j++]; moves++; }

    for (k = lo; k <= hi; k++) { arr[k] = temp[k]; moves++; }
}

inline void mergeSortRange(Patient arr[], Patient temp[], int lo, int hi,
                           PatientLess less, long& comparisons, long& moves) {
    if (lo >= hi) return;
    int mid = lo + (hi - lo) / 2;
    mergeSortRange(arr, temp, lo, mid, less, comparisons, moves);
    mergeSortRange(arr, temp, mid + 1, hi, less, comparisons, moves);
    mergeHalves(arr, temp, lo, mid, hi, less, comparisons, moves);
}

inline void mergeSort(Patient arr[], int n, PatientLess less,
                      long& comparisons, long& moves) {
    comparisons = 0;
    moves = 0;
    if (n < 2) return;

    Patient* temp = new Patient[n];    
    mergeSortRange(arr, temp, 0, n - 1, less, comparisons, moves);
    delete[] temp;
}

inline SortStats measureSort(const Patient original[], int n, PatientLess less,
                             const std::string& label, bool useInsertion) {
    Patient* work = new Patient[n];
    for (int i = 0; i < n; i++) work[i] = original[i];

    SortStats s;
    s.label = label;

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    if (useInsertion) insertionSort(work, n, less, s.comparisons, s.moves);
    else              mergeSort(work, n, less, s.comparisons, s.moves);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

    s.timeMs    = std::chrono::duration<double, std::milli>(end - start).count();
    s.dataBytes = static_cast<long>(n) * static_cast<long>(sizeof(Patient));
    s.auxBytes  = useInsertion ? static_cast<long>(sizeof(Patient)) : s.dataBytes;

    delete[] work;
    return s;
}

inline void printSortTable(const SortStats stats[], int count, const std::string& title, int n) {
    std::cout << "\n" << title << " (" << n << " records)\n";
    std::cout << std::left  << std::setw(24) << "Algorithm / Sort Key"
              << std::right << std::setw(13) << "Comparisons"
              << std::setw(10) << "Moves"
              << std::setw(12) << "Time(ms)"
              << std::setw(12) << "Data(B)"
              << std::setw(13) << "ExtraMem(B)" << "\n";
    std::cout << std::string(84, '-') << "\n";
    for (int i = 0; i < count; i++) {
        std::cout << std::left  << std::setw(24) << stats[i].label
                  << std::right << std::setw(13) << stats[i].comparisons
                  << std::setw(10) << stats[i].moves
                  << std::fixed << std::setprecision(4)
                  << std::setw(12) << stats[i].timeMs
                  << std::setw(12) << stats[i].dataBytes
                  << std::setw(13) << stats[i].auxBytes << "\n";
    }
}


inline void runSortingExperiment(const Patient patients[], int count, const std::string& title) {
    PatientLess rules[3] = { byAgeAsc, byStayAsc, byCostDesc };
    std::string names[3] = { "Age", "LengthOfStay", "TotalCost" };
    SortStats stats[6];

    for (int k = 0; k < 3; k++) {
        stats[k]     = measureSort(patients, count, rules[k], "Merge / " + names[k], false);
        stats[k + 3] = measureSort(patients, count, rules[k], "Insertion / " + names[k], true);
    }
    printSortTable(stats, 6, title, count);
}

inline void sortingAnalysisMenu(const Patient facilityA[], int countA,
                                const Patient facilityB[], int countB,
                                const Patient facilityC[], int countC,
                                const Patient combined[], int combinedCount) {
    std::cout << "\n=== SORTING EXPERIMENT (ARRAY) ===\n";
    runSortingExperiment(facilityA, countA, "FACILITY A");
    runSortingExperiment(facilityB, countB, "FACILITY B");
    runSortingExperiment(facilityC, countC, "FACILITY C");
    runSortingExperiment(combined, combinedCount, "COMBINED (A + B + C)");
}

#endif