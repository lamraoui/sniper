/**
 * \file Utils.cpp
 *
 * \author Si-Mohamed Lamraoui
 * \date   26 January 2015
 */

#include "Utils.h"

// =============================================================================
// displayProgressBar
//
// Print a progress bar
// =============================================================================
void displayProgressBar(unsigned progress, unsigned total) {
    std::cout << "[";
    int pos = ((progress*100)/total)/2;
    for (int i = 0; i < 50; ++i) {
        if (i <= pos) std::cout << "▒";
        else std::cout << " ";
    }
    std::cout << std::setprecision(2) << std::fixed;
    std::cout << "] "
    << ((float)(progress*100)/(float)total) << " %\r";
    std::cout.flush();
}

// =============================================================================
// MSG
// 
// Print the given message 
// if the user did not use -dis-opt-msg
// =============================================================================
void MSG(std::string msg, Options *o) {
    if(!o || o->dbgMsg()) {
        std::cout << msg;
    }
}
