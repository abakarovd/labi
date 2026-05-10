#include "Group.hpp"
#include "Student.hpp"

Group::Group(std::string title) : title_(std::move(title)) {}
void Group::addStudent(Student* student) { students_.push_back(student); }

double Group::average() const {
    if (students_.empty()) return 0.0;
    double s = 0;
    for (const Student* st : students_) s += st->average();
    return s / students_.size();
}
