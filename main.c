#include <xc.h>
#include "LCD.h"
#include "APDS9960.h"
#include "platform.h"
#include <stdio.h>

#pragma config FOSC=HSPLL
#pragma config WDTEN=OFF
#pragma config XINST=OFF

void InitPins(void);
void ConfigInterrupts(void);

RGBCdata rgbcData;
unsigned char proxData;
ProximityConfig proxConfig;
ALSConfig alsConfig;
GestureConfig gestureConfig;
volatile char update = 0;
GestureData gestureData[64];
volatile char currentPos = 0;
volatile char currentCount = 0;

void main(void) {
    OSCTUNEbits.PLLEN = 1;
    InitPins();
    LCDInit();
    lprintf(0, "APDS9960 Demo");
    
    //Configure the USART for 115200 baud asynchronous transmission
    TRISCbits.TRISC7 = 1;
    TRISCbits.TRISC6 = 0;
    SPBRG1 = 68; //115200 baud
    SPBRGH1 = 0;
    TXSTA1bits.BRGH = 1;
    BAUDCON1bits.BRG16 = 1;
    TXSTA1bits.SYNC = 0;
    RCSTA1bits.SPEN = 1;
    TXSTA1bits.TXEN = 1;
    printf("APDS9960 Demo\r\n");
    
    InitAPDS9960();
    char id = i2cReadRegister(APDS_ID);
    lprintf(1, "Chip ID = %#02x", id);
    __delay_ms(2000);
    ConfigInterrupts();
    
    //Configure proximity engine
    APDS9960ReadProximityConfig(&proxConfig); //get defaults
    proxConfig.gain = PGAIN_8X;
    proxConfig.pulses = 8;
    proxConfig.highThreshold = 100;
    proxConfig.lowThreshold = 0;
    proxConfig.persistence = 2;
    APDS9960SetProximityConfig(&proxConfig);
    
    //Configure gesture engine
    APDS9960ReadGestureConfig(&gestureConfig);
    gestureConfig.enterThreshold = 100;
    gestureConfig.exitThreshold = 75;
    gestureConfig.exitPersistence = GPERS_2;
    gestureConfig.exitMask = GMASK_ALL;
    gestureConfig.fifoThreshold = FIFO_OVL_16;
    gestureConfig.gain = GGAIN_4X;
    gestureConfig.pulses = 2;
    gestureConfig.pulseLength = PULSE_8US;
    gestureConfig.waitTime = GWTIME_30_8MS;
    APDS9960SetGestureConfig(&gestureConfig);
    
    APDS9960Start(PROXIMITY_ENABLE, PROXIMITY_INTERRUPT, 10, 0, 0);
    //APDS9960Start(PROXIMITY_ENABLE | GESTURE_ENABLE, GESTURE_INTERRUPT, 0, 0, 0);
    
    printf("U\tD\tL\tR\r\n");
    while (1) {
        if (update) {
            update = 0;
            int len  = currentPos + currentCount;
            for (int i = currentPos; i < len; ++i) {
                printf("%d\t%d\t%d\t%d\r\n", gestureData[i].up, 
                        gestureData[i].down, gestureData[i].left, 
                        gestureData[i].right);
            }
        }
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
        if (proxConfig.highThreshold == 100) {
            proxConfig.highThreshold = 255;
            proxConfig.lowThreshold = 75;
        } else {
            proxConfig.highThreshold = 100;
            proxConfig.lowThreshold = 0;
        }
        LATDbits.LATD1 ^= 1;
        APDS9960SetProximityConfig(&proxConfig);    
        APDS9960ClearAllInterrupts();
        
        //Get gesture data
        currentCount = APDS9960ReadGestureFIFO(&gestureData[currentPos], 32);
        if (currentCount > 0) {
            update = 1;
            if (currentPos == 0) {
                currentPos = 32;
            } else {
                currentPos = 0;
            }
        }
        INTCON3bits.INT1IF = 0; //must clear the flag to avoid recursive interrupts
    }
}

void putch(char c) {
    while (TX1IF == 0);
    TXREG1 = c;
}