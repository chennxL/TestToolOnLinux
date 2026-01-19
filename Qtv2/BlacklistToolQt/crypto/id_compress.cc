#include "../include/id_compress.hpp"

#include <array>
#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace id {

static const std::vector<unsigned int> REGIONS = {
    11, 12, 13, 14, 15, 21, 22, 23, 31, 32, 33, 34, 35, 36, 37, 41, 42,
    43, 44, 45, 46, 50, 51, 52, 53, 54, 61, 62, 63, 64, 65, 71, 81, 82
};

static const std::map<unsigned int, unsigned int> REGION_MAP = [] {
    std::map<unsigned int, unsigned int> m;
    for (size_t i = 0; i < REGIONS.size(); ++i)
        m[REGIONS[i]] = i;
    return m;
}();

static const std::array<int, 17> WEIGHTS = {
    7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2,
};

static const std::array<char, 11> CHECK_DIGITS = {
    '1', '0', 'X', '9', '8', '7', '6', '5', '4', '3', '2',
};

static auto pad(unsigned int v, int w) -> std::string {
    std::string s = std::to_string(v);
    return std::string(w - s.length(), '0') + s;
}

static auto calc_check_digit(const std::string& id) -> char {
    int sum = 0;
    for (int i = 0; i < 17; ++i) {
        sum += (id[i] - '0') * WEIGHTS[i];
    }
    return CHECK_DIGITS[sum % 11];
}

auto compress(const std::string& _id) -> uint64_t {
    const std::string id = _id.substr(0, 17);

    for (char c : id) {
        if (c < '0' || c > '9') {
            throw std::invalid_argument("ID contains non-digit characters");
        }
    }

    uint64_t region = std::stoul(id.substr(0, 2));
    uint64_t area = std::stoul(id.substr(2, 4));
    uint64_t year = std::stoul(id.substr(6, 4));
    uint64_t month = std::stoul(id.substr(10, 2));
    uint64_t day = std::stoul(id.substr(12, 2));
    uint64_t seq = std::stoul(id.substr(14, 3));

    if (!REGION_MAP.count(region)) {
        throw std::invalid_argument("Invalid region code");
    }
    if (year < 1800 || year > 2055) {
        throw std::invalid_argument("Invalid year");
    }
    if (month < 1 || month > 12) {
        throw std::invalid_argument("Invalid month");
    }
    if (day < 1 || day > 31) {
        throw std::invalid_argument("Invalid day");
    }

    uint64_t region_idx = REGION_MAP.at(region);

    uint64_t year_offset = year - 1800;
    uint64_t region_area = region_idx * 10000 + area;

    uint64_t c = 0;
    c |= (region_area & 0x7FFFF) << 27;
    c |= (year_offset & 0xFF) << 19;
    c |= (month & 0x0F) << 15;
    c |= (day & 0x1F) << 10;
    c |= (seq & 0x3FF);
    return c;
}

auto decompress(uint64_t compressed) -> std::string {
    unsigned int region_area = (compressed >> 27) & 0x7FFFF;
    unsigned int year_offset = (compressed >> 19) & 0xFF;
    unsigned int month = (compressed >> 15) & 0x0F;
    unsigned int day = (compressed >> 10) & 0x1F;
    unsigned int seq = compressed & 0x3FF;

    unsigned int region_idx = region_area / 10000;
    unsigned int area = region_area % 10000;

    if (region_idx >= REGIONS.size()) {
        throw std::invalid_argument("Invalid compressed data");
    }

    unsigned int region = REGIONS[region_idx];
    unsigned int year = year_offset + 1800;

    std::string id = pad(region, 2) + pad(area, 4) + pad(year, 4) +
                     pad(month, 2) + pad(day, 2) + pad(seq, 3);

    return id + calc_check_digit(id);
}

}  // namespace id
