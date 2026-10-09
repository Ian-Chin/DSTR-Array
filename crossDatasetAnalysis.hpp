#ifndef CROSS_DATASET_ANALYSIS_HPP
#define CROSS_DATASET_ANALYSIS_HPP

#include <iomanip>
#include <iostream>
#include <string>

#include "patient.hpp"
#include "ageGroupAnalysis.hpp"

struct DatasetTotals {
    int    patients    = 0;
    double totalAge    = 0.0;
    double totalStay   = 0.0;
    double totalVisits = 0.0;
    double totalCost   = 0.0;
    int    groupPatients[NUM_AGE_GROUPS] = {0};
    double groupStay[NUM_AGE_GROUPS]     = {0.0};
    double groupCost[NUM_AGE_GROUPS]     = {0.0};
};

inline DatasetTotals collectDatasetTotals(const Patient patients[], int count) {
    DatasetTotals t;
    for (int i = 0; i < count; i++) {
        const Patient& p = patients[i];
        double cost = p.lengthOfStay * p.baseCostPerHour * p.daysVisitsPerYear;

        t.patients++;
        t.totalAge    += p.age;
        t.totalStay   += p.lengthOfStay;
        t.totalVisits += p.daysVisitsPerYear;
        t.totalCost   += cost;

        int g = getAgeGroupIndex(p.age);
        if (g >= 0 && g < NUM_AGE_GROUPS) {
            t.groupPatients[g]++;
            t.groupStay[g] += p.lengthOfStay;
            t.groupCost[g] += cost;
        }
    }
    return t;
}

inline double safeAvg(double sum, int n) {
    return n > 0 ? sum / n : 0.0;
}

inline void printDatasetComparison(const DatasetTotals t[], const std::string labels[],
                                   int count, const DatasetTotals& combined) {
    std::cout << "\nDataset comparison  -  expenditure and visit duration\n";
    std::cout << std::left  << std::setw(26) << "Dataset"
              << std::right << std::setw(9)  << "Patients"
              << std::setw(8)  << "AvgAge"
              << std::setw(10) << "AvgStay"
              << std::setw(11) << "AvgVisits"
              << std::setw(16) << "TotalCost(RM)"
              << std::setw(14) << "AvgCost(RM)" << "\n";
    std::cout << std::string(94, '-') << "\n";

    for (int i = 0; i <= count; i++) {
        const DatasetTotals& d = (i < count) ? t[i] : combined;
        if (i == count) std::cout << std::string(94, '-') << "\n";
        std::cout << std::left  << std::setw(26) << (i < count ? labels[i] : "ALL DATASETS")
                  << std::right << std::setw(9) << d.patients
                  << std::fixed << std::setprecision(1)
                  << std::setw(8)  << safeAvg(d.totalAge, d.patients)
                  << std::setw(10) << safeAvg(d.totalStay, d.patients)
                  << std::setw(11) << safeAvg(d.totalVisits, d.patients)
                  << std::setprecision(2)
                  << std::setw(16) << d.totalCost
                  << std::setw(14) << safeAvg(d.totalCost, d.patients) << "\n";
    }
}

inline void printCrossMatrix(const DatasetTotals t[], const std::string labels[], int count,
                             const DatasetTotals& combined, bool costMode) {
    const int width = 21 + 16 * (count + 1);

    std::cout << "\n" << (costMode ? "Total medical cost (RM) by age group x dataset"
                                   : "Average length of stay (hours) by age group x dataset")
              << "\n";
    std::cout << std::left << std::setw(21) << "AgeGroup";
    for (int i = 0; i < count; i++) std::cout << std::right << std::setw(16) << labels[i];
    std::cout << std::right << std::setw(16) << "ALL" << "\n";
    std::cout << std::string(width, '-') << "\n";

    for (int g = 0; g < NUM_AGE_GROUPS; g++) {
        std::cout << std::left << std::setw(21) << getAgeGroupName(g) << std::right;
        for (int i = 0; i <= count; i++) {
            const DatasetTotals& d = (i < count) ? t[i] : combined;
            const int n = d.groupPatients[g];
            if (costMode) {
                std::cout << std::fixed << std::setprecision(2) << std::setw(16) << d.groupCost[g];
            } else if (n > 0) {
                std::cout << std::fixed << std::setprecision(1) << std::setw(16) << d.groupStay[g] / n;
            } else {
                std::cout << std::setw(16) << "-";
            }
        }
        std::cout << "\n";
    }
    std::cout << std::string(width, '-') << "\n";

    std::cout << std::left << std::setw(21) << (costMode ? "TOTAL" : "OVERALL AVG") << std::right;
    for (int i = 0; i <= count; i++) {
        const DatasetTotals& d = (i < count) ? t[i] : combined;
        if (costMode) {
            std::cout << std::fixed << std::setprecision(2) << std::setw(16) << d.totalCost;
        } else {
            std::cout << std::fixed << std::setprecision(1) << std::setw(16)
                      << safeAvg(d.totalStay, d.patients);
        }
    }
    std::cout << "\n";
}

#endif
