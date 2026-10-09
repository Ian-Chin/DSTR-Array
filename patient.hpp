#ifndef PATIENT_HPP
#define PATIENT_HPP

#include <string>

const int MAX_PATIENTS = 350;

struct Patient {
    std::string patientID;
    int age;
    std::string careType;
    double lengthOfStay;
    double baseCostPerHour;
    int daysVisitsPerYear;
};

const int MAX_CARE_TYPES = 16;

struct CareTypeRegistry {
    std::string names[MAX_CARE_TYPES];
    int count = 0;

    int indexOf(const std::string& name) {
        for (int i = 0; i < count; i++) {
            if (names[i] == name) return i;
        }
        if (count >= MAX_CARE_TYPES) return -1;
        names[count] = name;
        return count++;
    }
};

inline CareTypeRegistry& careTypeRegistry() {
    static CareTypeRegistry registry;
    return registry;
}

#endif
