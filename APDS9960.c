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
    i2cReadData(APDS_AILTL, (unsigned char *)&(config->lowThreshold), sizeof(config->lowThreshold));
    i2cReadData(APDS_AIHTL, (unsigned char *)&(config->highThreshold), sizeof(config->highThreshold));
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