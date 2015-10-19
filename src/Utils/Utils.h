/**
 * utils.h
 *
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2014/04/16
 * @copyright : NII 2014
 */


#ifndef _UTILS_H
#define _UTILS_H

#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>
#include <sys/time.h>

#include "../Options.h"

void displayProgressBar(unsigned progress, unsigned total);
void MSG(std::string msg, Options *o = NULL);
void error(std::string msg); 
    

class MSTimer {
private:
    struct timeval startTime;
    unsigned status;
public:
    MSTimer() : status(0) { }
    ~MSTimer() { };
    void start() {
        if (status!=0) {
            error("timer");   
        }
        gettimeofday(&startTime, NULL);
        status = 1;
    }
    void stop() {
        if (status!=1) {
            error("timer");
        }
        struct timeval endTime;
        gettimeofday(&endTime, NULL);
        long seconds  = endTime.tv_sec  - startTime.tv_sec;
        long useconds = endTime.tv_usec - startTime.tv_usec;
        long mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
        std::cout << mtime << std::flush;
        status = 0;
    }
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


#endif
