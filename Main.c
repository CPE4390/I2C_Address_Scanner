/* 
 * File:   Main.c
 * Author: Brad
 *
 * Created on April 19, 2014, 5:31 PM
 */

#include <xc.h>


#pragma config FOSC=HSPLL
#pragma config WDTEN=OFF
#pragma config XINST=OFF

#include "LCD.h"
#include <stdio.h>

#define _XTAL_FREQ  32000000L

/*
Connections:
        Master RD5 <-> SDA
        Master RD6 <-> SCL
 */
//char str[17];

void main(void) {
    unsigned char address;
    long i;
    OSCTUNEbits.PLLEN = 1;
    LCDInit();
    LATD = 0;
    TRISD = 0b01100000; //MMSP2 uses RD5 as SDA, RD6 as SCL, both set as inputs
    LATB = 0;
    TRISB = 0b00000001;
    SSP2ADD = 0x63; //100kHz
    SSP2STATbits.SMP = 1;
    SSP2CON1bits.SSPM = 0b1000; //I2C Master mode
    SSP2CON1bits.SSPEN = 1; //Enable MSSP
    for (address = 0; address < 128; ++address) {
        lprintf(0, "Testing %02X", address);
        SSP2CON2bits.SEN = 1;
        while (SSP2CON2bits.SEN == 1);
        SSP2BUF = address << 1; //address with R/W clear for write
        while (SSP2STATbits.BF || SSP2STATbits.R_W);
        if (SSP2CON2bits.ACKSTAT == 0) //got ACK
        {
            lprintf(1, "Found %02X:%02X", address, address << 1);
            while (PORTBbits.RB0 == 1);
            __delay_ms(10);
            while (PORTBbits.RB0 == 0);
        }
        SSP2CON2bits.PEN = 1;
        while (SSP2CON2bits.PEN == 1);
        __delay_ms(50);
    }
    while (1);
}

