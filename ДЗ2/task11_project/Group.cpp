#include "Group.hpp"

#include "Student.hpp"

#include <algorithm>
#include <iostream>
#include <utility>

Group::Group(std::string name) : name_(std::move(name)) {}

void Group::addStudent(Student* student) {
    if (student) students_.push_back(student);
}

void Group::removeStudentByName(const std::string& name) {
    students_.erase(
        std::remove_if(students_.begin(), students_.end(), [&](Student* s) {
            return s && s->getName() == name;
        }),
        students_.end());
}

void Group::printAll() const {
    std::cout << "Group " << name_ << "\n";
    for (const Student* s : students_) {
        if (s) s->print();
    }
}

double Group::average() const {
    if (students_.empty()) return 0.0;
    double sum = 0.0;
    int count = 0;
    for (const Student* s : students_) {
        if (!s) continue;
        sum += s->average();
        ++count;
    }
    return count ? (sum / count) : 0.0;
}

void Group::sortByAverageDesc() {
    std::sort(students_.begin(), students_.end(), [](const Student* a, const Student* b) {
        if (!a || !b) return a < b;
        if (a->average() != b->average()) return a->average() > b->average();
        return a->getName() < b->getName();
    });
}

void Group::filterByAverage(double threshold) {
    students_.erase(
        std::remove_if(students_.begin(), students_.end(), [threshold](const Student* s) {
            return s && s->average() < threshold;
        }),
        students_.end());
}

int Group::countExcellent() const {
    return static_cast<int>(std::count_if(students_.begin(), students_.end(), [](const Student* s) {
        return s && s->average() >= 4.5;
    }));
}

int Group::countRisk() const {
    return static_cast<int>(std::count_if(students_.begin(), students_.end(), [](const Student* s) {
        return s && s->average() < 3.0;
    }));
}

const std::string& Group::getName() const { return name_; }
const std::vector<Student*>& Group::getStudents() const { return students_; }
