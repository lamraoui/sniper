/**
 * \file sniper.h
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