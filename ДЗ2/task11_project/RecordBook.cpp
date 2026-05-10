#include "RecordBook.hpp"

#include <iostream>
#include <utility>

RecordBook::RecordBook(std::string number) : number_(std::move(number)) {}

void RecordBook::addGrade(double grade) {
    if (grade >= 0.0 && grade <= 5.0) {
        grades_.push_back(grade);
    }
}

double RecordBook::average() const {
    if (grades_.empty()) return 0.0;
    double sum = 0.0;
    for (double g : grades_) sum += g;
    return sum / grades_.size();
}

void RecordBook::print() const {
    std::cout << "RecordBook " << number_ << " grades: ";
    for (double g : grades_) std::cout << g << ' ';
    std::cout << '\n';
}

const std::string& RecordBook::getNumber() const { return number_; }
const std::vector<double>& RecordBook::getGrades() const { return grades_; }
