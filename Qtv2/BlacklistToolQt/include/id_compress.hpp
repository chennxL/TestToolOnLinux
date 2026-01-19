#pragma once

#include <cstdint>
#include <string>

namespace id {
    auto compress(const std::string& id) -> uint64_t;
    auto decompress(uint64_t compressed) -> std::string;
};
