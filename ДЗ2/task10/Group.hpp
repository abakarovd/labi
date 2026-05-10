#pragma once

#include <string>
#include <vector>

class Student;

class Group {
public:
    explicit Group(std::string title);
    void addStudent(Student* student);
    double average() const;
private:
    std::string title_;
    std::vector<Student*> students_;
};
