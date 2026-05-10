#pragma once

#include <string>
#include <vector>

class Student;

class Group {
public:
    explicit Group(std::string name = "");

    void addStudent(Student* student);
    void removeStudentByName(const std::string& name);
    void printAll() const;
    double average() const;
    void sortByAverageDesc();
    void filterByAverage(double threshold);

    int countExcellent() const;
    int countRisk() const;

    const std::string& getName() const;
    const std::vector<Student*>& getStudents() const;

private:
    std::string name_;
    std::vector<Student*> students_;
};
