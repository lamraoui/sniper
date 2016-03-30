/**
 * \file Utils.h
 *
 * ----------------------------------------------------------------------
 *                SNIPER : Automatic Fault Localization 
 *
 * Copyright (C) 2016 Si-Mohamed LAMRAOUI
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program (see LICENSE.TXT).  
 * If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------------------
 *
 * \author Si-Mohamed Lamraoui
 * \date   26 January 2015
 */

#ifndef _UTILS_H
#define _UTILS_H

#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>
#include <sys/time.h>

#include "../Options.h"

/**
 * Print a progress bar to the standard output.
 *
 * \param progress Current progress (>= 0 and <= \p total).
 * \param total Total progress (100%).
 */
void displayProgressBar(unsigned progress, unsigned total);
/**
 * Print the given message if the user passed the option \a -dbg-msg.
 *
 * \param msg A message to display to the standard output.
 * \param o User's options.
 */
void MSG(std::string msg, Options *o = NULL);
  
/**
 * \class MSTimer
 *
 * This class provides a timer.
 */
class MSTimer {
    
private:
    struct timeval startTime;
    unsigned status;
    
public:
    /**
     * Default constructor. 
     */
    MSTimer() : status(0) { }
    /**
     * Destructor.
     */
    ~MSTimer() { };
    
    /**
     * Start the timer.
     */
    void start() {
        assert(status==0 && "Timer is already running!");
        gettimeofday(&startTime, NULL);
        status = 1;
    }
    
    /**
     * Stop the timer and print the elapsed time (ms).
     */
    void stop() {
        assert(status==1 && "Timer is not running!");
        struct timeval endTime;
        gettimeofday(&endTime, NULL);
        long seconds  = endTime.tv_sec  - startTime.tv_sec;
        long useconds = endTime.tv_usec - startTime.tv_usec;
        long mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
        std::cout << mtime << std::flush;
        status = 0;
    }
    
    /**
     * Stop the timer, print a message and the elapsed time (ms).
     *
     * \param msg A message to be display to the standard output.
     */
    void stop(std::string msg) {
        std::cout << msg << ": "; 
        stop();
        std::cout << " ms\n";
    }
    
/*
//  Windows
#ifdef _WIN32
#include <Windows.h>
    double get_wall_time(){
        LARGE_INTEGER time,freq;
        if (!QueryPerformanceFrequency(&freq)){
            //  Handle error
            return 0;
        }
        if (!QueryPerformanceCounter(&time)){
            //  Handle error
            return 0;
        }
        return (double)time.QuadPart / freq.QuadPart;
    }
    double get_cpu_time(){
        FILETIME a,b,c,d;
        if (GetProcessTimes(GetCurrentProcess(),&a,&b,&c,&d) != 0){
            //  Returns total user time.
            //  Can be tweaked to include kernel times as well.
            return
            (double)(d.dwLowDateTime |
                     ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
        }else{
            //  Handle error
            return 0;
        }
    }
//  Posix/Linux
#else
#include <sys/time.h>
    double get_wall_time(){
        struct timeval time;
        if (gettimeofday(&time,NULL)){
            //  Handle error
            return 0;
        }
        return (double)time.tv_sec + (double)time.tv_usec * .000001;
    }
    double get_cpu_time(){
        return (double)clock() / CLOCKS_PER_SEC;
    }
#endif
 */
    
};

#endif // _UTILS_H
