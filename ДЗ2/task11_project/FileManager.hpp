#pragma once

#include <string>
#include <vector>

class Group;
class Student;

class FileManager {
public:
    static bool saveGroup(const Group& group, const std::string& path);
    static std::vector<Student*> loadGroup(Group& group, const std::string& path);
};
