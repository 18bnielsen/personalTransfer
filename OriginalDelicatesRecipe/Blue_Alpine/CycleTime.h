#pragma once
#include "Arduino.h"

/*
A heavily-modified version of the standard Seeed-Arduino-RTC and Time classes
    Added arithmetic and boolean operators
    Times are now zero-indexed (even though in RTC hardware days and months aren't)
    Assignment can be performed using Time objects or integer-seconds
    Literal operators convert minuets, hours and days into seconds
    Though the Time class supports up to 256 days, SAMD51_RTC counts are truncated to one month

ALTERNATIVE STRATEGY (not used):
    Use millis() as the base timer
    This has the added benefit of giving us millisecond-precision
    Might also be able to use <chrono>
*/



const inline unsigned operator""_sec(const unsigned long long i) { return i; }
const inline unsigned operator""_min(const unsigned long long i) { return i * 60; }
const inline unsigned operator""_hr (const unsigned long long i) { return i * 3600; }
const inline unsigned operator""_day(const unsigned long long i) { return i * 86400; }



struct Time
{
private:
    char _seconds;
    char _minutes;
    char _hours;
    char _days;

public:
    const char& Seconds = _seconds;
    const char& Minutes = _minutes;
    const char& Hours   = _hours;
    const char& Days    = _days;

    Time(unsigned days,    unsigned hours,   unsigned minutes, unsigned seconds);

    Time(unsigned hours,   unsigned minutes, unsigned seconds) : Time(0, hours, minutes, seconds) {}
    Time(unsigned minutes, unsigned seconds)                   : Time(0,     0, minutes, seconds) {}
    Time(unsigned seconds)                                     : Time(0,     0,       0, seconds) {}
    Time()                                                     : Time(0,     0,       0,       0) {}

    Time(const Time& other) { *this = other; }

    unsigned totalHours()   { return Hours   + Days           * 24; }
    unsigned totalMinutes() { return Minutes + totalHours()   * 60; }
    unsigned totalSeconds() { return Seconds + totalMinutes() * 60; }

    /// TODO: Add formatting options (ex, BA_Standard: "00 : 00 : 00")
    String toString() { return String(totalHours(), 10) + ":" + String(Minutes, 10) + ":" + String(Seconds, 10); }
    
    template <size_t N>
    void toString(char(&buf)[N], size_t len = N) { toString().toCharArray(buf, len); }


    Time operator +  (const Time& other);
    Time operator -  (const Time& other);
    Time operator  = (const Time& other) { return *(uint32_t*)this =  *(uint32_t*)&other; }

    Time operator += (const Time& other) { return *this = *this + other; }
    Time operator -= (const Time& other) { return *this = *this - other; }

    bool operator >  (const Time& other) { return *(uint32_t*)this >  *(uint32_t*)&other; }
    bool operator <  (const Time& other) { return *(uint32_t*)this <  *(uint32_t*)&other; }
    bool operator >= (const Time& other) { return *(uint32_t*)this >= *(uint32_t*)&other; }
    bool operator <= (const Time& other) { return *(uint32_t*)this <= *(uint32_t*)&other; }
    bool operator == (const Time& other) { return *(uint32_t*)this == *(uint32_t*)&other; }
    bool operator != (const Time& other) { return *(uint32_t*)this != *(uint32_t*)&other; }


    Time operator  = (const unsigned seconds) { return *this  = Time(seconds); }
    Time operator += (const unsigned seconds) { return *this += Time(seconds); }
    Time operator -= (const unsigned seconds) { return *this -= Time(seconds); }
    Time operator +  (const unsigned seconds) { return *this +  Time(seconds); }
    Time operator -  (const unsigned seconds) { return *this -  Time(seconds); }

    bool operator >  (const unsigned seconds) { return *this >  Time(seconds); }
    bool operator <  (const unsigned seconds) { return *this <  Time(seconds); }
    bool operator >= (const unsigned seconds) { return *this >= Time(seconds); }
    bool operator <= (const unsigned seconds) { return *this <= Time(seconds); }
    bool operator == (const unsigned seconds) { return *this == Time(seconds); }
    bool operator != (const unsigned seconds) { return *this != Time(seconds); }

    bool operator !  () { return *(uint32_t*)this == 0; }
};

/* NOTE:
    C++ doesn't have a concept of static classes like, say, C#
    What this actually is is a static instance of of the SAMD51_RTC class, ie, this instance is unique to any translation unit it is included in. This is okay right now because (a) the class contans no member-variables and all functions would work the same for any instance, (b) this program is compiled into only one translation unit.
    True singleton classes can be created by making every member static, but this isn't considered good form in C++. It's usually prefered to use a namespace instead.
*/

static class SAMD51_RTC
{
    friend void Command_Terminal();
private:
    enum
    {
        RESET_YEAR   = 0, // 0..63
        RESET_MONTH  = 1, // 1..12
        RESET_DAY    = 1, // 1..31
        RESET_HOUR   = 0, // 0..23
        RESET_MINUTE = 0, // 0..59
        RESET_SECOND = 0, // 0..59
    };

    enum Alarm_Match : uint8_t // Should we have this enum or just use the identifiers from /component/rtc.h ?
    {
        MATCH_OFF          = RTC_MODE2_MASK_SEL_OFF_Val,         // Never
        MATCH_SS           = RTC_MODE2_MASK_SEL_SS_Val,          // Every Minute
        MATCH_MMSS         = RTC_MODE2_MASK_SEL_MMSS_Val,        // Every Hour
        MATCH_HHMMSS       = RTC_MODE2_MASK_SEL_HHMMSS_Val,      // Every Day
        MATCH_DHHMMSS      = RTC_MODE2_MASK_SEL_DDHHMMSS_Val,    // Every Month
        MATCH_MMDDHHMMSS   = RTC_MODE2_MASK_SEL_MMDDHHMMSS_Val,  // Every Year
        MATCH_YYMMDDHHMMSS = RTC_MODE2_MASK_SEL_YYMMDDHHMMSS_Val // Once, on a specific date and a specific time
    };

    void config32kOSC(void);
    void configureClock(void);
    void RTCreadRequest();
    bool RTCisSyncing(uint32_t flag = RTC_MODE2_SYNCBUSY_MASK_);
    void RTCdisable();
    void RTCenable();
    void RTCreset();
    void RTCresetRemove();

public:
    bool begin();

    unsigned totalHours()   { return ((Time)(*this)()).totalHours();   }
    unsigned totalMinutes() { return ((Time)(*this)()).totalMinutes(); }
    unsigned totalSeconds() { return ((Time)(*this)()).totalSeconds(); }

    String toString() { return ((Time)(*this)()).toString(); }

    template <size_t N>
    void toString(char(&buf)[N], size_t len = N) { toString().toCharArray(buf, len); }

    // Can't do read-only getters because math needs to be performed on Days :(

    const Time operator () ();
    const Time operator =  (const Time& other);

    const Time operator +  (const Time& other) { return (Time)(*this)() + other; }
    const Time operator -  (const Time& other) { return (Time)(*this)() - other; }
    const Time operator += (const Time& other) { return *this = *this + other; }
    const Time operator -= (const Time& other) { return *this = *this - other; }

    const bool operator >  (const Time& other) { return (Time)(*this)() >  other; }
    const bool operator <  (const Time& other) { return (Time)(*this)() <  other; }
    const bool operator >= (const Time& other) { return (Time)(*this)() >= other; }
    const bool operator <= (const Time& other) { return (Time)(*this)() <= other; }
    const bool operator == (const Time& other) { return (Time)(*this)() == other; }
    const bool operator != (const Time& other) { return (Time)(*this)() != other; }


    const Time operator  = (const unsigned& seconds) { return *this =  Time(seconds); }
    const Time operator += (const unsigned& seconds) { return *this += Time(seconds); }
    const Time operator -= (const unsigned& seconds) { return *this -= Time(seconds); }
    const Time operator +  (const unsigned& seconds) { return *this +  Time(seconds); }
    const Time operator -  (const unsigned& seconds) { return *this -  Time(seconds); }

    const bool operator >  (const unsigned& seconds) { return *this >  Time(seconds); }
    const bool operator <  (const unsigned& seconds) { return *this <  Time(seconds); }
    const bool operator >= (const unsigned& seconds) { return *this >= Time(seconds); }
    const bool operator <= (const unsigned& seconds) { return *this <= Time(seconds); }
    const bool operator == (const unsigned& seconds) { return *this == Time(seconds); }
    const bool operator != (const unsigned& seconds) { return *this != Time(seconds); }


    operator Time() { return (*this)(); }

} CycleTime;