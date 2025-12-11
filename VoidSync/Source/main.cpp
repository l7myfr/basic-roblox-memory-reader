#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <string>
#include "../Utils/Logs/logs.h"
#include "../Utils/Offsets/Offsets.h"
#include "../Driver/driver_data.h"     
#include "../KDMapper/include/kdmapper.hpp"
#include "../KDMapper/include/intel_driver.hpp"
#include "../Driver/driver_implement.h" 
#include "../Utils/FindFirstChild/FND.h"  
#include "../Utils/xorstr/xorstr.hpp"
#include "../Cheat/PlayerCache.hpp"

const char* PROCESS_NAME = "RobloxPlayerBeta.exe";



uintptr_t GetModuleBaseAddress(DWORD processID, const char* moduleName) {
    uintptr_t baseAddress = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;

    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(snapshot, &moduleEntry)) {
        do {
            if (_stricmp(moduleEntry.szModule, moduleName) == 0) {
                baseAddress = (uintptr_t)moduleEntry.modBaseAddr;
                break;
            }
        } while (Module32Next(snapshot, &moduleEntry));
    }
    CloseHandle(snapshot);
    return baseAddress;
}

int main() {
    DWORD processID = mem::find_process(PROCESS_NAME);
    if (!processID) {
        std::cerr << "❌ Failed to find Roblox process.\n";
        system("pause");
        return 1;
    }
    mem::process_id = processID;
    std::cout << "✅ Roblox Process ID: " << processID << "\n";

    NTSTATUS status = intel_driver::Load();
    if (!NT_SUCCESS(status)) {
        std::cerr << "❌ Failed to load vulnerable driver.\n";
        system("pause");

        return -1;
    }

    std::cout << "✅ Vulnerable driver loaded successfully.\n";
    size_t RawSize = sizeof(RawData);  

    if (!kdmapper::MapDriver(RawData, RawSize)) {
        std::cerr << "❌ Failed to map driver!\n";
        intel_driver::Unload(); 
        system("pause");
        return -1;
    }

    std::cout << "✅ Driver successfully mapped!\n";

    if (!mem::find_driver()) {
        std::cerr << "❌ Custom driver not loaded or not responding!\n";
        intel_driver::Unload(); 
        system("pause");
        return -1;
    }
    std::cout << "✅ Custom driver is responsive.\n";

    uintptr_t baseAddress = GetModuleBaseAddress(processID, PROCESS_NAME);
    if (!baseAddress) {
        std::cerr << "❌ Failed to find Roblox base address.\n";
        intel_driver::Unload(); 
        system("pause");
        return 1;
    }
    std::cout << "✅ Roblox Base Address: 0x" << std::hex << baseAddress << std::dec << "\n";

    //std::wstring Username = RobloxLogs::get_username();
    //std::wcout << "✅ Got Player's Username: " << Username << std::endl; 
    // print pc user instead

    std::uint64_t fakeDataModelGlobalPtrLocation = baseAddress + RobloxOffsets::Game::FakeDataModelPointer;

    std::uint64_t fakeDataModelInstanceAddr = read<std::uint64_t>(fakeDataModelGlobalPtrLocation);
    if (fakeDataModelInstanceAddr == 0) {
        return 0;
    }

    std::uint64_t Game = read<std::uint64_t>(fakeDataModelInstanceAddr + RobloxOffsets::Game::FakeDatamodelToRealDatamodel);
    if (Game == 0) {
        std::cerr << "❌ Unable to get DataModel from VisualEngine\n";
        intel_driver::Unload();  
        system("pause");
        return 1;
    }
    std::cout << "✅ Got Datamodel: 0x" << std::hex << Game << std::dec << "\n";

    std::string gameInstanceName = RobloxFunctions::get_instance_name(Game);
    std::cout << "Game Instance Name: " << gameInstanceName << std::endl;

    std::cout << "✅Game Loaded" << std::hex << read<std::uint64_t>(Game + RobloxOffsets::Game::GameLoaded) << std::dec << "\n";

    auto Players = RobloxFunctions::find_first_child(Game, XorStr("Players"));
    if (!Players) {
        std::cerr << "❌ Unable to get Players\n";
        intel_driver::Unload();  
        system("pause");
        return 1;
    }
    std::cout << "✅ Got PlayerService: 0x" << std::hex << Players << std::dec << "\n";
    std::vector<std::uint64_t> PlayersChildrens = RobloxFunctions::get_instance_children(Players);

    for (const auto& Player : PlayersChildrens) {
        std::uint64_t Character = read<std::uint64_t>(Player + RobloxOffsets::Player::Character);

        if (!Character) {
            std::cout << "No character: " << RobloxFunctions::get_instance_name(Player) << std::endl;
            continue; 
        }

        auto HRP = RobloxFunctions::find_first_child(Character, XorStr("HumanoidRootPart"));
        auto Humanoid = RobloxFunctions::find_first_child(Character, XorStr("Humanoid"));

        if (!HRP) {
            std::cout << "No HRP: " << RobloxFunctions::get_instance_name(Player) << std::endl;
            continue;
        }
        if (!Humanoid) {
            std::cout << "No Humanoid: " << RobloxFunctions::get_instance_name(Player) << std::endl;
            continue;
        }
        uintptr_t primitive = read<uintptr_t>(HRP + RobloxOffsets::Workspace::Primitive);

        if (!primitive) {
            std::cout << "No primitive: " << RobloxFunctions::get_instance_name(Player) << std::endl;
            continue; 
        }

        Vector3 Position = read<Vector3>(primitive + RobloxOffsets::Workspace::Position);
        std::string playerName = RobloxFunctions::get_instance_name(Player);
        std::cout << "Player: " << playerName << " | Position: (" << Position.x << ", " << Position.y << ", " << Position.z << ")\n";

        float Health = read<float>(Humanoid + RobloxOffsets::Player::Health);
        std::cout << "Player Health: " << Health << "\n";
        

    }

    std::cout << "Unloading vulnerable driver (iqvw64e.sys)...\n";
    if (intel_driver::Unload()) {
        std::cout << "✅ Vulnerable driver unloaded successfully.\n";
    }
    else {
        std::cerr << "⚠️ Failed to unload vulnerable driver. It might have already been unloaded or an error occurred.\n";
    }

    std::cout << "Exiting application.\n";
    system("pause");
    return 0;
}
