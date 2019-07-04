#include <algorithm>
#include <iostream>
#include <bitset>
#include <iomanip>
#include "include/Types.h"
#include "lib/CP210xRuntimeDLL.h"

#include "include/cxxopts.hpp"

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

        auto device = opts["device"].as<std::string>().c_str();


        // Check to see if the device passed is a valid com port
        TCHAR lpTargetPath[255];
        if(QueryDosDevice(device, lpTargetPath, 255) == 0) {
            std::cout << "INVALID Serial Device: " << device << std::endl;
            exit(1);
        }

        char devbuf[strlen(device)+5];
        sprintf(devbuf, "\\\\.\\%s", device);
        auto hMasterCOM = CreateFile(devbuf, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);



        auto ret = CP210xRT_WriteLatch(hMasterCOM, 0xffff, 0);
        Sleep(100);

        uint16_t latchval = CP210x_GPIO_1;
        CP210xRT_WriteLatch(hMasterCOM, latchval, latchval);
        if (ret != CP210x_SUCCESS) {
            std::cout << "ERROR Writing LATCH: " << ret << std::endl;
        }
        Sleep(100);

        ret = CP210xRT_ReadLatch(hMasterCOM, &latchval);
        if (ret != CP210x_SUCCESS) {
            std::cout << "ERROR READING LATCH: " << ret << std::endl;
        }

        std::bitset<8> binary(latchval);
        auto bstr = binary.to_string();
        std::reverse(bstr.begin(), bstr.end());
        std::cout << bstr << std::endl;


        std::cout << "PRODUCT: ";
        CP210x_PRODUCT_STRING product[CP210x_MAX_PRODUCT_STRLEN];
        BYTE len = 0;
        CP210xRT_GetDeviceProductString(hMasterCOM, &product, &len, TRUE);
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