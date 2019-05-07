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

//Function flags
#define PROXIMITY_ENABLE    0x04
#define GESTURE_ENABLE      0x40
#define ALS_ENABLE          0x02

//Interrupt flags/enable
#define PROXIMITY_INTERRUPT  0x20
#define ALS_INTERRUPT       0x10
#define GESTURE_INTERRUPT   0x04

//Config bits
#define POWER_ON            0x01
#define WAIT_ENABLE         0x08
#define WLONG               0x02
#define SLEEP_AFTER_INT     0x10

//Status bits
#define CPSAT               0x80
#define PGSAT               0x40
#define PINT                0x20
#define AINT                0x10
#define GINT                0x04
#define PVALID              0x02
#define AVALID              0x01

//Settings
#define PULSE_4US           0
#define PULSE_8US           1
#define PULSE_16US          2
#define PULSE_32US          3


#define LED_100MA           0
#define LED_50MA            1
#define LED_25MA            2
#define LED_12_5MA          3

#define PGAIN_1X            0
#define PGAIN_2X            1
#define PGAIN_4X            2
#define PGAIN_8X            3

#define AGAIN_1X            0
#define AGAIN_4X            1
#define AGAIN_16X           2
#define AGAIN_64X           3

#define LED_BOOST_100       0
#define LED_BOOST_150       1
#define LED_BOOST_200       2
#define LED_BOOST_300       3


#ifdef	__cplusplus
extern "C" {
#endif
    typedef struct {
        unsigned int cdata;
        unsigned int rdata;
        unsigned int gdata;
        unsigned int bdata;
    } RGBCdata;
    
    typedef struct {
        unsigned int cycles;
        unsigned int lowThreshold;
        unsigned int highThreshold;
        unsigned int persistence : 4;
        unsigned int gain : 2;
        unsigned int reserved : 2;
    } ALSConfig;
    
    typedef struct {
        unsigned char highThreshold;
        unsigned char lowThreshold;
        char urOffset;
        char dlOffset;
        unsigned char pulses;
        unsigned int persistence : 4;
        unsigned int pulseLength : 2;
        unsigned int gain : 2;
        unsigned int pcmp : 1;
        unsigned int mask_u : 1;
        unsigned int mask_d : 1;
        unsigned int mask_l : 1;
        unsigned int mask_r : 1;
        unsigned int reserved : 3;
    } ProximityConfig;
    
    void InitAPDS9960(void);
    void APDS9960Start(unsigned char flags, unsigned int wait, char wlong, char sleepAfterInt);
    unsigned char APDS9960GetStatus(void);
    
    //Interrupt clear functions
    void APDS9960ClearAllInterrupts(void);
    void APDS9960ClearALSInterrupt(void);
    void APDS9960ClearProximityInterrupt(void);
    
    //ALS functions
    void APDS9960GetALSData(RGBCdata *data);
    void APDS9960ReadALSConfig(ALSConfig *config);
    void APDS9960SetALSConfig(const ALSConfig *config);
    unsigned int APDS9960ALSMaxCount(const ALSConfig *config);
    
    //Proximity functions
    unsigned char APDS9960GetProximityData(void);
    void APDS9960ReadProximityConfig(ProximityConfig *config);
    void APDS9960SetProximityConfig(const ProximityConfig *config);
    
    //LED current control
    void APDS9960SetLEDDriveCurrent(unsigned char ldrive, unsigned char boost);
    void APDS9960GetLEDDriveCurrent(unsigned char *ldrive, unsigned char *boost);
    
#ifdef	__cplusplus
}
#endif

#endif	/* APDS9960_H */

