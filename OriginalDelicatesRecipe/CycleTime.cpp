#include "CycleTime.h"

/// struct Time ///

Time::Time(unsigned days, unsigned hours, unsigned minutes, unsigned seconds) {
    minutes = minutes + seconds / 60;
    hours   = hours + minutes / 60;
    days    = days + hours / 24;

    seconds %= 60;
    minutes %= 60;
    hours   %= 24;
    days    %= 30;

    _seconds = seconds;
    _minutes = minutes;
    _hours   = hours;
    _days    = days;
}


Time Time::operator + (const Time& other) {
    return Time(Days + other.Days, Hours + other.Hours, Minutes + other.Minutes, Seconds + other.Seconds);
}

Time Time::operator - (const Time& other) {
    if (*this < other)
        return Time(0);
    else
        return Time(totalSeconds() - ((Time)other).totalSeconds());
}



/// static class SAMD51_RTC //

typedef void (*rtcCallBack)(uint32_t flag);
rtcCallBack RTC_callBack = NULL;

bool SAMD51_RTC::begin()
{
    config32kOSC();

    uint16_t tmp_reg_a = 0;

    // If the RTC is in clock mode and the reset was
    // not due to POR or BOD, preserve the clock time
    // POR causes a reset anyway, BOD behaviour is?
    bool validTime = false;
    RTC_MODE2_CLOCK_Type oldTime;

    if (RTC->MODE2.CTRLA.reg & RTC_MODE2_CTRLA_MODE_CLOCK)
    {
        validTime = true;
        oldTime.reg = RTC->MODE2.CLOCK.reg;
    }

    configureClock();

    RTCdisable();

    RTCreset();

    tmp_reg_a |= RTC_MODE2_CTRLA_MODE_CLOCK;        // set clock operating mode
    tmp_reg_a |= RTC_MODE2_CTRLA_PRESCALER_DIV1024; // set prescaler to 1024 for MODE2 so that f = GCLK_RTC / 1024 = 1.024K / 1024 = 1Hz
    tmp_reg_a &= ~RTC_MODE2_CTRLA_MATCHCLR;         // disable clear on match

    //According to the datasheet RTC_MODE2_CTRL_CLKREP = 0 for 24h
    tmp_reg_a &= ~RTC_MODE2_CTRLA_CLKREP; // 24h time representation

    RTC->MODE2.CTRLA.reg = tmp_reg_a;
    while (RTCisSyncing());

    NVIC_EnableIRQ(RTC_IRQn); // enable RTC interrupt
    NVIC_SetPriority(RTC_IRQn, 0x00);

    RTC->MODE2.INTENSET.reg |= RTC_MODE2_INTENSET_ALARM0; // enable alarm0 interrupt
    RTC->MODE2.Mode2Alarm[0].MASK.bit.SEL = MATCH_OFF;    // default alarm0 match is off (disabled)

    RTC->MODE2.INTENSET.reg |= RTC_MODE2_INTENSET_ALARM1; // enable alarm1 interrupt
    RTC->MODE2.Mode2Alarm[1].MASK.bit.SEL = MATCH_OFF;    // default alarm1 match is off (disabled)

    while (RTCisSyncing());

    RTCenable();
    RTCresetRemove();

    // If desired and valid, restore the time value, else use first valid time value
    if ((validTime) && (oldTime.reg != 0L))
    {
        RTC->MODE2.CLOCK.reg = oldTime.reg;
    }
    else
    {
        RTC->MODE2.CLOCK.reg = RTC_MODE2_CLOCK_YEAR(RESET_YEAR) | RTC_MODE2_CLOCK_MONTH(RESET_MONTH) | RTC_MODE2_CLOCK_DAY(RESET_DAY) | RTC_MODE2_CLOCK_HOUR(RESET_HOUR) | RTC_MODE2_CLOCK_MINUTE(RESET_MINUTE) | RTC_MODE2_CLOCK_SECOND(RESET_SECOND);
    }

    while (RTCisSyncing());

    return true;
}

const Time SAMD51_RTC::operator=(const Time& other)
{
    RTC_MODE2_CLOCK_Type newTime;

    newTime.bit.SECOND = other.Seconds < 0 ? 0 : other.Seconds;
    newTime.bit.MINUTE = other.Minutes < 0 ? 0 : other.Minutes;
    newTime.bit.HOUR   = other.Hours   < 0 ? 0 : other.Hours;
    newTime.bit.DAY    = other.Days    < 0 ? 0 : other.Days + 1;
    newTime.bit.MONTH = 1;
    newTime.bit.YEAR = 0;

    RTC->MODE2.CLOCK.reg = newTime.reg;
    while (RTCisSyncing());

    return (*this)();
}

const Time SAMD51_RTC::operator()()
{
    RTCreadRequest();
    char ss = RTC->MODE2.CLOCK.bit.SECOND;
    char mm = RTC->MODE2.CLOCK.bit.MINUTE;
    char hh = RTC->MODE2.CLOCK.bit.HOUR;
    char d = RTC->MODE2.CLOCK.bit.DAY - 1;

    auto t = Time(d, hh, mm, ss);

    // enable this for some rudimentary protection days to overflow at diffrent values depending on the month
    // (only works if this operator is called at least once/month)
    /*
    if (RTC->MODE2.CLOCK.bit.MONTH > 1)
    {
        (*this) = t;
    }
    //*/

    return t;
}

//Time SAMD51_RTC::alarm(uint8_t id)
//{
//    if (id >= 2)
//        return 0;
//    uint8_t ss = RTC->MODE2.Mode2Alarm[0].ALARM.bit.SECOND;
//    uint8_t mm = RTC->MODE2.Mode2Alarm[0].ALARM.bit.MINUTE;
//    uint8_t hh = RTC->MODE2.Mode2Alarm[0].ALARM.bit.HOUR;
//    uint8_t d = RTC->MODE2.Mode2Alarm[0].ALARM.bit.DAY;
//    uint8_t m = RTC->MODE2.Mode2Alarm[0].ALARM.bit.MONTH;
//    uint16_t y = RTC->MODE2.Mode2Alarm[0].ALARM.bit.YEAR;
//
//    return Time(y, m, d, hh, mm, ss);
//}
//
//void SAMD51_RTC::setAlarm(uint8_t id, const Time& dt)
//{
//    if (id >= 2)
//        return;
//    RTC_MODE2_CLOCK_Type alarmTime;
//
//    alarmTime.bit.SECOND = dt.Seconds;
//    alarmTime.bit.MINUTE = dt.Minutes;
//    alarmTime.bit.HOUR = dt.Hours;
//    alarmTime.bit.DAY = dt.Days;
//    alarmTime.bit.MONTH = dt.months();
//    alarmTime.bit.YEAR = dt.years()/* - 2000*/;
//
//    RTC->MODE2.Mode2Alarm[id].ALARM.reg = alarmTime.reg;
//    if (id == 0)
//    {
//        while (RTCisSyncing(RTC_MODE2_SYNCBUSY_ALARM0))
//            ;
//    }
//    else
//    {
//        while (RTCisSyncing(RTC_MODE2_SYNCBUSY_ALARM1))
//            ;
//    }
//}
//
//void SAMD51_RTC::enableAlarm(uint8_t id, Alarm_Match match)
//{
//    if (id >= 2)
//        return;
//    RTC->MODE2.Mode2Alarm[id].MASK.bit.SEL = match;
//    while (RTCisSyncing(RTC_MODE2_SYNCBUSY_ALARM_Msk))
//        ;
//}
//
//void SAMD51_RTC::disableAlarm(uint8_t id)
//{
//    if (id >= 2)
//        return;
//    RTC->MODE2.Mode2Alarm[id].MASK.bit.SEL = 0x00;
//    while (RTCisSyncing(RTC_MODE2_SYNCBUSY_ALARM_Msk))
//        ;
//}
//
//void SAMD51_RTC::attachInterrupt(rtcCallBack callback)
//{
//    RTC_callBack = callback;
//}
//
//void SAMD51_RTC::detachInterrupt()
//{
//    RTC_callBack = NULL;
//}
//
//void SAMD51_RTC::standbyMode()
//{
//    // Entering standby mode when connected
//    // via the native USB port causes issues.
//    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
//    __DSB();
//    __WFI();
//}

/* Attach peripheral clock to 32k oscillator and RTC*/
void SAMD51_RTC::configureClock()
{
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_RTC | MCLK_APBAMASK_OSC32KCTRL;
}

/*
 * Private Utility Functions
 */

 /* Configure the 32768Hz Oscillator */
void SAMD51_RTC::config32kOSC()
{
#ifdef CRYSTALLESS
    // This platform does not have external crystal.
    // Thus, we have to enable OSCULP32K to generate RTC clock.
    void* hw = (void*)OSC32KCTRL;
    uint16_t calib = 0;

    calib = ((Osc32kctrl*)hw)->OSCULP32K.reg;
    calib = (calib & OSC32KCTRL_OSCULP32K_CALIB_Msk) >> OSC32KCTRL_OSCULP32K_CALIB_Pos;
    //re-write calibrate
    ((Osc32kctrl*)hw)->OSCULP32K.reg &= ~OSC32KCTRL_OSCULP32K_WRTLOCK;

    ((Osc32kctrl*)hw)->OSCULP32K.reg |= calib;
    ((Osc32kctrl*)hw)->OSCULP32K.reg &= ~(OSC32KCTRL_OSCULP32K_EN32K);
    ((Osc32kctrl*)hw)->OSCULP32K.reg |= OSC32KCTRL_OSCULP32K_EN1K; //enable 1.024KHz OUPUT

    ((Osc32kctrl*)hw)->OSCULP32K.reg |= OSC32KCTRL_OSCULP32K_WRTLOCK;

    //Use 1.024KHz for RTC
    ((Osc32kctrl*)hw)->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL(OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K_Val);
#else
    // This platform has external crystal and Arduino core has already enabled the XOSC32K oscillator.
    // But the Arduino core does not enable 1.024[kHz] output.
    // Thus all we have to do is just enable 1.024[kHz] output by setting XOSC32K.EN1k to 1.
    OSC32KCTRL->XOSC32K.bit.EN1K = 1;
    // Use 1.024[kHz] output for RTC.
    OSC32KCTRL->RTCCTRL.bit.RTCSEL = OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K_Val;
#endif
}

/* Synchronise the CLOCK register for reading*/
inline void SAMD51_RTC::RTCreadRequest()
{
    RTC->MODE2.CTRLA.reg |= RTC_MODE2_CTRLA_CLOCKSYNC;
    while (RTCisSyncing(RTC_MODE2_SYNCBUSY_SWRST | RTC_MODE2_SYNCBUSY_ENABLE | RTC_MODE2_SYNCBUSY_CLOCKSYNC))
        ;
}

/* Wait for sync in write operations */
inline bool SAMD51_RTC::RTCisSyncing(uint32_t flag)
{
    return (RTC->MODE2.SYNCBUSY.reg & flag);
}

void SAMD51_RTC::RTCdisable()
{
    RTC->MODE2.CTRLA.reg &= ~RTC_MODE2_CTRLA_ENABLE; // disable RTC
    while (RTCisSyncing(RTC_MODE2_SYNCBUSY_SWRST | RTC_MODE2_SYNCBUSY_ENABLE | RTC_MODE2_SYNCBUSY_CLOCKSYNC));
}

void SAMD51_RTC::RTCenable()
{
    RTC->MODE2.CTRLA.reg |= RTC_MODE2_CTRLA_ENABLE; // enable RTC
    while (RTCisSyncing(RTC_MODE2_SYNCBUSY_SWRST | RTC_MODE2_SYNCBUSY_ENABLE | RTC_MODE2_SYNCBUSY_CLOCKSYNC));
}

void SAMD51_RTC::RTCreset()
{
    RTC->MODE2.CTRLA.reg |= RTC_MODE2_CTRLA_SWRST; // software reset
    while (RTCisSyncing(RTC_MODE2_CTRLA_SWRST))
        ;
}

void SAMD51_RTC::RTCresetRemove()
{
    RTC->MODE2.CTRLA.reg &= ~RTC_MODE2_CTRLA_SWRST; // software reset remove
    while (RTCisSyncing(RTC_MODE2_CTRLA_SWRST));
}

extern "C"
{
    void RTC_Handler(void)
    {
        if (RTC_callBack == NULL) return;

        uint32_t flag = 0;


        if (RTC->MODE2.INTFLAG.reg & RTC_MODE2_INTFLAG_ALARM0)
            flag |= 1;

        if (RTC->MODE2.INTFLAG.reg & RTC_MODE2_INTFLAG_ALARM1)
            flag |= 2;
        
        RTC_callBack(flag);
        
        if (flag & 1)
            RTC->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_ALARM0;

        if (flag & 2)
            RTC->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_ALARM1;
    }
}