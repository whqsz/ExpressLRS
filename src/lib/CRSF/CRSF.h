#ifndef H_CRSF
#define H_CRSF

#include <Arduino.h>
#include "crsf_protocol.h"
#include "HardwareSerial.h"
#include "msp.h"
#include "msptypes.h"
#include "../../src/targets.h"
#include "../../src/LowPassFilter.h"
#include "../CRC/crc.h"

#ifdef PLATFORM_ESP32
#include "esp32-hal-uart.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#endif


class CRSF
{

public:
    #if CRSF_RX_MODULE

    CRSF(Stream *dev) : _dev(dev)
    {
    }

    CRSF(Stream &dev) : _dev(&dev) {}

    #endif

    static HardwareSerial Port;

    static volatile uint16_t ChannelDataIn[16];
    static volatile uint16_t ChannelDataInPrev[16]; // Contains the previous RC channel data RX side only
    static volatile uint16_t ChannelDataOut[16];

    // current and sent switch values
    #define N_SWITCHES 8

    static uint8_t currentSwitches[N_SWITCHES];
    static uint8_t sentSwitches[N_SWITCHES];
    // which switch should be sent in the next rc packet
    static uint8_t nextSwitchIndex;

    static void (*RCdataCallback1)(); //function pointer for new RC data callback
    static void (*RCdataCallback2)(); //function pointer for new RC data callback

    static void (*disconnected)();
    static void (*connected)();

    static void (*RecvParameterUpdate)();

    static volatile uint8_t ParameterUpdateData[2];

    /////Variables/////

    static volatile crsf_channels_s PackedRCdataOut;            // RC data in packed format for output.
    static volatile crsfPayloadLinkstatistics_s LinkStatistics; // Link Statisitics Stored as Struct
    static volatile crsf_sensor_battery_s TLMbattSensor;

    /// UART Handling ///
    static uint32_t GoodPktsCountResult; // need to latch the results
    static uint32_t BadPktsCountResult; // need to latch the results

    static void Begin(); //setup timers etc
    static void End(); //stop timers etc

    void ICACHE_RAM_ATTR sendRCFrameToFC();
    void ICACHE_RAM_ATTR sendMSPFrameToFC(mspPacket_t* packet);
    void sendLinkStatisticsToFC();
    void ICACHE_RAM_ATTR sendLinkStatisticsToTX();
    void ICACHE_RAM_ATTR sendTelemetryToTX(uint8_t *data);

    void sendLUAresponse(uint8_t val[], uint8_t len);

    static void ICACHE_RAM_ATTR sendSetVTXchannel(uint8_t band, uint8_t channel);

    uint8_t ICACHE_RAM_ATTR getNextSwitchIndex();
    void ICACHE_RAM_ATTR setSentSwitch(uint8_t index, uint8_t value);

///// Variables for OpenTX Syncing //////////////////////////
    #define OpenTXsyncPacketInterval 200 // in ms
    static void ICACHE_RAM_ATTR setSyncParams(uint32_t PacketInterval);
    static void ICACHE_RAM_ATTR JustSentRFpacket();
    static void ICACHE_RAM_ATTR sendSyncPacketToTX();

    /////////////////////////////////////////////////////////////

    static void ICACHE_RAM_ATTR GetChannelDataIn();
    static void ICACHE_RAM_ATTR updateSwitchValues();

    static void inline nullCallback(void);

    static void handleUARTin();
    bool RXhandleUARTout();

private:
    Stream *_dev;

    static volatile uint8_t SerialInPacketLen;                   // length of the CRSF packet as measured
    static volatile uint8_t SerialInPacketPtr;                   // index where we are reading/writing

    static volatile inBuffer_U inBuffer;
    static volatile uint8_t CRSFoutBuffer[CRSF_MAX_PACKET_LEN + 1]; //index 0 hold the length of the datapacket

    static volatile bool CRSFframeActive;  //since we get a copy of the serial data use this flag to know when to ignore it

#if CRSF_TX_MODULE
    /// OpenTX mixer sync ///
    static volatile uint32_t OpenTXsyncLastSent;
    static uint32_t RequestedRCpacketInterval;
    static volatile uint32_t RCdataLastRecv;
    static volatile int32_t OpenTXsyncOffset;
    static uint32_t OpenTXsyncOffsetSafeMargin;
#ifdef FEATURE_OPENTX_SYNC_AUTOTUNE
    static uint32_t SyncWaitPeriodCounter;
#endif

    /// UART Handling ///
    static uint32_t GoodPktsCount;
    static uint32_t BadPktsCount;
    static uint32_t UARTwdtLastChecked;
    static uint32_t UARTcurrentBaud;
    static bool CRSFstate;

#ifdef PLATFORM_ESP32
    static void ESP32uartTask(void *pvParameters);
    static void ESP32syncPacketTask(void *pvParameters);
#endif

    static void duplex_set_RX();
    static void duplex_set_TX();
    static bool ProcessPacket();
    static void handleUARTout();
    static bool UARTwdt();
#endif

    static void flush_port_input(void);
};

#endif
