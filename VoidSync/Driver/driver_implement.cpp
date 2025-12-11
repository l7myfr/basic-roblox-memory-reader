#include <iostream>
#include "driver_implement.h"

HANDLE mem::driver_handle = NULL;
INT32 mem::process_id = 0;

bool mem::find_driver() {
    driver_handle = CreateFileW(L"\\\\.\\IoControl", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    if (!driver_handle || driver_handle == INVALID_HANDLE_VALUE)
        return false;

    return true;
}

void mem::read_physical(PVOID address, PVOID buffer, DWORD size) {
    RW arguments = { 0 };
    arguments.security = CODE_SECURITY;
    arguments.address = (ULONGLONG)address;
    arguments.buffer = (ULONGLONG)buffer;
    arguments.size = size;
    arguments.process_id = process_id;
    arguments.write = FALSE;

    DeviceIoControl(driver_handle, CODE_RW, &arguments, sizeof(arguments), NULL, NULL, NULL, NULL);
}

void mem::write_physical(PVOID address, PVOID buffer, DWORD size) {
    RW arguments = { 0 };
    arguments.security = CODE_SECURITY;
    arguments.address = (ULONGLONG)address;
    arguments.buffer = (ULONGLONG)buffer;
    arguments.size = size;
    arguments.process_id = process_id;
    arguments.write = TRUE;

    DeviceIoControl(driver_handle, CODE_RW, &arguments, sizeof(arguments), NULL, NULL, NULL, NULL);
}

uintptr_t mem::find_image() {
    uintptr_t image_address = 0;
    BA arguments = { 0 };
    arguments.security = CODE_SECURITY;
    arguments.process_id = process_id;
    arguments.address = &image_address;

    DeviceIoControl(driver_handle, CODE_BA, &arguments, sizeof(arguments), NULL, NULL, NULL, NULL);
    return image_address;
}

uintptr_t mem::get_guarded_region() {
    uintptr_t guarded_region_address = 0;
    GA arguments = { 0 };
    arguments.security = CODE_SECURITY;
    arguments.address = &guarded_region_address;

    DeviceIoControl(driver_handle, CODE_GET_GUARDED_REGION, &arguments, sizeof(arguments), NULL, NULL, NULL, NULL);
    return guarded_region_address;
}

INT32 mem::find_process(LPCTSTR process_name) {
    PROCESSENTRY32 pt;
    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pt.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hsnap, &pt)) {
        do {
            if (!lstrcmpi(pt.szExeFile, process_name)) {
                CloseHandle(hsnap);
                process_id = pt.th32ProcessID;
                return pt.th32ProcessID;
            }
        } while (Process32Next(hsnap, &pt));
    }
    CloseHandle(hsnap);
    return 0;
}
