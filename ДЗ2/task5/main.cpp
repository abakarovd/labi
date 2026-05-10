#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#pragma pack(push, 1)
struct FileHeader {
    char signature[4];
    uint16_t version;
    uint32_t students;
};
#pragma pack(pop)

int main() {
    std::vector<std::vector<double>> grades = {{4.0, 5.0}, {3.5, 4.5}};

    FileHeader h{{'G', 'R', 'D', 'S'}, 1, static_cast<uint32_t>(grades.size())};
    std::cout << "sizeof(FileHeader)=" << sizeof(FileHeader) << "\n";

    std::ofstream out("data.bin", std::ios::binary);
    out.write(reinterpret_cast<const char*>(&h), sizeof(h));
    uint32_t m = grades.empty() ? 0u : static_cast<uint32_t>(grades[0].size());
    out.write(reinterpret_cast<const char*>(&m), sizeof(m));
    for (const auto& row : grades) out.write(reinterpret_cast<const char*>(row.data()), static_cast<std::streamsize>(row.size() * sizeof(double)));
    out.close();

    std::ifstream in("data.bin", std::ios::binary);
    FileHeader r{};
    in.read(reinterpret_cast<char*>(&r), sizeof(r));
    if (r.signature[0] != 'G' || r.signature[1] != 'R' || r.signature[2] != 'D' || r.signature[3] != 'S') {
        std::cout << "Bad signature\n";
        return 1;
    }
    uint32_t rm = 0;
    in.read(reinterpret_cast<char*>(&rm), sizeof(rm));
    std::vector<std::vector<double>> loaded(r.students, std::vector<double>(rm));
    for (auto& row : loaded) in.read(reinterpret_cast<char*>(row.data()), static_cast<std::streamsize>(row.size() * sizeof(double)));

    for (const auto& row : loaded) {
        for (double v : row) std::cout << v << ' ';
        std::cout << '\n';
    }
}
