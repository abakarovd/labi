#include <iostream>
#include <string>
#include <vector>

class Person {
protected:
    std::string name;
public:
    explicit Person(std::string n) : name(std::move(n)) {}
    virtual ~Person() = default;
    virtual void print() const { std::cout << "Person: " << name << '\n'; }
};

class Student : public Person {
public:
    explicit Student(const std::string& n) : Person(n) {}
    void print() const override { std::cout << "Student: " << name << '\n'; }
};

class Teacher : public Person {
public:
    explicit Teacher(const std::string& n) : Person(n) {}
    void print() const override { std::cout << "Teacher: " << name << '\n'; }
};

int main() {
    std::vector<Person*> people;
    people.push_back(new Student("Anna"));
    people.push_back(new Teacher("Dr. Smith"));
    for (const Person* p : people) p->print();
    for (Person* p : people) delete p;
}
