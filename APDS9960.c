#include <xc.h>
#include "APDS9960.h"
#include "platform.h"

void InitAPDS9960(void) {
    initPlatform();
    __delay_ms(6);
    i2cWriteRegister(APDS_ENABLE, POWER_ON);
}

void APDS9960Start(unsigned char flags, unsigned int wait, char wlong, char sleepAfterInt) {
    unsigned char value = 0;
    flags &= 0b01110110;
    flags |= POWER_ON;
    if (wait != 0) {
        flags |= WAIT_ENABLE;
        if (wait >= 256) {
            value = 0;
        } else {
            value = -((unsigned char) wait);
        }
        if (wlong) {
            i2cWriteRegister(APDS_CONFIG1, WLONG | 0x60);
        } else {
            i2cWriteRegister(APDS_CONFIG1, 0x60); //Default value per datasheet
        }
        i2cWriteRegister(APDS_WTIME, value);
    }
    value = i2cReadRegister(APDS_CONFIG3);
    if (sleepAfterInt) {
        value |= SLEEP_AFTER_INT;
    } else {
        value &= ~SLEEP_AFTER_INT;
    }
    i2cWriteRegister(APDS_CONFIG3, value);
    i2cWriteRegister(APDS_ENABLE, flags);
}

unsigned char APDS9960GetStatus(void) {
    return i2cReadRegister(APDS_STATUS);
}

void APDS9960ClearAllInterrupts(void) {
    i2cClearInterrupt(APDS_AICLEAR);
}

void APDS9960ClearGestureInterrupt(void) {

}

void APDS9960ClearALSInterrupt(void) {
    i2cClearInterrupt(APDS_CICLEAR);
}

void APDS9960ClearProximityInterrupt(void) {
    i2cClearInterrupt(APDS_PICLEAR);
}

void APDS9960GetALSData(RGBCdata *data) {
    i2cReadData(APDS_CDATAL, (unsigned char *) data, sizeof (RGBCdata));
}

unsigned char APDS9960GetProximityData(void) {
    return i2cReadRegister(APDS_PDATA);
}

void APDS9960SetProximityGain(unsigned char pgain) {
    unsigned char value;
    value = i2cReadRegister(APDS_CONTROL);
    value &= 0b11110011;
    pgain &= 0b00000011;
    value |= (pgain << 2);
    i2cWriteRegister(APDS_CONTROL, value);
}
unsigned char APDS9960GetProximityGain(void) {
    unsigned char value;
    value = i2cReadRegister(APDS_CONTROL);
    value &= 0b00001100;
    value >>= 2;
    return value;
}

void APDS9960SetALSGain(unsigned char again) {
    unsigned char value;
    value = i2cReadRegister(APDS_CONTROL);
    value &= 0b11111100;
    again &= 0b00000011;
    value |= again;
    i2cWriteRegister(APDS_CONTROL, value);
}
unsigned char APDS9960GetALSGain(void) {
    unsigned char value;
    value = i2cReadRegister(APDS_CONTROL);
    value &= 0b00000011;
    return value;
}

void APDS9960SetLEDDriveCurrent(unsigned char ldrive, unsigned char boost) {
    unsigned char value;
    value = i2cReadRegister(APDS_CONTROL);
    value &= 0b00111111;
    ldrive &= 0b00000011;
    value |= (ldrive << 6);
    i2cWriteRegister(APDS_CONTROL, value);
    value = i2cReadRegister(APDS_CONFIG2);
    value &= 0b11001111;
    boost &= 0b00000011;
    value |= (boost << 4);
    i2cWriteRegister(APDS_CONFIG2, value);
}

void APDS9960GetLEDDriveCurrent(unsigned char *ldrive, unsigned char *boost) {
    unsigned char value;
    value = i2cReadRegister(APDS_CONTROL);
    *ldrive = value >> 6;
    value = i2cReadRegister(APDS_CONFIG2);
    *boost = (value & 0b00110000) >> 4;
}

void APDS9960SetProximityPulseValues(unsigned char pplen, unsigned char pulses) {
    unsigned char value;
    value = pplen << 6;
    if (pulses > 64) {
        pulses = 64;
    }
    value |=  pulses - 1;
    i2cWriteRegister(APDS_PPULSE, value);
}

void APDS9960GetProximityPulseValues(unsigned char *pplen, unsigned char *pulses) {
    unsigned char value;
    value = i2cReadRegister(APDS_PPULSE);
    *pplen = value >> 6;
    *pulses = (value & 0b00111111) + 1;
}

void APDS9960SetALSIntegrationTime(unsigned int cycles) {
    unsigned char atime;
    if (cycles >= 256) {
        atime = 0;
    } else {
        atime = -((unsigned char) cycles);
    }
    i2cWriteRegister(APDS_ATIME, atime);
}

unsigned int APDS9960GetALSMaxCount(void) {
    unsigned char atime;
    long cycles;
    atime = i2cReadRegister(APDS_ATIME);
    if (atime == 0) {
        cycles = 256;
    } else {
        cycles = (unsigned char)(-atime);
    }
    cycles *= 1025;
    if (cycles > 65535) {
        cycles = 65535;
    }
    return (unsigned int)cycles;
}