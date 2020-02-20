#include "ESP8266_hwTimer.h"

void inline ICACHE_RAM_ATTR hwTimer::nullCallback(void){};

void (*hwTimer::callbackTick)() = &nullCallback; // function is called whenever there is new RC data.
void (*hwTimer::callbackTock)() = &nullCallback; // function is called whenever there is new RC data.

volatile uint32_t hwTimer::HWtimerInterval = TimerIntervalUSDefault;
volatile bool hwTimer::TickTock = false;
volatile int32_t hwTimer::PhaseShift = 0;
bool hwTimer::ResetNextLoop = false;

uint8_t hwTimer::HWtimerDiv = 0;
uint8_t hwTimer::HWtimerCounter;

bool hwTimer::NewIntervalReq = false;
volatile uint32_t hwTimer::newHWtimerInterval = 0;

uint32_t hwTimer::LastCallbackMicrosTick = 0;
uint32_t hwTimer::LastCallbackMicrosTock = 0;

void ICACHE_RAM_ATTR hwTimer::init()
{
    noInterrupts();

    timer0_isr_init();
    timer0_attachInterrupt(hwTimer::callback);
    //timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP); //5MHz ticks
    timer0_write(80000000);
    interrupts();
}

void ICACHE_RAM_ATTR hwTimer::stop()
{
    timer1_detachInterrupt();
}

void ICACHE_RAM_ATTR hwTimer::pause()
{
    timer1_detachInterrupt();
}

void ICACHE_RAM_ATTR hwTimer::updateInterval(uint32_t Interval)
{
    //hwTimer::HWtimerInterval = newTimerInterval * 5;
    //timer1_write(hwTimer::HWtimerInterval >> 1);
    hwTimer::newHWtimerInterval = Interval * 80;
    hwTimer::NewIntervalReq = true;
}

void ICACHE_RAM_ATTR hwTimer::phaseShift(int32_t ReqPhaseShift)
{
    // int32_t MaxPhaseShift = hwTimer::HWtimerInterval >> 1;
    // int32_t NewPhaseShift;

    // if (ReqPhaseShift > MaxPhaseShift)
    // {
    //     NewPhaseShift = MaxPhaseShift;
    // }
    // else
    // {
    //     NewPhaseShift = ReqPhaseShift;
    // }

    // if (ReqPhaseShift < -MaxPhaseShift)
    // {
    //     NewPhaseShift = -MaxPhaseShift;
    // }
    // else
    // {
    //     NewPhaseShift = ReqPhaseShift;
    // }
    hwTimer::PhaseShift = ReqPhaseShift;
}

void ICACHE_RAM_ATTR hwTimer::callback()
{
    noInterrupts();
    if (hwTimer::TickTock)
    {
        //uint32_t timerVal;

        if (NewIntervalReq)
        {
            hwTimer::NewIntervalReq = false;
            hwTimer::HWtimerInterval = hwTimer::newHWtimerInterval;

            // if (newHWtimerInterval < HWtimerInterval)
            // {
            //     hwTimer::PhaseShift = hwTimer::HWtimerInterval;
            // }
            // else
            // {
            //     hwTimer::PhaseShift = -hwTimer::HWtimerInterval;
            // }
        }

        // if (hwTimer::ResetNextLoop)
        // {

        //     timer1_write(hwTimer::HWtimerInterval >> 1);
        //     //timerVal = hwTimer::HWtimerInterval >> 1;
        //     hwTimer::ResetNextLoop = false;
        // }
        // else
        // {
        timer0_write((hwTimer::HWtimerInterval >> 1) + hwTimer::PhaseShift);

        // if (hwTimer::PhaseShift > 0 || hwTimer::PhaseShift < 0)
        // {2

        //     timerVal = (hwTimer::HWtimerInterval + hwTimer::PhaseShift) >> 1;

        //     hwTimer::ResetNextLoop = true;
        //     hwTimer::PhaseShift = 0;
        // }

        //hwTimer::LastCallbackMicrosTick = micros();

        hwTimer::LastCallbackMicrosTick = ESP.getCycleCount();
        hwTimer::callbackTick();
    }
    else
    {

        //hwTimer::LastCallbackMicrosTock = micros();

        //timer1_write((hwTimer::HWtimerInterval >> 1));

        hwTimer::LastCallbackMicrosTock = ESP.getCycleCount();
        timer0_write((hwTimer::HWtimerInterval >> 1) + hwTimer::PhaseShift);
        hwTimer::callbackTock();

        //timer1_write(hwTimer::HWtimerInterval >> 1);
        //timer1_write((hwTimer::HWtimerInterval >> 1) + hwTimer::PhaseShift);
    }
    hwTimer::TickTock = !hwTimer::TickTock;
    interrupts();
}