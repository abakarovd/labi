#include <iostream>
#include <string>
#include <vector>

class RecordBook {
    std::string number;
    std::vector<double> grades;
public:
    explicit RecordBook(std::string num) : number(std::move(num)) {}
    void addGrade(double g) { if (g >= 0 && g <= 5) grades.push_back(g); }
    double average() const {
        if (grades.empty()) return 0.0;
        double s = 0;
        for (double g : grades) s += g;
        return s / grades.size();
    }
};

class Student {
    std::string name;
    RecordBook rb;
public:
    Student(std::string n, std::string num) : name(std::move(n)), rb(std::move(num)) {}
    void addGrade(double g) { rb.addGrade(g); }
    double average() const { return rb.average(); }
};

int main() {
    Student s("Oleg", "RB-001");
    s.addGrade(5);
    s.addGrade(4);
    std::cout << s.average() << '\n';
}
