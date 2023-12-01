//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
// School: University of Victoria, Canada.
// Course: ECE 355 "Microprocessor-Based Systems".
// PWM Signal Generation and Monitoring System
// Isaac Northrop - V00976549
// Saxon Forster - V00990191
//
// See "system/include/cmsis/stm32f051x8.h" for register/bit definitions.
// See "system/src/cmsis/vectors_stm32f051x8.c" for handler declarations.
// ----------------------------------------------------------------------------

#include <stdio.h>
#include "diag/Trace.h"
#include "cmsis/cmsis_device.h"
#include <time.h>


// ----- main() ---------------------------------------------------------------


/* Definitions of registers and their bits are
   given in system/include/cmsis/stm32f051x8.h */


/* Clock prescaler for TIM2 timer: no prescaling */
#define myTIM2_PRESCALER ((uint16_t)0x0000)
/* Maximum possible setting for overflow */
#define myTIM2_PERIOD ((uint32_t)0xFFFFFFFF)
/* Clock prescaler for TIM3 timer: no prescaling */
#define myTIM3_PRESCALER ((uint32_t)0x47999)
/* Default overflow to be changed in delay() */
#define myTIM3_PERIOD ((uint32_t)0x64)

/* Function Prototypes */
void myGPIOA_Init(void);
void myGPIOB_Init(void);
void myTIM2_Init(void);
void myTIM3_Init(void);
void myEXTI_Init(void);
void myADC_Init(void);
void myDAC_Init(void);
void mySPI_Init(void);
void delay(uint32_t cycles);
void refresh_OLED(void);
void oled_Write_Cmd(unsigned char cmd);
void oled_Write_Data(unsigned char data);
void oled_Write(unsigned char Value);


/* SPI Init Struct */
SPI_HandleTypeDef SPI_Handle;

// LED Display initialization commands
unsigned char oled_init_cmds[] =
{
    0xAE,
    0x20, 0x00,
    0x40,
    0xA0 | 0x01,
    0xA8, 0x40 - 1,
    0xC0 | 0x08,
    0xD3, 0x00,
    0xDA, 0x32,
    0xD5, 0x80,
    0xD9, 0x22,
    0xDB, 0x30,
    0x81, 0xFF,
    0xA4,
    0xA6,
    0xAD, 0x30,
    0x8D, 0x10,
    0xAE | 0x01,
    0xC0,
    0xA0
};


// Character specifications for LED Display (1 row = 8 bytes = 1 ASCII character)
// Example: to display '4', retrieve 8 data bytes stored in Characters[52][X] row
//          (where X = 0, 1, ..., 7) and send them one by one to LED Display.
// Row number = character ASCII code (e.g., ASCII code of '4' is 0x34 = 52)

unsigned char Characters[][8] = {
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b01011111, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // !
    {0b00000000, 0b00000111, 0b00000000, 0b00000111, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // "
    {0b00010100, 0b01111111, 0b00010100, 0b01111111, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // #
    {0b00100100, 0b00101010, 0b01111111, 0b00101010, 0b00010010,0b00000000, 0b00000000, 0b00000000},  // $
    {0b00100011, 0b00010011, 0b00001000, 0b01100100, 0b01100010,0b00000000, 0b00000000, 0b00000000},  // %
    {0b00110110, 0b01001001, 0b01010101, 0b00100010, 0b01010000,0b00000000, 0b00000000, 0b00000000},  // &
    {0b00000000, 0b00000101, 0b00000011, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // '
    {0b00000000, 0b00011100, 0b00100010, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // (
    {0b00000000, 0b01000001, 0b00100010, 0b00011100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // )
    {0b00010100, 0b00001000, 0b00111110, 0b00001000, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // *
    {0b00001000, 0b00001000, 0b00111110, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // +
    {0b00000000, 0b01010000, 0b00110000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // ,
    {0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // -
    {0b00000000, 0b01100000, 0b01100000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // .
    {0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010,0b00000000, 0b00000000, 0b00000000},  // /
    {0b00111110, 0b01010001, 0b01001001, 0b01000101, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // 0
    {0b00000000, 0b01000010, 0b01111111, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // 1
    {0b01000010, 0b01100001, 0b01010001, 0b01001001, 0b01000110,0b00000000, 0b00000000, 0b00000000},  // 2
    {0b00100001, 0b01000001, 0b01000101, 0b01001011, 0b00110001,0b00000000, 0b00000000, 0b00000000},  // 3
    {0b00011000, 0b00010100, 0b00010010, 0b01111111, 0b00010000,0b00000000, 0b00000000, 0b00000000},  // 4
    {0b00100111, 0b01000101, 0b01000101, 0b01000101, 0b00111001,0b00000000, 0b00000000, 0b00000000},  // 5
    {0b00111100, 0b01001010, 0b01001001, 0b01001001, 0b00110000,0b00000000, 0b00000000, 0b00000000},  // 6
    {0b00000011, 0b00000001, 0b01110001, 0b00001001, 0b00000111,0b00000000, 0b00000000, 0b00000000},  // 7
    {0b00110110, 0b01001001, 0b01001001, 0b01001001, 0b00110110,0b00000000, 0b00000000, 0b00000000},  // 8
    {0b00000110, 0b01001001, 0b01001001, 0b00101001, 0b00011110,0b00000000, 0b00000000, 0b00000000},  // 9
    {0b00000000, 0b00110110, 0b00110110, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // :
    {0b00000000, 0b01010110, 0b00110110, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // ;
    {0b00001000, 0b00010100, 0b00100010, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // <
    {0b00010100, 0b00010100, 0b00010100, 0b00010100, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // =
    {0b00000000, 0b01000001, 0b00100010, 0b00010100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // >
    {0b00000010, 0b00000001, 0b01010001, 0b00001001, 0b00000110,0b00000000, 0b00000000, 0b00000000},  // ?
    {0b00110010, 0b01001001, 0b01111001, 0b01000001, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // @
    {0b01111110, 0b00010001, 0b00010001, 0b00010001, 0b01111110,0b00000000, 0b00000000, 0b00000000},  // A
    {0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b00110110,0b00000000, 0b00000000, 0b00000000},  // B
    {0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00100010,0b00000000, 0b00000000, 0b00000000},  // C
    {0b01111111, 0b01000001, 0b01000001, 0b00100010, 0b00011100,0b00000000, 0b00000000, 0b00000000},  // D
    {0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b01000001,0b00000000, 0b00000000, 0b00000000},  // E
    {0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // F
    {0b00111110, 0b01000001, 0b01001001, 0b01001001, 0b01111010,0b00000000, 0b00000000, 0b00000000},  // G
    {0b01111111, 0b00001000, 0b00001000, 0b00001000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // H
    {0b01000000, 0b01000001, 0b01111111, 0b01000001, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // I
    {0b00100000, 0b01000000, 0b01000001, 0b00111111, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // J
    {0b01111111, 0b00001000, 0b00010100, 0b00100010, 0b01000001,0b00000000, 0b00000000, 0b00000000},  // K
    {0b01111111, 0b01000000, 0b01000000, 0b01000000, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // L
    {0b01111111, 0b00000010, 0b00001100, 0b00000010, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // M
    {0b01111111, 0b00000100, 0b00001000, 0b00010000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // N
    {0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // O
    {0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000110,0b00000000, 0b00000000, 0b00000000},  // P
    {0b00111110, 0b01000001, 0b01010001, 0b00100001, 0b01011110,0b00000000, 0b00000000, 0b00000000},  // Q
    {0b01111111, 0b00001001, 0b00011001, 0b00101001, 0b01000110,0b00000000, 0b00000000, 0b00000000},  // R
    {0b01000110, 0b01001001, 0b01001001, 0b01001001, 0b00110001,0b00000000, 0b00000000, 0b00000000},  // S
    {0b00000001, 0b00000001, 0b01111111, 0b00000001, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // T
    {0b00111111, 0b01000000, 0b01000000, 0b01000000, 0b00111111,0b00000000, 0b00000000, 0b00000000},  // U
    {0b00011111, 0b00100000, 0b01000000, 0b00100000, 0b00011111,0b00000000, 0b00000000, 0b00000000},  // V
    {0b00111111, 0b01000000, 0b00111000, 0b01000000, 0b00111111,0b00000000, 0b00000000, 0b00000000},  // W
    {0b01100011, 0b00010100, 0b00001000, 0b00010100, 0b01100011,0b00000000, 0b00000000, 0b00000000},  // X
    {0b00000111, 0b00001000, 0b01110000, 0b00001000, 0b00000111,0b00000000, 0b00000000, 0b00000000},  // Y
    {0b01100001, 0b01010001, 0b01001001, 0b01000101, 0b01000011,0b00000000, 0b00000000, 0b00000000},  // Z
    {0b01111111, 0b01000001, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // [
    {0b00010101, 0b00010110, 0b01111100, 0b00010110, 0b00010101,0b00000000, 0b00000000, 0b00000000},  // back slash
    {0b00000000, 0b00000000, 0b00000000, 0b01000001, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // ]
    {0b00000100, 0b00000010, 0b00000001, 0b00000010, 0b00000100,0b00000000, 0b00000000, 0b00000000},  // ^
    {0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // _
    {0b00000000, 0b00000001, 0b00000010, 0b00000100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // `
    {0b00100000, 0b01010100, 0b01010100, 0b01010100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // a
    {0b01111111, 0b01001000, 0b01000100, 0b01000100, 0b00111000,0b00000000, 0b00000000, 0b00000000},  // b
    {0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // c
    {0b00111000, 0b01000100, 0b01000100, 0b01001000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // d
    {0b00111000, 0b01010100, 0b01010100, 0b01010100, 0b00011000,0b00000000, 0b00000000, 0b00000000},  // e
    {0b00001000, 0b01111110, 0b00001001, 0b00000001, 0b00000010,0b00000000, 0b00000000, 0b00000000},  // f
    {0b00001100, 0b01010010, 0b01010010, 0b01010010, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // g
    {0b01111111, 0b00001000, 0b00000100, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // h
    {0b00000000, 0b01000100, 0b01111101, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // i
    {0b00100000, 0b01000000, 0b01000100, 0b00111101, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // j
    {0b01111111, 0b00010000, 0b00101000, 0b01000100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // k
    {0b00000000, 0b01000001, 0b01111111, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // l
    {0b01111100, 0b00000100, 0b00011000, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // m
    {0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // n
    {0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00111000,0b00000000, 0b00000000, 0b00000000},  // o
    {0b01111100, 0b00010100, 0b00010100, 0b00010100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // p
    {0b00001000, 0b00010100, 0b00010100, 0b00011000, 0b01111100,0b00000000, 0b00000000, 0b00000000},  // q
    {0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // r
    {0b01001000, 0b01010100, 0b01010100, 0b01010100, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // s
    {0b00000100, 0b00111111, 0b01000100, 0b01000000, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // t
    {0b00111100, 0b01000000, 0b01000000, 0b00100000, 0b01111100,0b00000000, 0b00000000, 0b00000000},  // u
    {0b00011100, 0b00100000, 0b01000000, 0b00100000, 0b00011100,0b00000000, 0b00000000, 0b00000000},  // v
    {0b00111100, 0b01000000, 0b00111000, 0b01000000, 0b00111100,0b00000000, 0b00000000, 0b00000000},  // w
    {0b01000100, 0b00101000, 0b00010000, 0b00101000, 0b01000100,0b00000000, 0b00000000, 0b00000000},  // x
    {0b00001100, 0b01010000, 0b01010000, 0b01010000, 0b00111100,0b00000000, 0b00000000, 0b00000000},  // y
    {0b01000100, 0b01100100, 0b01010100, 0b01001100, 0b01000100,0b00000000, 0b00000000, 0b00000000},  // z
    {0b00000000, 0b00001000, 0b00110110, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // {
    {0b00000000, 0b00000000, 0b01111111, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // |
    {0b00000000, 0b01000001, 0b00110110, 0b00001000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // }
    {0b00001000, 0b00001000, 0b00101010, 0b00011100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // ~
    {0b00001000, 0b00011100, 0b00101010, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000}   // <-
};

uint32_t edge = 0x0;
uint32_t count = 0x0000;
float period = 0;
int source = 1;
unsigned int Res = 0;
unsigned int Freq = 0;


/*** Call this function to boost the STM32F0xx clock to 48 MHz ***/

void SystemClock48MHz( void )
{

	/* Disable the PLL */
    RCC->CR &= ~(RCC_CR_PLLON);

    /* Wait for the PLL to unlock */
    while (( RCC->CR & RCC_CR_PLLRDY ) != 0 );

    /* Configure the PLL for 48-MHz system clock */
    RCC->CFGR = 0x00280000;

    /* Enable the PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait for the PLL to lock */
    while (( RCC->CR & RCC_CR_PLLRDY ) != RCC_CR_PLLRDY );

    /* Switch the processor to the PLL clock source */
    RCC->CFGR = ( RCC->CFGR & (~RCC_CFGR_SW_Msk)) | RCC_CFGR_SW_PLL;

    /* Update the system with the new clock frequency */
    SystemCoreClockUpdate();

}

/*****************************************************************/


int main()
{

	SystemClock48MHz();


	myGPIOA_Init();		/* Initialize I/O port PA */
	myGPIOB_Init();		/* Initialize I/O port PB */
	myTIM2_Init();		/* Initialize timer TIM2 */
	myTIM3_Init();		/* Initialize timer TIM3 */
	myEXTI_Init();		/* Initialize EXTI */
	myADC_Init();   	/* Initialize ADC */
	myDAC_Init();		/* Initialize DAC */
	mySPI_Init();		/* Initialize SPI */


	while (1)
	{


		if((ADC1->ISR & 0x00000001) != 0)
		{

			ADC1->CR |= ADC_CR_ADSTART;
			while((ADC1->ISR & 0x4) == 0);
			DAC->DHR12R1 = ADC1->DR;

		}

		if((GPIOA->IDR & GPIO_IDR_0) != 0)
		{
			/* Wait for button to be released (PA0 = 0) */
			while((GPIOA->IDR & GPIO_IDR_0) != 0);
			if(source == 1)
				source = 0;
			else
				source = 1;
		}
		refresh_OLED();
	}

	return 0;

}



void myGPIOA_Init()
{
	/* Enable clock for GPIOA peripheral */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	/* Clear bits */
	GPIOA->MODER &= ~((uint32_t)0x0);

	/* Configure PA0 as input */
	GPIOA->MODER &= ~(GPIO_MODER_MODER0);

	/* Ensure no pull-up/pull-down for PA0 */ //user button
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR0);

	/* Configure PA1 as input */
	GPIOA->MODER &= ~(GPIO_MODER_MODER1);

	/* Ensure no pull-up/pull-down for PA1 */ // timer
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR1);

	/* Configure PA2 as input */
	GPIOA->MODER &= ~(GPIO_MODER_MODER2);

	/* Ensure no pull-up/pull-down for PA2 */ // function gen
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR2);

	/* Configure PA5 as analog */
	GPIOA->MODER |= (GPIO_MODER_MODER5);

	/* Ensure no pull-up/pull-down for PA5 */ // pot
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR5);

	/* Configure PA4 as analog */
	GPIOA->MODER |= (GPIO_MODER_MODER4);

	/* Ensure no pull-up/pull-down for PA4 */ // DAC
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR4);

}

void myGPIOB_Init(){

	/* Enable GPIOB clock */
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	/* Configure PB3 as AF */
	GPIOB->MODER |= GPIO_MODER_MODER3_1;

	/* Ensure no pull-up/pull-down for PB3 */
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR3);

	/* Configure PB4 as output */
	GPIOB->MODER |= GPIO_MODER_MODER4_0;

	/* Ensure no pull-up/pull-down for PB4 */
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR4);

	/* Configure PB5 as AF */
	GPIOB->MODER |= GPIO_MODER_MODER5_1;

	/* Ensure no pull-up/pull-down for PB5 */
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR5);

	/* Configure PB6 as output */
	GPIOB->MODER |= GPIO_MODER_MODER6_0;

	/* Ensure no pull-up/pull-down for PB6 */
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR6);

	/* Configure PB7 as output */
	GPIOB->MODER |= GPIO_MODER_MODER7_0;

	/* Ensure no pull-up/pull-down for PB7 */
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR7);

}

void myTIM2_Init()
{
	/* Enable clock for TIM2 peripheral */
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	/* Enable update events */
	TIM2->CR1 &= ~(TIM_CR1_UDIS);

	/* Event on over/underflow */
	TIM2->CR1 |= TIM_CR1_URS;

	/* Counter stops at update events */
	TIM2->CR1 |= TIM_CR1_OPM;

	/* Count up */
	TIM2->CR1 &= ~(TIM_CR1_DIR);

	/* Buffer auto-reload */
	TIM2->CR1 |= TIM_CR1_ARPE;

	/* Set clock prescaler value */
	TIM2->PSC = myTIM2_PRESCALER;
	/* Set auto-reloaded delay */
	TIM2->ARR = myTIM2_PERIOD;

	/* Update timer registers */
	// Relevant register: TIM2->EGR
	TIM3->EGR |= TIM_EGR_UG;

	/* Assign TIM2 interrupt priority = 0 in NVIC */
	NVIC_SetPriority(TIM2_IRQn, 0);

	/* Enable TIM2 interrupts in NVIC */
	NVIC_EnableIRQ(TIM2_IRQn);

	/* Enable update interrupt generation */
	TIM2->DIER |= TIM_DIER_UIE;
}

void myTIM3_Init(){

	/* Enable clock for TIM3 peripheral */
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	/* Enable update events */
	TIM3->CR1 &= ~(TIM_CR1_UDIS);

	/* Event on over/underflow */
	TIM3->CR1 |= TIM_CR1_URS;

	/* Counter stops at update events */
	TIM3->CR1 |= TIM_CR1_OPM;

	/* Count up */
	TIM3->CR1 &= ~(TIM_CR1_DIR);

	/* Buffer auto-reload */
	TIM3->CR1 |= TIM_CR1_ARPE;

	/* Set clock prescaler value */
	TIM3->PSC = myTIM3_PRESCALER;

	/* Set clock reload value */
	TIM3->ARR = myTIM3_PERIOD;

	/* Enable update interrupt generation */
	TIM3->DIER |= TIM_DIER_UIE;

}

void myEXTI_Init()
{
	/* Map EXTI2 line to PA2, EXTI1 line to PA1, EXTI0 line to PA0 */
	SYSCFG->EXTICR[0] = ((uint32_t)0x00000000);


	/* EXTI2 and EXTI1 line interrupts: set rising-edge trigger */
	EXTI->RTSR |= ((uint32_t)0x00000006);

	/* Unmask interrupts from EXTI2, EXTI1 line */
	EXTI->IMR |= ((uint32_t)0x00000006);

	/* Assign EXTI2 interrupt priority = 0 in NVIC */
	NVIC_SetPriority(EXTI2_3_IRQn, 0);

	/* Enable EXTI2 interrupts in NVIC */
	NVIC_EnableIRQ(EXTI2_3_IRQn);

	/* Assign EXTI1 interrupt priority = 0 in NVIC */
	NVIC_SetPriority(EXTI0_1_IRQn, 0);

	/* Enable EXTI1 interrupts in NVIC */
	NVIC_EnableIRQ(EXTI0_1_IRQn);



}

void myADC_Init()
{

	/* Enable ADC clock */
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;


	/* Set data resolution to 12 bits, right align data, overwrite overuns,
	   set continuous conversion mode */
	ADC1->CFGR1 = ((uint32_t)0x00003000);

	/* Clear CR before enabling ADC */
	ADC1->CR = ((uint32_t)0x00000000);

	/* ADC enable */
	ADC1->CR = ((uint32_t)0x00000001);

	/* Set sampling rate to 239 1/2 ADC clock cycles */
	ADC1->SMPR = ((uint32_t)0x00000007);

	/* Select port 5 for conversion */
	ADC1->CHSELR = ((uint32_t)0x00000020);

}

void myDAC_Init()
{

	/* Enable DAC clock */
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;

	/* Enable DAC Channel 1*/
	DAC->CR = DAC_CR_EN1;

}

void mySPI_Init(){

	/* Enable SPI Clock */
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	/* Set struct instance to SPI1 */
    SPI_Handle.Instance = SPI1;

    /* Communication is 1 line bi-directional */
    SPI_Handle.Init.Direction = SPI_DIRECTION_1LINE;

    /* Set to master mode */
    SPI_Handle.Init.Mode = SPI_MODE_MASTER;

    /* SP1 data length is 8 bits */
    SPI_Handle.Init.DataSize = SPI_DATASIZE_8BIT;

    /* Idle clock polarity is 0 */
    SPI_Handle.Init.CLKPolarity = SPI_POLARITY_LOW;

    /* Active clock edge is the first */
    SPI_Handle.Init.CLKPhase = SPI_PHASE_1EDGE;

    /* NSS is forced to 1 */
    SPI_Handle.Init.NSS = SPI_NSS_SOFT;

    /* Set SCK Prescaler */
    SPI_Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;

    /* Data is communicated MSB first */
    SPI_Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;

    /* Set CRC */
    SPI_Handle.Init.CRCPolynomial = 7;

	/* Init SPI */
    HAL_SPI_Init( &SPI_Handle );

    /* Enable SPI */
    __HAL_SPI_ENABLE( &SPI_Handle );

    /* Reset the display */
    GPIOB->ODR &= ~(GPIO_ODR_4);
    delay((uint32_t)0x1); // delay 1ms

    GPIOB->ODR |= GPIO_ODR_4;
    delay((uint32_t)0x1); // delay 1ms

    /* Send initialization commands to LED Display */
	for ( unsigned int i = 0; i < sizeof( oled_init_cmds ); i++ )
	{
		oled_Write_Cmd(oled_init_cmds[i]);
	}

	/* Send zeros to LED Display */
	for( unsigned int j = 0; j < 8; j++ ){
		oled_Write_Cmd(0xB0 + j);
	    oled_Write_Cmd(0x03);
	    oled_Write_Cmd(0x10);
		for ( unsigned int i = 0; i < 128; i++ )
		{
			oled_Write_Data(0x00);
		}
	}
}

void delay(uint32_t cycles){

	/* Set reload value to passed value */
	TIM3->ARR = cycles;

	/* Clear Count */
	TIM3->CNT = ((uint32_t)0x00000000);

	/* Enable update generation */
	TIM3->EGR |= TIM_EGR_UG;

	/* Start timer */
	TIM3->CR1 |= TIM_CR1_CEN;

	/* Wait for interrupt flag */
	while((TIM3->SR & TIM_SR_UIF) == 0);

	/* Reset interrupt flag */
	TIM3->SR &= ~(TIM_SR_UIF);

	/* Stop counter */
	TIM3->CR1 &= ~TIM_CR1_CEN;

	/* Clear Count */
	TIM3->CNT = ((uint32_t)0x00000000);

}

void refresh_OLED(void)
{

    char Buffer[17];

    int size = sizeof(Buffer);

    Res = DAC->DHR12R1;

    /* Send ASCII codes to the LED memory buffer. */
    snprintf(Buffer, sizeof(Buffer), "R: %5u Ohms", Res);

    /* Select page 2 of display */
    oled_Write_Cmd(0xB2);

    /* Select position */
    oled_Write_Cmd(0x03);
    oled_Write_Cmd(0x10);

    for(int j = 0; j<16; j++){
		for(int i = 0; i<8; i++){
			oled_Write_Data(Characters[(int)Buffer[j]][i]);
		}
    }

    for(int i = 0; i<17; i++){
    	Buffer[i]=' ';
    }


    snprintf(Buffer, size, "F: %5u Hz", Freq);

    /* Select page 4 of display */
    oled_Write_Cmd(0xB4);
    oled_Write_Cmd(0x03);
    oled_Write_Cmd(0x10);

    for(int j = 0; j<16; j++){
		for(int i = 0; i<8; i++){
			oled_Write_Data(Characters[(int)Buffer[j]][i]);
		}
    }

    /* Delay 100ms */
    delay((uint32_t)0x64);

}


void oled_Write_Cmd(unsigned char cmd)
{
    // make PB6 = CS# = 1
	GPIOB->ODR |= GPIO_ODR_6;
    // make PB7 = D/C# = 0
	GPIOB->ODR &= ~(GPIO_ODR_7);
    // make PB6 = CS# = 0
	GPIOB->ODR &= ~(GPIO_ODR_6);

    oled_Write(cmd);

    // make PB6 = CS# = 1
	GPIOB->ODR |= GPIO_ODR_6;
}

void oled_Write_Data(unsigned char data)
{
	// make PB6 = CS# = 1

	GPIOB->ODR |= GPIO_ODR_6;

    // make PB7 = D/C# = 1

	GPIOB->ODR |= GPIO_ODR_7;

    // make PB6 = CS# = 0

	GPIOB->ODR &= ~(GPIO_ODR_6);

    oled_Write(data);
    // make PB6 = CS# = 1

    GPIOB->ODR |= GPIO_ODR_6;

}

void oled_Write(unsigned char Value)
{

    /* Wait until SPI1 is ready for writing (TXE = 1 in SPI1_SR) */

	while((SPI1->SR & SPI_SR_TXE) == 0);

    /* Send one 8-bit character:
       - This function also sets BIDIOE = 1 in SPI1_CR1
    */
    HAL_SPI_Transmit(&SPI_Handle, &Value, 1, HAL_MAX_DELAY);

    /* Wait until transmission is complete (TXE = 1 in SPI1_SR) */


    while((SPI1->SR & SPI_SR_TXE) == 0);

}

void TIM2_IRQHandler()
{
	/* Check if update interrupt flag is indeed set */
	if ((TIM2->SR & TIM_SR_UIF) != 0)
	{
		trace_printf("\n*** Overflow! ***\n");

		/* Clear update interrupt flag */
		// Relevant register: TIM2->SR
		NVIC->ICER[0] = ((uint32_t)0x0000000);

		/* Restart stopped timer */
		// Relevant register: TIM2->CR1
		TIM2->CR1 |= TIM_CR1_CEN;

	}
}

void EXTI2_3_IRQHandler()
{

	/* Check if EXTI2 interrupt pending flag is indeed set */
	if (((EXTI->PR & EXTI_PR_PR2) && source) != 0)
	{
		/* First edge */
		if(edge == 0x0){
			edge = 0x1;

		/* Clear count */
		TIM2->CNT = ((uint32_t)0x00000000);

		/* Start timer */
		TIM2->CR1 |= TIM_CR1_CEN;

		/* Second edge */
		} else if (edge == 0x1) {

		/* Stop timer */
		TIM2->CR1 &= ~TIM_CR1_CEN;

		/* Read count */
		count = TIM2->CNT;

		/* Reset edge counter */
		edge = 0;

		/* Calculate period and frequency */
		period = count/48000000.;
		/* Set global variable */
		Freq = 1./period;

		/* Clear count */
		TIM2->CNT = ((uint32_t)0x00000000);

		/* Restart timer */
		TIM2->CR1 |= TIM_CR1_CEN;
		}
		/* No matter the source of the interrupt, reset the timer */
		TIM2->CR1 |= TIM_CR1_CEN;
	}

	/* Clear EXTI2 flag */
	EXTI->PR = EXTI_PR_PR2;

}

void EXTI0_1_IRQHandler()
{
	/* Check if EXTI1 interrupt pending flag is indeed set */
	if (((EXTI->PR & EXTI_PR_PR1) && !source) != 0){

			/* First edge */
			if(edge == 0x0){
				edge = 0x1;

			/* Clear count */
			TIM2->CNT = ((uint32_t)0x00000000);

			/* Start timer */
			TIM2->CR1 |= TIM_CR1_CEN;

			/* Second edge */
			} else if (edge == 0x1) {

			/* Stop timer */
			TIM2->CR1 &= ~TIM_CR1_CEN;

			/* Reset count */
			count = TIM2->CNT;

			/* Reset edge counter */
			edge = 0;

			/* Calculate frequency */
			period = count/48000000.;

			/* Set global variable */
			Freq = 1./period;

			/* Clear count */
			TIM2->CNT = ((uint32_t)0x00000000);

			/* Restart timer */
			TIM2->CR1 |= TIM_CR1_CEN;
		}
		/* No matter the source of the interrupt, reset the timer */
		TIM2->CR1 |= TIM_CR1_CEN;
	}

	/* Clear EXTI1 flag */
	EXTI->PR = EXTI_PR_PR1;

}
---------------------------------------------------------------------------
