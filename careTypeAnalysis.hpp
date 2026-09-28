#ifndef CARE_TYPE_ANALYSIS_HPP
#define CARE_TYPE_ANALYSIS_HPP

#include <string>
#include <iostream>
#include <iomanip>
#include "patient.hpp"

inline void printCareTypeReport(const Patient patients[], int count, const std::string& title) {
    std::string line(60, '=');
    std::cout << "\n" << line << "\n";
    std::cout << "               " << title << " CARE TYPE ANALYSIS REPORT\n";
    std::cout << line << "\n";

    std::string types[20];
    int counts[20] = {0};
    double totalHours[20] = {0.0};
    int typeCount = 0;

    for (int i = 0; i < count; i++) {
        bool found = false;
        for (int j = 0; j < typeCount; j++) {
            if (types[j] == patients[i].careType) {
                counts[j]++;
                totalHours[j] += patients[i].lengthOfStay;
                found = true;
                break;
            }
        }
        if (!found && typeCount < 20) {
            types[typeCount] = patients[i].careType;
            counts[typeCount] = 1;
            totalHours[typeCount] = patients[i].lengthOfStay;
            typeCount++;
        }
    }

    std::cout << std::left << std::setw(20) << "Care Type" << std::setw(18) << "Patient Count" << "Total Stay Hours" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    for (int i = 0; i < typeCount; i++) {
        std::cout << std::left << std::setw(20) << types[i] 
                  << std::setw(18) << counts[i] 
                  << std::fixed << std::setprecision(2) << totalHours[i] << " hrs" << std::endl;
    }
    std::cout << line << "\n";
}

inline void careTypeAnalysisMenu(const Patient facilityA[], int countA,
                                 const Patient facilityB[], int countB,
                                 const Patient facilityC[], int countC,
                                 const Patient combined[], int combinedCount) {
    printCareTypeReport(facilityA, countA, "FACILITY A");
    printCareTypeReport(facilityB, countB, "FACILITY B");
    printCareTypeReport(facilityC, countC, "FACILITY C");
    printCareTypeReport(combined, combinedCount, "OVERALL");
}

#endif