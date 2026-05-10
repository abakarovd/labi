#include "RecordBook.hpp"

RecordBook::RecordBook(std::string number) : number_(std::move(number)) {}
void RecordBook::addGrade(double grade) { if (grade >= 0 && grade <= 5) grades_.push_back(grade); }

double RecordBook::average() const {
    if (grades_.empty()) return 0.0;
    double s = 0;
    for (double g : grades_) s += g;
    return s / grades_.size();
}

const std::string& RecordBook::number() const { return number_; }
