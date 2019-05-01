/* 
 * File:   APDS9960.h
 * Author: bmcgarvey
 *
 * Created on April 30, 2019, 8:37 AM
 */

#ifndef APDS9960_H
#define	APDS9960_H

#define I2C_ADDRESS     0x39

//Register defines
#define APDS_ENABLE         0x80
#define APDS_ATIME          0x81
#define APDS_WTIME          0x83
#define APDS_AILTL          0x84
#define APDS_AILTH          0x85
#define APDS_AIHTL          0x86
#define APDS_AIHTH          0x87
#define APDS_PILT           0x89
#define APDS_PIHT           0x8b
#define APDS_PERS           0x8c
#define APDS_CONFIG1        0x8d
#define APDS_PPULSE         0x8e
#define APDS_CONTROL        0x8f
#define APDS_CONFIG2        0x90
#define APDS_ID             0x92
#define APDS_STATUS         0x93
#define APDS_CDATAL         0x94
#define APDS_CDATAH         0x95
#define APDS_RDATAL         0x96
#define APDS_RDATAH         0x97
#define APDS_GDATAL         0x98
#define APDS_GDATAH         0x99
#define APDS_BDATAL         0x9a
#define APDS_BDATAH         0x9b
#define APDS_PDATA          0x9c
#define APDS_POFFSET_UR     0x9d
#define APDS_POFFSET_DL     0x9e
#define APDS_CONFIG3        0x9f
#define APDS_GPENTH         0xa0
#define APDS_GEXTH          0xa1
#define APDS_GCONF1         0xa2
#define APDS_GCONF2         0xa3
#define APDS_GOFFSET_U      0xa4
#define APDS_GOFFSET_D      0xa5
#define APDS_GOFFSET_L      0xa7
#define APDS_GOFFSET_R      0xa9
#define APDS_GPULSE         0xa6
#define APDS_GCONF3         0xaa
#define APDS_GCONF4         0xab
#define APDS_GFLVL          0xae
#define APDS_GSTATUS        0xaf
#define APDS_IFORCE         0xe4
#define APDS_PICLEAR        0xe5
#define APDS_CICLEAR        0xe6
#define APDS_AICLEAR        0xe7
#define APDS_GFIFO_U        0xfc
#define APDS_GFIFO_D        0xfd
#define APDS_GFIFO_L        0xfe
#define APDS_GFIFO_R        0xff

#ifdef	__cplusplus
extern "C" {
#endif

    void InitAPDS9960(void);
    void I2CWriteRegister(unsigned char reg, unsigned char byte);
    void I2CWriteRegisterWord(unsigned char reg, unsigned int word);
    void I2CReadData(unsigned char reg, unsigned char *buffer, char count);
    unsigned char I2CReadRegister(unsigned char reg);
    void ClearInterrupt(unsigned char reg);

#ifdef	__cplusplus
}
#endif

#endif	/* APDS9960_H */

