#ifndef AGE_GROUP_ANALYSIS_HPP
#define AGE_GROUP_ANALYSIS_HPP

#include <string>
#include <iostream>
#include <iomanip>
#include "patient.hpp"

const int NUM_AGE_GROUPS = 5;

inline std::string getAgeGroupName(int index) {
    switch (index) {
        case 0: return "0-17 Pediatrics";
        case 1: return "18-25 Young Adult";
        case 2: return "26-45 Working Early";
        case 3: return "46-60 Working Late";
        case 4: return "61-100 Senior";
        default: return "Unknown";
    }
}

inline int getAgeGroupIndex(int age) {
    if (age >= 0  && age <= 17)  return 0;
    if (age >= 18 && age <= 25)  return 1;
    if (age >= 26 && age <= 45)  return 2;
    if (age >= 46 && age <= 60)  return 3;
    if (age >= 61)               return 4;
    return -1;
}

struct AgeGroupStats {
    int    count      = 0;
    double sumAge     = 0.0;
    double sumStay    = 0.0;
    double totalCost  = 0.0;
    int    careCounts[MAX_CARE_TYPES] = {0};
};

inline std::string getTopCareType(const AgeGroupStats& s) {
    const CareTypeRegistry& reg = careTypeRegistry();
    int best = -1;
    for (int c = 0; c < reg.count; c++) {
        if (s.careCounts[c] > 0 && (best < 0 || s.careCounts[c] > s.careCounts[best])) {
            best = c;
        }
    }
    if (best < 0) return "-";
    return reg.names[best] + " (" + std::to_string(s.careCounts[best]) + ")";
}

inline void collectAgeGroupStats(const Patient patients[], int count, AgeGroupStats stats[]) {
    for (int i = 0; i < NUM_AGE_GROUPS; i++) {
        stats[i] = AgeGroupStats();
    }
    for (int i = 0; i < count; i++) {
        int idx = getAgeGroupIndex(patients[i].age);
        if (idx < 0 || idx >= NUM_AGE_GROUPS) continue;

        AgeGroupStats& s = stats[idx];
        s.count++;
        s.sumAge  += patients[i].age;
        s.sumStay += patients[i].lengthOfStay;
        double cost = patients[i].lengthOfStay
                    * patients[i].baseCostPerHour
                    * patients[i].daysVisitsPerYear;
        s.totalCost += cost;

        int c = careTypeRegistry().indexOf(patients[i].careType);
        if (c >= 0) s.careCounts[c]++;
    }
}

inline void printAgeGroupReport(const Patient patients[], int count, const std::string& title) {
    AgeGroupStats stats[NUM_AGE_GROUPS];
    collectAgeGroupStats(patients, count, stats);

    double grandTotalCost = 0.0;
    for (int i = 0; i < NUM_AGE_GROUPS; i++) grandTotalCost += stats[i].totalCost;

    std::cout << "\n" << title << "  -  by age group   (" << count << " patients)\n";
    std::cout << std::left  << std::setw(21) << "AgeGroup"
              << std::right << std::setw(9)  << "Patients"
              << std::setw(8)  << "Share"
              << std::setw(8)  << "AvgAge"
              << "  " << std::left << std::setw(24) << "TopCareType"
              << std::right << std::setw(9)  << "AvgStay"
              << std::setw(16) << "TotalCost(RM)"
              << std::setw(14) << "AvgCost(RM)" << "\n";
    std::cout << std::string(109, '-') << "\n";

    for (int i = 0; i < NUM_AGE_GROUPS; i++) {
        const AgeGroupStats& s = stats[i];
        const int n = s.count;

        std::cout << std::left  << std::setw(21) << getAgeGroupName(i)
                  << std::right << std::setw(9) << n
                  << std::fixed << std::setprecision(1)
                  << std::setw(7) << (count > 0 ? 100.0 * n / count : 0.0) << "%"
                  << std::setw(8) << (n > 0 ? s.sumAge / n : 0.0)
                  << "  " << std::left << std::setw(24) << getTopCareType(s)
                  << std::right << std::setprecision(1)
                  << std::setw(9) << (n > 0 ? s.sumStay / n : 0.0)
                  << std::setprecision(2)
                  << std::setw(16) << s.totalCost
                  << std::setw(14) << (n > 0 ? s.totalCost / n : 0.0) << "\n";
    }
    std::cout << std::string(109, '-') << "\n";
    std::cout << std::left  << std::setw(21) << "TOTAL"
              << std::right << std::setw(9) << count
              << std::fixed << std::setprecision(2)
              << std::setw(67) << grandTotalCost
              << std::setw(14) << (count > 0 ? grandTotalCost / count : 0.0)
              << "\n";
}

#endif
