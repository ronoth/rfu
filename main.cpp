#include <iostream>
#include <bitset>
#include "Types.h"
#include "CP210xRuntimeDLL.h"


int main(int argc, const char** argv) {

    HANDLE hMasterCOM = CreateFile("\\\\.\\COM6", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);

    //1 2 4 8
    CP210x_STATUS ret;
    CP210xRT_WriteLatch(hMasterCOM, 0xffff, 0);
    Sleep(100);
    ret = CP210xRT_WriteLatch(hMasterCOM, 0x00ff, 0x02 | 0x01);
    if(ret != CP210x_SUCCESS) {
        std::cout << "ERROR Writing LATCH: " << ret << std::endl;
    }
    Sleep(100);


    WORD value;
    ret = CP210xRT_ReadLatch( hMasterCOM, &value);
    if(ret != CP210x_SUCCESS) {
        std::cout << "ERROR READING LATCH: " << ret << std::endl;
    }

    std::bitset<8> binary(value);
    std::cout << binary << std::endl;


    std::cout << "PRODUCT: ";
    CP210x_PRODUCT_STRING product[CP210x_MAX_PRODUCT_STRLEN];
    BYTE len = 0;
    ret = CP210xRT_GetDeviceProductString(hMasterCOM, &product, &len, TRUE);
    if(ret != CP210x_SUCCESS) {
        std::cout << "ERROR READING Product String: " << ret << std::endl;
    }
    fwrite(product, sizeof(char), len, stdout);
    std::cout << std::endl;

    std::cout << "SERIAL: ";
    CP210x_SERIAL_STRING serial[CP210x_MAX_SERIAL_STRLEN];
    ret = CP210xRT_GetDeviceSerialNumber(hMasterCOM, &serial, &len, TRUE);
    if(ret != CP210x_SUCCESS) {
        std::cout << "ERROR READING Product String: " << ret << std::endl;
    }
    fwrite(serial, sizeof(char), len, stdout);
    std::cout << std::endl;

    return 0;
}