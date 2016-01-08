/* Include in all files to print
    pc.printf("text")
    wrap any lines with D() to only compile if DEBUG is defined
*/

#ifndef __PC_PRINT_H__
#define __PC_PRINT_H__

#include "mbed.h"
#include "pindef.h"

static Serial pc(USB_SERIAL_TX, USB_SERIAL_RX);

#ifdef DEBUG 
    #define D(x) x
#else 
    #define D(x)
#endif

#endif