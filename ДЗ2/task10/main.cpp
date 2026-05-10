#include "Group.hpp"
#include "Student.hpp"

int main() {
    Student a("A", "001");
    a.addGrade(5.0);
    Group g("Test");
    g.addStudent(&a);
    return 0;
}
