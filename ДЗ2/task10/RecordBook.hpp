#pragma once

#include <string>
#include <vector>

class RecordBook {
public:
    explicit RecordBook(std::string number);
    void addGrade(double grade);
    double average() const;
    const std::string& number() const;
private:
    std::string number_;
    std::vector<double> grades_;
};
