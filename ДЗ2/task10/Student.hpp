#pragma once

#include "Person.hpp"
#include "RecordBook.hpp"

class Student : public Person {
public:
    Student(std::string name, std::string rbNumber);
    void addGrade(double grade);
    double average() const;
    void print() const override;
private:
    RecordBook recordBook_;
};
