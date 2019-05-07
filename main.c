#include <xc.h>
#include "LCD.h"
#include "APDS9960.h"
#include "platform.h"

#pragma config FOSC=HSPLL
#pragma config WDTEN=OFF
#pragma config XINST=OFF

void InitPins(void);
void ConfigInterrupts(void);

RGBCdata rgbcData;
unsigned char proxData;

void main(void) {
    OSCTUNEbits.PLLEN = 1;
    InitPins();
    LCDInit();
    lprintf(0, "APDS9960 Demo");
    InitAPDS9960();
    char id = i2cReadRegister(APDS_ID);
    lprintf(1, "Chip ID = %#02x", id);
    __delay_ms(2000);
    ConfigInterrupts();
    APDS9960SetProximityGain(PGAIN_4X);
    APDS9960SetALSGain(AGAIN_4X);
    APDS9960SetALSIntegrationTime(1);
    APDS9960Start(ALS_ENABLE | PROXIMITY_ENABLE | PROXIMITY_INTERRUPT, 100, 1, 0);
    while (1) {
        char status = APDS9960GetStatus();
        if (status & AVALID) {
            APDS9960GetALSData(&rgbcData);
        }
        if (status & PVALID) {
            proxData = APDS9960GetProximityData();
        }
        lprintf(0, "S%02x C%u R%u", status, rgbcData.cdata, rgbcData.rdata);
        lprintf(1, "G%u B%u P%u", rgbcData.gdata, rgbcData.bdata, proxData);
        __delay_ms(500);
    }
}

void InitPins(void) {
    LATD = 0; //Turn off all LED's
    TRISD = 0; //LED's are outputs
    //Set TRIS bits for any required peripherals here.
    TRISB = 0b00000011; //Button0 is input; RB1 is INT1
    INTCON2bits.RBPU = 0; //enable weak pullups on port B
}

void ConfigInterrupts(void) {
    RCONbits.IPEN = 0; //no priorities.
    //set up INT0 to interrupt on falling edge
    INTCON2bits.INTEDG0 = 0; //interrupt on falling edge
    INTCONbits.INT0IE = 1; //Enable the interrupt
    INTCONbits.INT0IF = 0; //Always clear the flag before enabling interrupts
    INTCON2bits.INTEDG1 = 0;
    INTCON3bits.INT1E = 1;
    INTCON3bits.INT1IF = 0;
    INTCONbits.GIE = 1; //Turn on interrupts
}

void __interrupt(high_priority) HighIsr(void) {
    //Check the source of the interrupt
    if (INTCONbits.INT0IF == 1) {
        
        INTCONbits.INT0IF = 0; //must clear the flag to avoid recursive interrupts
    }
    if (INTCON3bits.INT1IF == 1) {
        LATDbits.LATD1 ^= 1;
        APDS9960ClearAllInterrupts();
        INTCON3bits.INT1IF = 0; //must clear the flag to avoid recursive interrupts
    }
}
