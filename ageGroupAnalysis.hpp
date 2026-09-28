#ifndef AGE_GROUP_ANALYSIS_HPP
#define AGE_GROUP_ANALYSIS_HPP

#include <string>
#include <iostream>
#include <iomanip>
#include "patient.hpp"

const int NUM_AGE_GROUPS = 5;

inline std::string getAgeGroupName(int index) {
    switch(index) {
        case 0: return "0 - 18 (Pediatric/Youth)";
        case 1: return "19 - 35 (Young Adult)";
        case 2: return "36 - 50 (Adult)";
        case 3: return "51 - 65 (Middle Age)";
        case 4: return "66+ (Senior)";
        default: return "Unknown";
    }
}

inline int getAgeGroupIndex(int age) {
    if (age >= 0 && age <= 18) return 0;
    if (age >= 19 && age <= 35) return 1;
    if (age >= 36 && age <= 50) return 2;
    if (age >= 51 && age <= 65) return 3;
    if (age >= 66) return 4;
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

inline void printAgeGroupReport(const Patient patients[], int count, const std::string& title) {
    std::string line(60, '=');
    std::cout << "\n" << line << "\n";
    std::cout << "               " << title << " AGE GROUP ANALYSIS REPORT\n";
    std::cout << line << "\n";

    int ageCounts[NUM_AGE_GROUPS];
    int outOfRange = countAgeGroups(patients, count, ageCounts);

    for (int i = 0; i < NUM_AGE_GROUPS; i++) {
        std::cout << std::left << std::setw(32) << getAgeGroupName(i) << ": " << ageCounts[i] << " patients\n";
    }
    if (outOfRange > 0) {
        std::cout << std::left << std::setw(32) << "Unknown / Out of range" << ": " << outOfRange << " patients\n";
    }
    std::cout << line << "\n";
}

inline void ageGroupAnalysisMenu(const Patient facilityA[], int countA,
                                 const Patient facilityB[], int countB,
                                 const Patient facilityC[], int countC,
                                 const Patient combined[], int combinedCount) {
    printAgeGroupReport(facilityA, countA, "FACILITY A");
    printAgeGroupReport(facilityB, countB, "FACILITY B");
    printAgeGroupReport(facilityC, countC, "FACILITY C");
    printAgeGroupReport(combined, combinedCount, "OVERALL");
}

#endif