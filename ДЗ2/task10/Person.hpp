#pragma once

#include <string>

class Person {
public:
    explicit Person(std::string name);
    virtual ~Person();
    virtual void print() const;
protected:
    std::string name_;
};
