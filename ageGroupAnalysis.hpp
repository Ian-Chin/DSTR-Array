#ifndef AGE_GROUP_ANALYSIS_HPP
#define AGE_GROUP_ANALYSIS_HPP

#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <sstream>
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
    if (age >= 0  && age <= 17)  return 0;   // 0-17 Pediatrics
    if (age >= 18 && age <= 25)  return 1;   // 18-25 Young Adult
    if (age >= 26 && age <= 45)  return 2;   // 26-45 Working Early
    if (age >= 46 && age <= 60)  return 3;   // 46-60 Working Late
    if (age >= 61)               return 4;   // 61-100 Senior
    return -1;
}

inline int countAgeGroups(const Patient patients[], int count, int ageCounts[]) {
    for (int i = 0; i < NUM_AGE_GROUPS; i++) {
        ageCounts[i] = 0;
    }
    int outOfRange = 0;
    for (int i = 0; i < count; i++) {
        int idx = getAgeGroupIndex(patients[i].age);
        if (idx >= 0 && idx < NUM_AGE_GROUPS) {
            ageCounts[idx]++;
        } else {
            outOfRange++;
        }
    }
    return outOfRange;
}

struct AgeGroupStats {
    int    count      = 0;
    double sumAge     = 0.0;
    double sumStay    = 0.0;
    double totalCost  = 0.0;
    std::string careTypes[20];
    int         careCounts[20] = {0};
    int         careTypeCount  = 0;
};

inline void addCareType(AgeGroupStats& s, const std::string& care) {
    for (int i = 0; i < s.careTypeCount; i++) {
        if (s.careTypes[i] == care) {
            s.careCounts[i]++;
            return;
        }
    }
    if (s.careTypeCount < 20) {
        s.careTypes[s.careTypeCount]  = care;
        s.careCounts[s.careTypeCount] = 1;
        s.careTypeCount++;
    }
}

inline std::string getTopCareType(const AgeGroupStats& s) {
    if (s.careTypeCount == 0) return "-";
    int best = 0;
    for (int i = 1; i < s.careTypeCount; i++) {
        if (s.careCounts[i] > s.careCounts[best]) best = i;
    }
    return s.careTypes[best] + " (" + std::to_string(s.careCounts[best]) + ")";
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
        addCareType(s, patients[i].careType);
    }
}

inline void printAgeGroupReport(const Patient patients[], int count, const std::string& title) {
    AgeGroupStats stats[NUM_AGE_GROUPS];
    collectAgeGroupStats(patients, count, stats);

    double grandTotalCost = 0.0;
    for (int i = 0; i < NUM_AGE_GROUPS; i++) grandTotalCost += stats[i].totalCost;

    std::cout << "\n";
    std::cout << title << "  - by age group  (" << count << " patients)\n";
    std::cout << std::left
              << std::setw(22) << "AgeGroup"
              << std::setw(10) << "Patients"
              << std::setw(8)  << "Share"
              << std::setw(8)  << "AvgAge"
              << std::setw(18) << "TopCareType"
              << std::setw(10) << "AvgStay"
              << std::setw(14) << "TotalCost(RM)"
              << std::setw(12) << "AvgCost(RM)"
              << "\n";
    std::cout << std::string(100, '-') << "\n";

    std::cout << std::fixed;

    for (int i = 0; i < NUM_AGE_GROUPS; i++) {
        const AgeGroupStats& s = stats[i];
        double share   = (count > 0) ? (s.count * 100.0 / count) : 0.0;
        double avgAge  = (s.count > 0) ? (s.sumAge  / s.count) : 0.0;
        double avgStay = (s.count > 0) ? (s.sumStay / s.count) : 0.0;
        double avgCost = (s.count > 0) ? (s.totalCost / s.count) : 0.0;

        std::ostringstream shareStr;
        shareStr << std::fixed << std::setprecision(1) << share << "%";

        std::cout << std::left << std::setw(22) << getAgeGroupName(i)
                  << std::setw(10) << s.count
                  << std::setw(8)  << shareStr.str()
                  << std::setprecision(1) << std::setw(8) << avgAge
                  << std::setw(18) << getTopCareType(s)
                  << std::setw(10) << avgStay
                  << std::right << std::setw(13) << std::setprecision(2) << s.totalCost
                  << "  " << std::left << std::setprecision(2) << avgCost
                  << "\n";
    }

    std::cout << std::string(100, '-') << "\n";
    double overallAvgCost = (count > 0) ? (grandTotalCost / count) : 0.0;
    std::cout << std::left << std::setw(22) << "TOTAL"
              << std::setw(10) << count
              << std::setw(8)  << ""
              << std::setw(8)  << ""
              << std::setw(18) << ""
              << std::setw(10) << ""
              << std::right << std::setw(13) << std::setprecision(2) << grandTotalCost
              << "  " << std::left << std::setprecision(2) << overallAvgCost
              << "\n";
    std::cout << "\n";
}

inline void ageGroupAnalysisMenu(const Patient facilityA[], int countA,
                                 const Patient facilityB[], int countB,
                                 const Patient facilityC[], int countC,
                                 const Patient combined[], int combinedCount) {
    printAgeGroupReport(facilityA, countA, "Dataset 1 - Facility A");
    printAgeGroupReport(facilityB, countB, "Dataset 2 - Facility B");
    printAgeGroupReport(facilityC, countC, "Dataset 3 - Facility C");
    printAgeGroupReport(combined, combinedCount, "OVERALL");
}

#endif