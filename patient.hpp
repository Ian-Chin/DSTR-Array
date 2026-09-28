#ifndef PATIENT_HPP
#define PATIENT_HPP

#include <string>

const int MAX_PATIENTS = 350; // Maximum capacity for raw arrays per facility

struct Patient {
    std::string patientID;
    int age;
    std::string careType;
    double lengthOfStay;
    double baseCostPerHour;
    int daysVisitsPerYear;
};

#endif