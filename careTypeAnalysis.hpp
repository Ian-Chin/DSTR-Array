#ifndef CARE_TYPE_ANALYSIS_HPP
#define CARE_TYPE_ANALYSIS_HPP

#include <string>
#include <iostream>
#include <iomanip>
#include "patient.hpp"

inline void printCareTypeReport(const Patient patients[], int count, const std::string& title) {
    int counts[MAX_CARE_TYPES] = {0};
    double totalHours[MAX_CARE_TYPES] = {0.0};
    double totalCost[MAX_CARE_TYPES] = {0.0};
    double grandStay = 0.0;
    double grandCost = 0.0;
    CareTypeRegistry& reg = careTypeRegistry();

    for (int i = 0; i < count; i++) {
        double cost = patients[i].lengthOfStay * patients[i].baseCostPerHour * patients[i].daysVisitsPerYear;
        grandStay += patients[i].lengthOfStay;
        grandCost += cost;

        int c = reg.indexOf(patients[i].careType);
        if (c >= 0) {
            counts[c]++;
            totalHours[c] += patients[i].lengthOfStay;
            totalCost[c] += cost;
        }
    }

    std::cout << "\n" << title << "  -  by care type\n";
    std::cout << std::left  << std::setw(18) << "CareType"
              << std::right << std::setw(9)  << "Patients"
              << std::setw(9)  << "AvgStay"
              << std::setw(16) << "TotalCost(RM)"
              << std::setw(14) << "AvgCost(RM)"
              << std::setw(11) << "ShareCost" << "\n";
    std::cout << std::string(77, '-') << "\n";

    for (int i = 0; i < reg.count; i++) {
        if (counts[i] == 0) continue;
        std::cout << std::left  << std::setw(18) << reg.names[i]
                  << std::right << std::setw(9) << counts[i]
                  << std::fixed << std::setprecision(1)
                  << std::setw(9) << totalHours[i] / counts[i]
                  << std::setprecision(2)
                  << std::setw(16) << totalCost[i]
                  << std::setw(14) << totalCost[i] / counts[i]
                  << std::setprecision(1)
                  << std::setw(10) << (grandCost > 0.0 ? 100.0 * totalCost[i] / grandCost : 0.0)
                  << "%" << "\n";
    }
    std::cout << std::string(77, '-') << "\n";
    std::cout << std::left  << std::setw(18) << "TOTAL"
              << std::right << std::setw(9) << count
              << std::fixed << std::setprecision(1)
              << std::setw(9) << (count > 0 ? grandStay / count : 0.0)
              << std::setprecision(2)
              << std::setw(16) << grandCost
              << std::setw(14) << (count > 0 ? grandCost / count : 0.0)
              << std::setprecision(1) << std::setw(10) << 100.0 << "%\n";
}

#endif
