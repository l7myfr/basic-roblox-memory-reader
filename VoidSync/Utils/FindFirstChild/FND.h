#pragma once

#include <string>       
#include <vector>      
#include <cstdint>      

namespace RobloxFunctions {
    std::string readstring(std::uint64_t address);

    std::string readstring2(std::uint64_t string_address);

    std::string get_instance_name(std::uint64_t instance);

    std::vector<std::uint64_t> get_instance_children(std::uint64_t instance);

    std::uint64_t find_first_child(std::uint64_t instance, const std::string& child_name);
    std::uint64_t get_render_view();
    std::uint64_t get_visualengine_address();

}

