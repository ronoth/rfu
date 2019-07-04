import time
import usb.core
import usb.util
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("mode", help="[read|write]")
parser.add_argument("--value", type=int, default=-1)
args = parser.parse_args()

PID = 0xea60
VID = 0x10c4

dev = usb.core.find(idVendor=VID, idProduct=PID, bus=3, address=28)
if not dev:
        print("CP2102N was not found :(")
        exit(1)
# print("Yeeha! Found CP2102")

REQTYPE_HOST_TO_INTERFACE = 0x41
REQTYPE_INTERFACE_TO_HOST = 0xc1
REQTYPE_HOST_TO_DEVICE = 0x40
REQTYPE_DEVICE_TO_HOST	= 0xc0

REQ_GPIO = 0xFF
READ_LATCH = 0x00C2
WRITE_LATCH = 0x37E1


# Without configuration GPIO3 only drives to 1.6v and GPIO2 doesn't seem to be working

# QFN20 only has these four GPIO
GPIO0 = 0x01
GPIO1 = 0x02
GPIO2 = 0x04
GPIO3 = 0x08

# QFN28 has 3 additional GPIO
GPIO4 = 0x16
GPIO5 = 0x32
GPIO6 = 0x64

NRST = GPIO2 # Active Low
BOOT0 = GPIO3 # Active High

def clear_gpio():
    write_gpio(0x00ff)

def gpio_on(gpio):
    write_gpio( (0xff << 8) + gpio )

def write_gpio(value):
    dev.ctrl_transfer(REQTYPE_HOST_TO_DEVICE, REQ_GPIO, WRITE_LATCH, value, [])    

def read_gpio():
    ret = dev.ctrl_transfer(REQTYPE_DEVICE_TO_HOST, REQ_GPIO, READ_LATCH, 0, 16)
    return ret[0]

def reset_gpio():
    clear_gpio()
    gpio_on(NRST)

if(__name__ == "__main__"):
#    import pdb; pdb.set_trace()
    if(args.mode == "read"):
        print(read_gpio())
        print("{0:07b}".format(read_gpio())[::-1])
    elif(args.mode == "write"):
        clear_gpio() # makes all pins low, enabling reset
        time.sleep(.5)
        gpio_on(BOOT0) # Pull BOOT0 high, signaling bootloader
        time.sleep(.5)
        gpio_on(NRST) # release reset pin
    elif(args.mode == "reset"):
        reset_gpio()
    elif(args.mode == "clear"):
        clear_gpio()
    else:
        print("INVALID mode argument")