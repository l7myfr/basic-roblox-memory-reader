#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>

#define CODE_RW CTL_CODE(FILE_DEVICE_UNKNOWN, 0x71, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define CODE_BA CTL_CODE(FILE_DEVICE_UNKNOWN, 0x72, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define CODE_GET_GUARDED_REGION CTL_CODE(FILE_DEVICE_UNKNOWN, 0x73, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define CODE_SECURITY 0x85b3e12

typedef struct _RW {
    INT32 security;
    INT32 process_id;
    ULONGLONG address;
    ULONGLONG buffer;
    ULONGLONG size;
    BOOLEAN write;
} RW, * PRW;

typedef struct _BA {
    INT32 security;
    INT32 process_id;
    ULONGLONG* address;
} BA, * PBA;

typedef struct _GA {
    INT32 security;
    ULONGLONG* address;
} GA, * PGA;

namespace mem {
    extern HANDLE driver_handle; 
    extern INT32 process_id;

    bool find_driver();
    void read_physical(PVOID address, PVOID buffer, DWORD size);
    void write_physical(PVOID address, PVOID buffer, DWORD size);
    uintptr_t find_image();
    uintptr_t get_guarded_region();
    INT32 find_process(LPCTSTR process_name);
}

template <typename T>
T read(uint64_t address) {

    T buffer{ };
    mem::read_physical((PVOID)address, &buffer, sizeof(T));
    return buffer;
}

template <typename T>
void write(uint64_t address, const T& buffer) {
    mem::write_physical((PVOID)address, (PVOID)&buffer, sizeof(T));
}
