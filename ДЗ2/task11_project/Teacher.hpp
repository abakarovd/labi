#pragma once

#include "Person.hpp"

#include <string>

class Teacher : public Person {
public:
    Teacher(std::string name, std::string subject);
    void print() const override;

private:
    std::string subject_;
};
