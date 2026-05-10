#pragma once

#include "Person.hpp"
#include "RecordBook.hpp"

#include <string>

class Student : public Person {
public:
    Student(std::string name, std::string recordBookNumber);
    void addGrade(double grade);
    double average() const;
    void print() const override;

    const RecordBook& getRecordBook() const;

private:
    RecordBook recordBook_;
};
