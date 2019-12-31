//
// Created by steven on 7/14/19.
//

#ifndef RFU_SLEEPMS_H
#define RFU_SLEEPMS_H

#if defined(__WIN32__) || defined(__CYGWIN__)
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#endif

void sleep_ms(int milliseconds) // cross-platform sleep function
{
#if defined(__WIN32__) || defined(__CYGWIN__)
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(milliseconds * 1000);
#endif
}

#endif //RFU_SLEEPMS_H
