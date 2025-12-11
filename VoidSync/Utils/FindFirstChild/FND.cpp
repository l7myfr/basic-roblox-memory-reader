#include "FND.h"
#include <iostream>
#include <fstream>
#include <string> // Required for std::string, std::getline, std::strtoull
#include <vector> // Required for std::vector
#include "../../Driver/driver_implement.h"
#include "../../Utils/Logs/logs.h"        
#include "../../Utils/Offsets/Offsets.h"   
#include "../../Utils/xorstr/xorstr.hpp"  



namespace RobloxFunctions {

    std::uint64_t get_render_view() // not sure that this even works lol
    {
        auto latest_log = RobloxLogs::get_latest_log(); 
        if (latest_log.empty()) {
            return 0;
        }

        std::ifstream rbx_log(latest_log);
        if (!rbx_log.is_open()) {
            return 0;
        }

        std::string rbx_log_line;
        const std::string target_str = XorStr("initialize view(");

        while (std::getline(rbx_log, rbx_log_line)) 
        {
            size_t pos = rbx_log_line.find(target_str);
            if (pos != std::string::npos)
            {
                rbx_log_line = rbx_log_line.substr(pos + target_str.length());
                size_t hex_start_pos = rbx_log_line.find(XorStr("0x"));
                if (hex_start_pos == std::string::npos) {
                    continue;
                }

                rbx_log_line = rbx_log_line.substr(hex_start_pos); 

                size_t end_bracket_pos = rbx_log_line.find(')');
                if (end_bracket_pos != std::string::npos) {
                    rbx_log_line = rbx_log_line.substr(0, end_bracket_pos);
                }
                else {
                    continue;
                }

                try {
                    std::uint64_t renderview = std::strtoull(rbx_log_line.c_str(), nullptr, 16);
                    rbx_log.close();
                    return renderview;
                }
                catch (const std::exception& e) {
                }
            }
        }
        rbx_log.close();
        return 0;
    }


    std::uint64_t get_visualengine_address()
    {
        auto render_view = get_render_view();
        if (render_view == 0) return 0; 

        auto visualengine =
            read<std::uint64_t>(render_view + 0x10); 
        // nvm i dont even use this anymore just ignore. found a new way to get datamoddel

        return visualengine;
    }


    std::string readstring(std::uint64_t address)
    {
        std::string string_val;
        if (address == 0) return string_val; 

        char character = 0;
        int offset = 0;

        string_val.reserve(64); 

        while (offset < 200)
        {
            character = read<char>(address + offset);

            if (character == 0) 
                break;

            string_val.push_back(character);
            offset += 1; 
        }

        return string_val;
    }

    std::string readstring2(std::uint64_t string_obj_address) 
    {
        if (string_obj_address == 0) return "";

        const auto length = read<int>(string_obj_address + RobloxOffsets::Misc::StringLength);
        if (length >= 16u) 
        {
            const auto char_array_ptr = read<std::uint64_t>(string_obj_address);
            if (char_array_ptr == 0) return "";
            return readstring(char_array_ptr); 
        }
        else 
        {
            return readstring(string_obj_address);
        }
    }

    std::string get_instance_name(std::uint64_t instance)
    {
        if (instance == 0) return XorStr("???");

        const auto name_string_obj_ptr = read<std::uint64_t>(instance + RobloxOffsets::Misc::Name);
        if (name_string_obj_ptr == 0)
            return XorStr("???");

        return readstring2(name_string_obj_ptr);
    }
    std::vector<std::uint64_t> get_instance_children(std::uint64_t instance)
    {
        std::vector<uint64_t> container;

            if (!instance)
                return container;

            auto start = read<std::uint64_t>(instance + RobloxOffsets::Children::ChildrenStart);

            if (!start)
                return container;

            auto end = read<std::uint64_t>(start + RobloxOffsets::Children::ChildrenEnd);

            for (auto instances = read<std::uint64_t>(start); instances != end; instances += 16)
                container.emplace_back(read<uint64_t>(instances));

            return container;
    }


    std::uint64_t find_first_child(std::uint64_t instance, const std::string& child_name)
    {
        if (instance == 0 || child_name.empty())
            return 0;

        std::vector<std::uint64_t> children = get_instance_children(instance); 
        for (const auto& child_instance : children)
        {
            if (get_instance_name(child_instance) == child_name)
            {
                return child_instance; 
            }
        }

        return 0; 
    }

} // namespace RobloxFunctions