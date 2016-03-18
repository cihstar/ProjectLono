/* Pin Definitions File! */

#ifndef __PIN_DEF_H__
#define __PIN_DEF_H__

#include "mbed.h"

#define USB_SERIAL_TX USBTX
#define USB_SERIAL_RX USBRX

#define BOARD_LED_1 LED1
#define BOARD_LED_2 LED2
#define BOARD_LED_3 LED3
#define BOARD_LED_4 LED4

#define GSM_TX p28
#define GSM_RTS p21
#define GSM_RX p27
#define GSM_CTS p22
#define GSM_TERM_ON p26
#define GSM_RESET p30

#define SD_MOSI p5
#define SD_MISO p6
#define SD_CLK p7
#define SD_CS p8

#define P_SENSE_OUT p15
#define P_SENSE_SLEEP p30

#define LCD_SDA p9
#define LCD_SCL p10
#define LCD_RST p11

#define PB1 p25
#define PB2 p23
#define PB3 p24

#define BATTERY_LEVEL p16

#define XBEE_TX p13
#define XBEE_RX p14

#endif