/**
 * \file Utils.cpp
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
