#include "FileManager.hpp"

#include "Group.hpp"
#include "Student.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {
struct Header {
    char signature[4] = {'G', 'R', 'P', '1'};
    std::uint32_t studentCount = 0;
};

bool writeString(std::ofstream& out, const std::string& s) {
    std::uint32_t len = static_cast<std::uint32_t>(s.size());
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(s.data(), static_cast<std::streamsize>(len));
    return static_cast<bool>(out);
}

bool readString(std::ifstream& in, std::string& s) {
    std::uint32_t len = 0;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (!in) return false;
    s.resize(len);
    in.read(&s[0], static_cast<std::streamsize>(len));
    return static_cast<bool>(in);
}
}  // namespace

bool FileManager::saveGroup(const Group& group, const std::string& path) {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        std::cerr << "Cannot open for write: " << path << '\n';
        return false;
    }

    Header h;
    h.studentCount = static_cast<std::uint32_t>(group.getStudents().size());

    out.write(reinterpret_cast<const char*>(&h), sizeof(h));
    if (!writeString(out, group.getName())) return false;

    for (const Student* s : group.getStudents()) {
        if (!s) continue;
        if (!writeString(out, s->getName())) return false;
        if (!writeString(out, s->getRecordBook().getNumber())) return false;

        const auto& grades = s->getRecordBook().getGrades();
        std::uint32_t count = static_cast<std::uint32_t>(grades.size());
        out.write(reinterpret_cast<const char*>(&count), sizeof(count));
        out.write(reinterpret_cast<const char*>(grades.data()), static_cast<std::streamsize>(grades.size() * sizeof(double)));
        if (!out) return false;
    }

    return true;
}

std::vector<Student*> FileManager::loadGroup(Group& group, const std::string& path) {
    std::vector<Student*> allocated;
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        std::cerr << "Cannot open for read: " << path << '\n';
        return allocated;
    }

    Header h;
    in.read(reinterpret_cast<char*>(&h), sizeof(h));
    if (!in || h.signature[0] != 'G' || h.signature[1] != 'R' || h.signature[2] != 'P' || h.signature[3] != '1') {
        std::cerr << "Invalid file format\n";
        return allocated;
    }

    std::string groupName;
    if (!readString(in, groupName)) return allocated;

    for (std::uint32_t i = 0; i < h.studentCount; ++i) {
        std::string name;
        std::string rb;
        if (!readString(in, name) || !readString(in, rb)) break;

        std::uint32_t gradeCount = 0;
        in.read(reinterpret_cast<char*>(&gradeCount), sizeof(gradeCount));
        if (!in) break;

        Student* st = new Student(name, rb);
        allocated.push_back(st);

        for (std::uint32_t j = 0; j < gradeCount; ++j) {
            double g = 0;
            in.read(reinterpret_cast<char*>(&g), sizeof(g));
            if (!in) break;
            st->addGrade(g);
        }

        group.addStudent(st);
    }

    return allocated;
}
