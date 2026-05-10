#pragma once

#include <string>
#include <vector>

class RecordBook {
public:
    explicit RecordBook(std::string number = "");
    void addGrade(double grade);
    double average() const;
    void print() const;

    const std::string& getNumber() const;
    const std::vector<double>& getGrades() const;

private:
    std::string number_;
    std::vector<double> grades_;
};
