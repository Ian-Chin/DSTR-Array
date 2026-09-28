#ifndef BILLING_ANALYSIS_HPP
#define BILLING_ANALYSIS_HPP

#include <iostream>
#include <iomanip>
#include "patient.hpp"

inline double calculateFacilityCost(const Patient patients[], int count) {
    double totalCost = 0.0;
    for (int i = 0; i < count; i++) {
        totalCost += patients[i].lengthOfStay * patients[i].baseCostPerHour * patients[i].daysVisitsPerYear;
    }
    return totalCost;
}

inline void printBillingReport(const Patient patients[], int count, const std::string& title) {
    std::string line(60, '=');
    std::cout << "\n" << line << "\n";
    std::cout << "             " << title << " TOTAL BILL COST ANALYSIS REPORT\n";
    std::cout << line << "\n";

    double totalCost = calculateFacilityCost(patients, count);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << std::left << std::setw(30) << "Total Bill Cost" << ": RM " << totalCost << "\n";
    std::cout << line << "\n";
}

inline void billingAnalysisMenu(const Patient facilityA[], int countA,
                                const Patient facilityB[], int countB,
                                const Patient facilityC[], int countC,
                                const Patient combined[], int combinedCount) {
    printBillingReport(facilityA, countA, "FACILITY A");
    printBillingReport(facilityB, countB, "FACILITY B");
    printBillingReport(facilityC, countC, "FACILITY C");
    printBillingReport(combined, combinedCount, "OVERALL");
}

#endif