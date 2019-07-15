//
// Created by steven on 7/14/19.
//

#if defined(__WIN32__) || defined(__CYGWIN__)
#include "serial-gpio-win32.cpp"
#else
#include "serial-gpio-posix.cpp"
#endif