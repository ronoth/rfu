import ctypes
import os
import serial
import serial.tools.list_ports

URL_PROTOCOL = "silabs://"

#TODO: Add maps for other CP210x devices
CP210X_HWID = {
    "cp2102" : "10C4:EA60",
    "cp2103" : "10C4:EA60",
    "cp2104" : "10C4:EA60",
    "cp2105" : "10C4:EA70",
    "cp2108" : "10C4:EA71",
}
CP210X_HWID_DEFAULT = "10C4:EA"

def cp210x_errcheck(result, func, args):
    if result != 0:
        raise serial.SerialException("CP210x runtime error: %d" % result)

if os.name == 'nt':
    _DLL = ctypes.WinDLL(os.path.join(os.path.dirname(__file__), "CP210xRuntime.dll"))

    for cp210x_function in [
        "CP210xRT_ReadLatch", 
        "CP210xRT_WriteLatch",
        "CP210xRT_GetPartNumber", 
        "CP210xRT_GetDeviceProductString",
        "CP210xRT_GetDeviceSerialNumber",
        "CP210xRT_GetDeviceInterfaceString"]:
        fnc = getattr(_DLL, cp210x_function)
        fnc.restype = ctypes.c_int
        fnc.errcheck = cp210x_errcheck

class Serial(serial.Serial):
    """Adds CP210x specific functionality to the base serial.Serial class."""

    def setPort(self, value):
        """Translates the port name before storing it."""
        if isinstance(value, str) and value.lower().startswith(URL_PROTOCOL):
            serial.Serial.setPort(self, self.fromURL(value))
        else:
            serial.Serial.setPort(self, value)

    def fromURL(self, url):
        """Determines port name from an URL string."""
        url = url.lower()
        if url.startswith(URL_PROTOCOL): 
            url = CP210X_HWID.get(url[len(URL_PROTOCOL):], CP210X_HWID_DEFAULT)

        # Use a for loop to get the 1st element from the generator
        # TODO: Add the ability to open any CP210x interface
        for port, desc, hwid in sorted(serial.tools.list_ports.grep(url), key=lambda x: x[1]):
            return port
        else:
            raise serial.SerialException('no ports found matching hwid %r' % (url,))

    # Override the setter for serial.Serial.port property to use our custom URL version
    port = property(serial.Serial.getPort, setPort, doc="Port setting")

    # CP210xRT_ReadLatch(HANDLE cyHandle, LPWORD lpLatch);
    def ReadLatch(self):
        """Returns the current port latch value from the CP210x device."""
        latch = ctypes.c_ushort(0)
        _DLL.CP210xRT_ReadLatch(self.hComPort, ctypes.byref(latch))
        return latch.value

    # CP210xRT_WriteLatch(HANDLE cyHandle, WORD mask, WORD latch);
    def WriteLatch(self, mask, latch):
        """Sets the current port latch value for the CP210x device."""
        _DLL.CP210xRT_WriteLatch(self.hComPort, mask, latch)

    # CP210xRT_GetPartNumber(HANDLE cyHandle, LPBYTE lpbPartNum);
    def GetPartNumber(self):
        """Returns the part number from the CP210x device."""
        part_num = ctypes.c_byte(0)
        _DLL.CP210xRT_GetPartNumber(self.hComPort, ctypes.byref(part_num))
        return part_num.value

    # CP210xRT_GetDeviceProductString(HANDLE cyHandle, LPVOID lpProduct,
    #       LPBYTE lpbLength, BOOL bConvertToASCII = TRUE);
    def GetDeviceProductString(self):
        """Returns the product string from the CP210x device."""
        buf = ctypes.create_string_buffer(128)
        cnt = ctypes.c_byte(0)
        _DLL.CP210xRT_GetDeviceProductString(self.hComPort, buf, ctypes.byref(cnt), True)
        return buf.value.decode()
        
    # CP210xRT_GetDeviceSerialNumber(HANDLE cyHandle, LPVOID lpSerialNumber,
    #       LPBYTE lpbLength, BOOL bConvertToASCII = TRUE);
    def GetDeviceSerialNumber(self):
        """Returns the serial number string from the CP210x device."""
        buf = ctypes.create_string_buffer(128)
        cnt = ctypes.c_byte(0)
        _DLL.CP210xRT_GetDeviceSerialNumber(self.hComPort, buf, ctypes.byref(cnt), True)
        return buf.value.decode()

    # CP210xRT_GetDeviceInterfaceString(HANDLE cyHandle, LPVOID lpInterfaceString,
    #       LPBYTE lpbLength, BOOL bConvertToASCII = TRUE);
    def GetDeviceInterfaceString(self):
        """Returns the interface string from the CP210x device."""
        buf = ctypes.create_string_buffer(128)
        cnt = ctypes.c_byte(0)
        _DLL.CP210xRT_GetDeviceInterfaceString(self.hComPort, buf, ctypes.byref(cnt), True)
        return buf.value.decode()


if __name__ == "__main__":
    import sys

    try:
        port = Serial("silabs://CP210x")
        print("Serial.port: %s" % port.port)
        print("GetDeviceProductString() = %s" % port.GetDeviceProductString())
        print("GetDeviceSerialNumber()  = %s" % port.GetDeviceSerialNumber())
        print("GetDeviceInterfaceString()  = %s" % port.GetDeviceInterfaceString())
        PartNumber = port.GetPartNumber()
        print("GetPartNumber() = %s" % PartNumber)
        if PartNumber > 2 and PartNumber < 0x22:
            print("ReadLatch()     = %s" % hex(port.ReadLatch()))

    except serial.SerialException as e:
        print("SerialException: %s" % e)
        sys.exit(1)
