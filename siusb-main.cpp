#include <iostream>
#include <bitset>
#include <windows.h>
#include <winnt.h>
#include "SiUSBXp.h"


int main(int argc, const char** argv) {

    HANDLE hMasterCOM = CreateFile("\\\\.\\COM26", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);



    char deviceString[128];
    int devStrLen = 0;
    SI_STATUS ret = SI_GetProductStringSafe(1, &deviceString, 128, SI_RETURN_SERIAL_NUMBER);
    if(ret != 0) {
        std::cout << "Error fetching product string: " << ret << std::endl;
    }

    return 0;
}