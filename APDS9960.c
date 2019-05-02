#include <xc.h>
#include "APDS9960.h"
#include "platform.h"

void InitAPDS9960(void) {
    initPlatform();
    __delay_ms(6);
    i2cWriteRegister(APDS_ENABLE, POWER_ON);
}

void APDS9960Start(unsigned char flags, unsigned int wait, char wlong, char sleepAfterInt) {
    unsigned char byte = 0;
    functions &= 0b01110110;
    functions |= POWER_ON;
    if (wait != 0) {
        functions |= WAIT_ENABLE;
        byte = -(unsigned char) wait;
        if (wlong) {
            i2cWriteRegister(APDS_CONFIG1, WLONG | 0x60);
        } else {
            i2cWriteRegister(APDS_CONFIG1, 0x60); //Default value per datasheet
        }
        i2cWriteRegister(APDS_WTIME, byte);
    }
    byte = i2cReadRegister(APDS_CONFIG3);
    if (sleepAfterInt) {
        byte |= SLEEP_AFTER_INT;
    } else {
        byte &= ~SLEEP_AFTER_INT;
    }
    i2cWriteRegister(APDS_CONFIG3, byte);
    i2cWriteRegister(APDS_ENABLE, functions);
}

unsigned char APDS9960GetStatus(void) {
    return i2cReadRegister(APDS_STATUS);
}

void APDS9960ClearAllInterrupts(void) {
    i2cClearInterrupt(APDS_AICLEAR);
}

void APDS996ClearGestureInterrupt(void) {
    
}

void APDS996ClearALSInterrupt(void) {
    i2cClearInterrupt(APDS_CICLEAR);
}

void APDS996ClearProximityInterrupt(void) {
    i2cClearInterrupt(APDS_PICLEAR);
}

char APDS9960GetALSData(RGBCdata *data) {
    unsigned char status = 0;
    status = APDS9960GetStatus();
    if (status & AVALID) {
        i2cReadData(APDS_CDATAL, (unsigned char *) data, sizeof (RGBCdata));
        return 1;
    } else {
        return 0;
    }
}
