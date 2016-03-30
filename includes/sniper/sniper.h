/**
 * \file sniper.h
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
 * \date 14 January 2016
 */

#ifndef __SNIPER_H__
#define __SNIPER_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    /**
     *
     */
    void __attribute__ ((noinline)) sniper_assert(bool arg);
    
    /**
     *
     */
    void __attribute__ ((noinline)) sniper_assume(bool arg);
    
#ifdef __cplusplus
}
#endif

#endif //__SNIPER_H__