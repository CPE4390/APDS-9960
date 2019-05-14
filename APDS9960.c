#include <xc.h>
#include "APDS9960.h"
#include "platform.h"

void InitAPDS9960(void) {
    initPlatform();
    __delay_ms(6);
    i2cWriteRegister(APDS_ENABLE, 0); //All off
    APDS9960ClearGestureFIFO(); //make sure fifo is clear
}

void APDS9960Start(unsigned char enable, unsigned char interrupts,
        unsigned int wait, char wlong, char sleepAfterInt) {
    unsigned char value;
    enable &= 0b01000110;
    enable |= POWER_ON;
    if (wait != 0) {
        enable |= WAIT_ENABLE;
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
    enable |= (interrupts & 0b00110000); //ALS and Prox interrupts
    value = i2cReadRegister(APDS_GCONF4);
    if (interrupts & GESTURE_INTERRUPT) {
        value |= 0b00000010;
    } else {
        value &= 0b11111101;
    }
    i2cWriteRegister(APDS_GCONF4, value);
    value = i2cReadRegister(APDS_CONFIG2);
    if (interrupts & CPSAT_INTERRUPT) {
        value |= 0b01000000;
    } else {
        value &= 0b10111111;
    }
    if (interrupts & PGSAT_INTERRUPT) {
        value |= 0b10000000;
    } else {
        value &= 0b01111111;
    }
    i2cWriteRegister(APDS_CONFIG2, value);
    i2cWriteRegister(APDS_ENABLE, enable);
}

unsigned char APDS9960GetStatus(void) {
    return i2cReadRegister(APDS_STATUS);
}

void APDS9960ClearAllInterrupts(void) {
    i2cClearInterrupt(APDS_AICLEAR);
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

void APDS9960ReadALSConfig(ALSConfig *config) {
    unsigned char regValue;
    regValue = i2cReadRegister(APDS_ATIME);
    if (regValue == 0) {
        config->cycles = 256;
    } else {
        config->cycles = (unsigned int) (-regValue);
    }
    i2cReadData(APDS_AILTL, (unsigned char *) &(config->lowThreshold), sizeof (config->lowThreshold));
    i2cReadData(APDS_AIHTL, (unsigned char *) &(config->highThreshold), sizeof (config->highThreshold));
    regValue = i2cReadRegister(APDS_PERS);
    config->persistence = regValue & 0b00001111;
    regValue = i2cReadRegister(APDS_CONTROL);
    config->gain = regValue & 0b00000011;
}

void APDS9960SetALSConfig(const ALSConfig *config) {
    unsigned char regValue;
    if (config->cycles >= 256) {
        regValue = 0;
    } else {
        regValue = -((unsigned char) config->cycles);
    }
    i2cWriteRegister(APDS_ATIME, regValue);
    i2cWriteRegisterWord(APDS_AILTL, config->lowThreshold);
    i2cWriteRegisterWord(APDS_AIHTL, config->highThreshold);
    regValue = i2cReadRegister(APDS_PERS);
    regValue &= 0b11110000;
    regValue |= config->persistence;
    i2cWriteRegister(APDS_PERS, regValue);
    regValue = i2cReadRegister(APDS_CONTROL);
    regValue &= 0b11111100;
    regValue |= config->gain;
    i2cWriteRegister(APDS_CONTROL, regValue);
}

unsigned int APDS9960ALSMaxCount(const ALSConfig *config) {
    long maxCount;
    maxCount = config->cycles * 1025;
    if (maxCount > 65535) {
        maxCount = 65535;
    }
    return (unsigned int) maxCount;
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

void APDS9960ReadProximityConfig(ProximityConfig *config) {
    unsigned char regValue;
    config->lowThreshold = i2cReadRegister(APDS_PILT);
    config->highThreshold = i2cReadRegister(APDS_PIHT);
    regValue = i2cReadRegister(APDS_PERS);
    config->persistence = regValue >> 4;
    regValue = i2cReadRegister(APDS_PPULSE);
    config->pulseLength = regValue >> 6;
    config->pulses = (regValue & 0b00111111) + 1;
    regValue = i2cReadRegister(APDS_CONTROL);
    regValue &= 0b00001100;
    config->gain = regValue >> 2;
    config->urOffset = i2cReadRegister(APDS_POFFSET_UR);
    config->dlOffset = i2cReadRegister(APDS_POFFSET_DL);
    regValue = i2cReadRegister(APDS_CONFIG3);
    config->pcmp = (regValue & 0b00100000) >> 5;
    config->mask_u = (regValue & 0b00001000) >> 3;
    config->mask_d = (regValue & 0b00000100) >> 2;
    config->mask_l = (regValue & 0b00000010) >> 1;
    config->mask_r = (regValue & 0b00000001);
}

void APDS9960SetProximityConfig(const ProximityConfig *config) {
    unsigned char regValue;
    i2cWriteRegister(APDS_PILT, config->lowThreshold);
    i2cWriteRegister(APDS_PIHT, config->highThreshold);
    regValue = i2cReadRegister(APDS_PERS);
    regValue &= 0b00001111;
    regValue |= (config->persistence << 4);
    i2cWriteRegister(APDS_PERS, regValue);
    regValue = config->pulseLength;
    regValue <<= 6;
    if (config->pulses > 0) {
        regValue |= ((config->pulses - 1) & 0b00111111);
    }
    i2cWriteRegister(APDS_PPULSE, regValue);
    regValue = i2cReadRegister(APDS_CONTROL);
    regValue &= 0b11110011;
    regValue |= (config->gain << 2);
    i2cWriteRegister(APDS_CONTROL, regValue);
    i2cWriteRegister(APDS_POFFSET_UR, config->urOffset);
    i2cWriteRegister(APDS_POFFSET_DL, config->dlOffset);
    regValue = i2cReadRegister(APDS_CONFIG3);
    regValue &= 0b00010000;
    regValue |= config->pcmp << 5;
    regValue |= config->mask_u << 3;
    regValue |= config->mask_d << 2;
    regValue |= config->mask_l << 1;
    regValue |= config->mask_r;
    i2cWriteRegister(APDS_CONFIG3, regValue);
}

void APDS9960ReadGestureConfig(GestureConfig *config) {
    unsigned char regValue;

    config->enterThreshold = i2cReadRegister(APDS_GPENTH);
    config->exitThreshold = i2cReadRegister(APDS_GEXTH);
    regValue = i2cReadRegister(APDS_GCONF1);
    config->fifoThreshold = regValue >> 6;
    config->exitMask = (regValue & 0b00111100) >> 2;
    config->exitPersistence = regValue & 0b00000011;
    regValue = i2cReadRegister(APDS_GCONF2);
    config->gain = regValue >> 5;
    config->ledDriveStrength = (regValue & 0b00011000) >> 3;
    config->waitTime = regValue & 0b00000111;
    config->offset_u = i2cReadRegister(APDS_GOFFSET_U);
    config->offset_d = i2cReadRegister(APDS_GOFFSET_D);
    config->offset_l = i2cReadRegister(APDS_GOFFSET_L);
    config->offset_r = i2cReadRegister(APDS_GOFFSET_R);
    regValue = i2cReadRegister(APDS_GPULSE);
    config->pulseLength = regValue >> 6;
    config->pulses = (regValue & 0b00111111) + 1;
    config->dimensionSelect = i2cReadRegister(APDS_GCONF3);
}

void APDS9960SetGestureConfig(const GestureConfig *config) {
    unsigned char regValue;
    i2cWriteRegister(APDS_GPENTH, config->enterThreshold & 0b11101111); //bit 4 must be 0 per datasheet
    i2cWriteRegister(APDS_GEXTH, config->exitThreshold);
    regValue = config->fifoThreshold << 6;
    regValue |= config->exitMask << 2;
    regValue |= config->exitPersistence;
    i2cWriteRegister(APDS_GCONF1, regValue);
    regValue = config->gain << 5;
    regValue |= config->ledDriveStrength << 3;
    regValue |= config->waitTime;
    i2cWriteRegister(APDS_GCONF2, regValue);
    i2cWriteRegister(APDS_GOFFSET_U, config->offset_u);
    i2cWriteRegister(APDS_GOFFSET_D, config->offset_d);
    i2cWriteRegister(APDS_GOFFSET_L, config->offset_l);
    i2cWriteRegister(APDS_GOFFSET_R, config->offset_r);
    regValue = config->pulseLength << 6;
    if (config->pulses > 0) {
        regValue |= (config->pulses - 1) & 0b00111111;
    }
    i2cWriteRegister(APDS_GPULSE, regValue);
    i2cWriteRegister(APDS_GCONF3, config->dimensionSelect);
}

unsigned char APDS9960GetGestureStatus(void) {
    return i2cReadRegister(APDS_GSTATUS);
}

void APDS9960SetGestureMode(unsigned char mode) {
    unsigned char regValue;
    regValue = i2cReadRegister(APDS_GCONF4);
    if (mode) {
        regValue |= 0b00000001;
    } else {
        regValue &= 0b11111110;
    }
    i2cWriteRegister(APDS_GCONF4, regValue);
}

unsigned char APDS9960GestureActive(void) {
    unsigned char mode;
    mode = i2cReadRegister(APDS_GCONF4);
    return mode & 0b00000001;
}

void APDS9960ClearGestureFIFO(void) {
    unsigned char regValue;
    regValue = i2cReadRegister(APDS_GCONF4);
    regValue |= 0b00000100;
    i2cWriteRegister(APDS_GCONF4, regValue);
}

unsigned char APDS9960ReadGestureFIFO(GestureData *data, unsigned char max) {
    unsigned char available;
    available = i2cReadRegister(APDS_GFLVL);
    if (available == 0) {
        return 0;
    }
    if (available > max) {
        available = max;
    }
    i2cReadData(APDS_GFIFO_U, (unsigned char *)data, available * 4);
    return available;
}

unsigned char APDS9960ProcessGesture(GestureData *data, int len, char done) {
    
    return GESTURE_UNKNOWN;
}