#include "Teacher.hpp"

#include <iostream>
#include <utility>

Teacher::Teacher(std::string name, std::string subject)
    : Person(std::move(name)), subject_(std::move(subject)) {}

void Teacher::print() const {
    std::cout << "Teacher: " << name_ << " | subject=" << subject_ << '\n';
}
