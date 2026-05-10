#include "Person.hpp"
#include <iostream>

Person::Person(std::string name) : name_(std::move(name)) {}
Person::~Person() = default;
void Person::print() const { std::cout << name_ << '\n'; }
