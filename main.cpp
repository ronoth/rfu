#include <algorithm>
#include <iostream>
#include <bitset>
#include "include/Types.h"
#include "lib/CP210xRuntimeDLL.h"

#include "include/cxxopts.hpp"

void help() {

}

int main(int argc, char** argv) {


    cxxopts::Options options("Ronoth DFU", "Device Firmware Upgrade for devices by Ronoth, LLC");
    options.add_options()
            ("d,device", "Device Path / COM Port", cxxopts::value<std::string>())
            ("h,help", "Display this message");

    try {
        auto opts = options.parse(argc, argv);


        if (opts.count("help")) {
            std::cout << options.help();
            exit(1);
        }
        if (opts.count("device") != 1) {
            std::cout << "Must specify serial device" << std::endl;
            std::cout << options.help();
            exit(1);
        }

        LPCSTR device = opts["device"].as<std::string>().c_str();

        auto hMasterCOM = CreateFile(device, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);


        CP210xRT_WriteLatch(hMasterCOM, 0xffff, 0);
        Sleep(100);
        auto ret = CP210xRT_WriteLatch(hMasterCOM, 0x00ff, 1);
        if(ret != CP210x_SUCCESS) {
            std::cout << "ERROR Writing LATCH: " << ret << std::endl;
        }
        Sleep(100);


        WORD value;
        ret = CP210xRT_ReadLatch( hMasterCOM, &value);
        if(ret != CP210x_SUCCESS) {
            std::cout << "ERROR READING LATCH: " << ret << std::endl;
        }

        std::bitset<7> binary(value);
        auto bstr = binary.to_string();
        std::reverse(bstr.begin(), bstr.end());
        std::cout <<  bstr << std::endl;


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

    } catch(cxxopts::OptionException& exception) {
        std::cout << exception.what() << std::endl;
        std::cout << options.help();
        exit(1);
    }



    return 0;
}