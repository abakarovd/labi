#include "FileManager.hpp"
#include "Group.hpp"
#include "Student.hpp"
#include "Teacher.hpp"

#include <iostream>
#include <vector>

int main() {
    Student* s1 = new Student("Anna", "RB001");
    Student* s2 = new Student("Boris", "RB002");
    Student* s3 = new Student("Clara", "RB003");

    s1->addGrade(5.0);
    s1->addGrade(4.5);
    s2->addGrade(3.0);
    s2->addGrade(2.5);
    s3->addGrade(4.0);
    s3->addGrade(4.2);

    Group group("IU8-11");
    group.addStudent(s1);
    group.addStudent(s2);
    group.addStudent(s3);

    std::cout << "== Original group ==\n";
    group.printAll();
    std::cout << "Group average: " << group.average() << "\n";

    group.sortByAverageDesc();
    std::cout << "\n== Sorted ==\n";
    group.printAll();

    std::cout << "\nExcellent: " << group.countExcellent() << " Risk: " << group.countRisk() << "\n";

    group.filterByAverage(3.0);
    std::cout << "\n== Filtered (>=3.0) ==\n";
    group.printAll();

    const std::string path = "group.bin";
    if (FileManager::saveGroup(group, path)) {
        std::cout << "Saved to " << path << '\n';
    }

    Group loaded("loaded");
    std::vector<Student*> loadedStudents = FileManager::loadGroup(loaded, path);
    std::cout << "\n== Loaded group ==\n";
    loaded.printAll();

    Teacher t("Prof. Petrov", "C++");
    t.print();

    delete s1;
    delete s2;
    delete s3;
    for (Student* s : loadedStudents) delete s;

    return 0;
}
