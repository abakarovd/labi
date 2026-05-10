#include "Student.hpp"

#include <iostream>
#include <utility>

Student::Student(std::string name, std::string recordBookNumber)
    : Person(std::move(name)), recordBook_(std::move(recordBookNumber)) {}

void Student::addGrade(double grade) { recordBook_.addGrade(grade); }
double Student::average() const { return recordBook_.average(); }

void Student::print() const {
    std::cout << "Student: " << name_ << " | avg=" << average() << " | rb=" << recordBook_.getNumber() << '\n';
}

const RecordBook& Student::getRecordBook() const { return recordBook_; }
