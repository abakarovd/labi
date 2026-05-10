#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

class Student {
    std::string name;
    double avg;
public:
    Student(std::string n, double a) : name(std::move(n)), avg(a) {}
    const std::string& getName() const { return name; }
    double getAverage() const { return avg; }
};

class Group {
    std::string title;
    std::vector<Student*> students;
public:
    explicit Group(std::string t) : title(std::move(t)) {}
    void add(Student* s) { students.push_back(s); }
    void removeByName(const std::string& n) {
        students.erase(std::remove_if(students.begin(), students.end(), [&](Student* s) { return s->getName() == n; }), students.end());
    }
    double average() const {
        if (students.empty()) return 0.0;
        double sum = 0;
        for (const Student* s : students) sum += s->getAverage();
        return sum / students.size();
    }
};

int main() {
    Student a("A", 4.7), b("B", 3.9);
    Group g("IU8");
    g.add(&a);
    g.add(&b);
    std::cout << g.average() << '\n';
    g.removeByName("B");
    std::cout << g.average() << '\n';
}
