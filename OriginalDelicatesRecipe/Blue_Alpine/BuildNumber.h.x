// compute the build number as a compile-time constant
// build number should update every time this file is compiled, meaning we can have isolated build numbers for diffrent translation uints

#pragma once
/*
constexpr unsigned long long GET_BUID_NUM()
{
    unsigned year = 0;
    unsigned month = 0;
    unsigned day = 0;
    unsigned hour = 0;
    unsigned min = 0;
    unsigned sec = 0;
    unsigned long long build = 0;

    switch (__DATE__[0])
    {
    case 'J':  // Jan, Jun, July
        if (__DATE__[1] == 'a')
            month = 1;
        else
            month = __DATE__[2] == 'n' ? 6 : 7;
        break;
    case 'F':  // Feb
        month = 2;
        break;
    case 'M':  // Mar, May
        month = __DATE__[2] == 'r' ? 3 : 5;
        break;
    case 'A':  // Apr, Aug
        month = __DATE__[2] == 'r' ? 4 : 8;
        break;
    case 'S':  // Sep
        month = 9;
        break;
    case 'O':  // Oct
        month = 10;
        break;
    case 'N':  // Nov
        month = 11;
        break;
    case 'D':  // Dec
        month = 12;
        break;
    }

    year = (__DATE__[7]-'0') * 1000
        + (__DATE__[8]-'0') * 100
        + (__DATE__[9]-'0') * 10
        + (__DATE__[10]-'0') * 1;

    day = (__DATE__[4]-'0') * 10
        + (__DATE__[5]-'0') * 1;

    hour = (__TIME__[0]-'0') * 10
        + (__TIME__[1]-'0') * 1;

    min = (__TIME__[3]-'0') * 10
        + (__TIME__[4]-'0') * 1;

    sec = (__TIME__[6]-'0') * 10
        + (__TIME__[7]-'0') * 1;

    build = year;
    build = build * 100 + month;
    build = build * 100 + day;
    build = build * 100 + hour;
    build = build * 100 + min;
    build = build * 100 + sec;

    return build;
}
*/

#define STR_IMPL_(x) #x  //stringify argument
#define STR(x) STR_IMPL_(x)  //indirection to expand argument macros

inline const unsigned long long BUILD_TIMESTAMP =
((__TIME__[6]-'0') * 10 + (__TIME__[7]-'0')) +
((__TIME__[3]-'0') * 10 + (__TIME__[4]-'0')) * 100 +
((__TIME__[0]-'0') * 10 + (__TIME__[1]-'0')) * 100*100 +
((__DATE__[4]-'0') * 10 + (__DATE__[5]-'0')) * 100*100*100 +
 (__DATE__[0] == 'J'?(__DATE__[1] == 'a'?1:(__DATE__[2] == 'n' ? 6 : 7)) :
 (__DATE__[0] == 'F' ? 2 :
 (__DATE__[0] == 'M' ? (__DATE__[2] == 'r' ? 3 : 5) :
 (__DATE__[0] == 'A' ? (__DATE__[1] == 'p' ? 4 : 8) :
 (__DATE__[0] == 'S' ? 9 :
 (__DATE__[0] == 'O' ? 10 :
 (__DATE__[0] == 'N' ? 11 : 12))))))) * 100000000LL +\
((__DATE__[7]-'0') * 1000 + (__DATE__[8]-'0') * 100 + (__DATE__[9]-'0') * 10 + (__DATE__[10]-'0') * 1) * 100*100000000LL;

#define BUILD_TIMESTAMP_STRING /*\
     __DATE__[7] __DATE__[8] __DATE__[9] __DATE__[10] \
    (__DATE__[0] == "O" ? "1" : (__DATE__[0] == "N" ? "1" : (__DATE__[0] == "D" ? "1" : "0")) \
    (__DATE__[0] == "J" ? (__DATE__[1] == "a" ? "1" : (__DATE__[2] == "n" ? "6" : "7")) : \
    (__DATE__[0] == "F" ? "2" : \
    (__DATE__[0] == "M" ? (__DATE__[2] == "r" ? "3" : "5") : \
    (__DATE__[0] == "A" ? (__DATE__[1] == "p" ? "4" : "8") : \
    (__DATE__[0] == "S" ? "9" : \
    (__DATE__[0] == "O" ? "10" : \
    (__DATE__[0] == "N" ? "11" : "12"))))))) \
    __DATE__[4] __DATE__[5] \
    __TIME__[0] __TIME__[1] \
    __TIME__[3] __TIME__[4] \
    __TIME__[6] __TIME__[7] \*/
