#pragma once

#include <cstdint>

#define WAIT_FOR_RESYNC 1000

typedef enum {
    SENDER_IDLE = 0,
    SENDING,
    SEND_NEXT,
    WAIT_UNTIL_NEXT_CONFIRM,
    RESYNC
} stubborn_sender_state_s;

class StubbornSender
{
public:
    StubbornSender(uint8_t maxPackageIndex);
    void ResetState();
    void SetDataToTransmit(uint8_t lengthToTransmit, uint8_t* dataToTransmit, uint8_t bytesPerCall);
    void GetCurrentPayload(uint8_t *packageIndex, uint8_t *count, uint8_t **currentData);
    void ConfirmCurrentPayload(bool telemetryConfirmValue);
    bool IsActive();
private:
    uint8_t *data;
    volatile uint8_t length;
    volatile uint8_t bytesPerCall;
    volatile uint8_t currentOffset;
    volatile uint8_t currentPackage;
    volatile bool waitUntilTelemetryConfirm;
    volatile bool resetState;
    volatile uint16_t waitCount;
    volatile uint8_t maxPackageIndex;
    volatile stubborn_sender_state_s senderState;
};
