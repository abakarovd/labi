#include <iostream>
#include <string>
#include <vector>

class Student {
private:
    std::string name;
    std::vector<double> grades;

public:
    Student() : name("Unknown") {}
    explicit Student(std::string n) : name(std::move(n)) {}

    void addGrade(double g) { if (g >= 0 && g <= 5) grades.push_back(g); }

    double average() const {
        if (grades.empty()) return 0.0;
        double s = 0;
        for (double g : grades) s += g;
        return s / grades.size();
    }

    void print() const {
        std::cout << name << " avg=" << average() << " grades: ";
        for (double g : grades) std::cout << g << ' ';
        std::cout << '\n';
    }
};

int main() {
    Student a;
    Student b("Ivan");
    b.addGrade(5);
    b.addGrade(4.5);
    a.print();
    b.print();
}
