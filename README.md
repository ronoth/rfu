# rfu - Ronoth Firmware Updater

Firmware Updater for LoDev.

This project is based on
[stm32flash](https://sourceforge.net/projects/stm32flash/) and
just provides a thin wrapper to simplify the user interface.

# Usage
```rfu <port> <filename.bin>```

# Installation

## Windows

We provide a windows binary, just download the latest release and place the exe somewhere in your 
shells path. https://github.com/ronoth/rfu/releases

To compile from source you will need to use CGWIN/MSYS and install cmake.  Building from Visual Studio
is not supported.

## Linux / MacOS
Building from source requires cmake.  You can install it via apt/brew or your favorite distro's 
package manager and then run it followed by make.  Once you're done, toss the binary somewhere that is
in your environment path.

```
cmake .
make
```