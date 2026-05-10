#include "Person.hpp"

#include <iostream>
#include <utility>

Person::Person(std::string name) : name_(std::move(name)) {}
Person::~Person() = default;

void Person::print() const {
    std::cout << "Person: " << name_ << '\n';
}

const std::string& Person::getName() const {
    return name_;
}
