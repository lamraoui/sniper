/**
 * utils.cpp
 *
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : simo@nii.ac.jp
 * @date : 2014/04/16
 * @copyright : NII 2014
 */

#include "utils.h"

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

// =============================================================================
// error
// 
// Print the given message and exit
// =============================================================================
void error(std::string msg) {
    std::cerr << "error: " << msg << std::endl;
    exit(1);
}