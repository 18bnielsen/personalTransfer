/* 
This is our main file for our system. This file contains our global variables, our setup, and main loop.
*/ 



#define RELEASE
#define CONFIG
/*
#define EXPERIMENTAL
#define CONFIG " EXPERIMENTAL " __DATE__ " " __TIME__;
*/

#define REV 2
#define VERSION "Delicates Two Phase 2.0"

//Included libraries
#include <SdFat.h>
#include <Adafruit_SPIFlash.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <TemperatureZero.h>
#include <AutoPID.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <Adafruit_FT6206.h>
#include <FlashStorage_SAMD.h>
#include <Wire.h>
#include "bitmaps.h"
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>


//Custom Created Fonts to be used
#include <Fonts/Rajdhani_SemiBold9pt7b.h>
#include <Fonts/Rajdhani_SemiBold12pt7b.h>
#include <Fonts/Rajdhani_SemiBold20pt7b.h>
#include <Fonts/Nunito_Black20pt7b.h>
#include <Fonts/Nunito_Black10pt7b.h>
#include <Fonts/Nunito_Black14pt7b.h>
#include <Fonts/Nunito_Black30pt7b.h>

#include "Max31865.h"
#include "CycleTime.h"


#if   REV == 1
const char SOFTWARE_VERSION[] = "V 1." VERSION CONFIG;
#define OFFSET 90
#elif REV == 2
const char SOFTWARE_VERSION[] = "V 2." VERSION CONFIG;
#define OFFSET 0  // in pascals
#endif


//Color theme colors
#define MAIN_THEME 0x134B
#define COMP_THEME 0x428B
#define BACKGROUND 0x4208
#define BUTTON 0x3106
#define BUTTON2 0xE73D
#define TFT_GRAY 0x9cf3
#define TFT_GREEN2 0x5ec7
#define ICON 0xffff
#define PG_TXT 0xffff
#define TITLE 0xffff

// needed for external-declaration it in Max31865 class
extern const uint16_t bg_default = BACKGROUND;


//Color Theme for QC Test Pages
#define TEST_BACKGROUND_GRAY 0x4228
#define TEST_BACKGROUND_BLUE 0X19E9
#define TEST_BUTTON 0x04B6

//Define Digital Outputs These pins are used to activate our hardware 
#define VAC 44
#define FRIDGE 45
#define FAN 46
#define TRIAC 40
#define BUZZER 11
#define Buffer_Reset 12  // Chip pin 125, PB00, is pin 12 in arduino...

//PID Values
#define KP 2.0
#define KI 0
#define KD 0.5


//Definitions for Thermal Sensors

// The value of the Rref resistor. We use 4960 as a calibrated value for our Temp sensor
#if REV == 1
#define RREF      4960.0
#elif REV == 2
#define RREF      4030.0
#endif

// The 'nominal' 0-degrees-C resistance of the sensor. We use 1000.0 for PT1000
#define RNOMINAL  1000.0



// Neopixel Definitions
#define RGB 88
#define NUMPIXELS 1


// how long the dry cycle should wait before checking the other two timeouts (minutes)
#define DRY_INIT_TIMEOUT_min 2

// how long to allow a disconnected pressure sensor (milliseconds)
#define PRES_POLL_TIMEOUT 2_min

// how long to allow the pressure to be out of bounds before aborting (milliseconds)
#define PRES_INV_TIMEOUT  5_min
#define PRES_INV_TIMEOUT_STRICT  10_sec

// how long to allow the temperature sensors to be offline before going to an error page
#define SHELF_TEMP_TIMEOUT 3_min
#define CHMBR_TEMP_TIMEOUT 3_min


#define PRES_INV_TIMEOUT_STRICT_ms  (PRES_INV_TIMEOUT_STRICT * 1000)
#define SHELF_TEMP_TIMEOUT_ms       (SHELF_TEMP_TIMEOUT * 1000)
#define CHMBR_TEMP_TIMEOUT_ms       (CHMBR_TEMP_TIMEOUT * 1000)
#define PRES_INV_TIMEOUT_ms         (PRES_INV_TIMEOUT * 1000)
#define PRES_POLL_TIMEOUT_ms        (PRES_POLL_TIMEOUT * 1000)

// uncommecnt this to include the chamber sensor in standard sensor checks and crieria to leave fatal_sensor_error()
///#define INCL_CHMBR_IN_STD_CHECK


// the following are signed to prevent CommandTerminal from wrapping them
int PressureTimeOffset = 0;         // used by CommandTerminal to hack/fake TimeSinceLast*
int TimeOfLastBufferPacket = 0;     // set if we have any incomming packet on UART
int TimeOfLastPressureRead = 0;     // set if the received packet is considered valid
int TimeSinceLastBufferPacket = 0;  // Time in milliseconds. A large value indicates the buffer is disconnected.
int TimeSinceLastValidPresure = 0;  // Time in milliseconds. A large value may indicate the sensor is disconnected, or simply that the chamber is currently at atmospheric pressure.
int PressureReconnectTime = 0; // Time in milliseconds. Used for calculating uptime.


enum {
    VACCUM_ERROR_DEFAULT,
    VACCUM_ERROR_BUFFER_DISCONNECT,
    VACCUM_ERROR_SENSOR_DISCONNECT,
} Vaccum_Error_Reason;



//Hardware objects
MCUFRIEND_kbv tft;  // Display object
Adafruit_FT6206 ts = Adafruit_FT6206();  // Touchscreen object /Used by buttons

Max31865 ChmbrTemp = Max31865(22, RREF, RNOMINAL);
Max31865 ShelfTemp = Max31865(23, RREF, RNOMINAL);

/// TODO: what IS this??
TemperatureZero TempZero = TemperatureZero();  // Internal temperature sensor

Adafruit_FlashTransport_QSPI flashTransport;  // Flash objects
Adafruit_SPIFlash flash(&flashTransport);
FatFileSystem fatfs;  // file system object from SdFat

Adafruit_NeoPixel pixels(NUMPIXELS, RGB, NEO_GRB + NEO_KHZ800);  // NeoPixel Object


//touch variables
int ts_x;                                
int ts_y;

struct TestVariables
{
    // QC vars
    uint16_t shelf_resistance;
    uint16_t chamber_resistance;
    uint32_t ambient_pressure_1;
    uint32_t ambient_pressure_2;
    uint32_t cold_pressure_1;
    float chamber_test_f;
    float heating_test_f;
    int qc_test_num;
    float leak_rate_init;
    float leak_rate_final;
    float leak_rate_test;
    bool leak_only;
    bool off_gas;
    bool heat_const;

    // Diagnostic tests

};

TestVariables Tests
{
    .shelf_resistance = 0,
    .chamber_resistance = 0,
    .ambient_pressure_1 = 760000,
    .ambient_pressure_2 = 760000,
    .cold_pressure_1 = 760000,
    .chamber_test_f = 255,
    .heating_test_f = 0,
    .qc_test_num = 0,
    .leak_rate_init = 0,
    .leak_rate_final = 0,
    .leak_rate_test = 0,
    .leak_only = false,
    .off_gas = true,
    .heat_const = false,
};

//PID Variables

bool relay, relay_2, relay_3, relay_4, relay_5, relay_6, relay_7, relay_8;
double set_pressure, defrost_temp, test_temp, complete_temp, candy_pre_warm_temp;
// Desired Shelf Temperature for Manual test page
const double manual_temp = 49;  // 120F


//page variables
int page;
int prev_page;
bool done;
int test_num;
int recipe_sel;
bool recipe_cycle;
bool button_sound;
bool cycle_sound;
bool defrost_fan;
bool f_reset;
bool sound;
const bool testingErrorPage = false;
const bool benchDevelopment = false;

bool ForceHeaterON;  // ignore normal checks and turn heater on
bool ForceHeaterOFF;  // ignore normal checks and turn heater off

bool IgnoreChmbrTemp = false;

bool completeVaccumOnAlways = true;


bool override_refresh, diagnostic_refresh_1,
     diagnostic_refresh_2, diagnostic_refresh_3,
     temp_refresh, pressure_refresh;
int selection, freeze_selector, dry_selector,
    prev_freeze_selector, prev_dry_selector,
    candy_selector, prev_candy_selector;

//temperature variables
/// TODO: Enumerate
//*
bool temp_unit;  // 0 celsius (false) //1 fahrenheit (true)
/*/
enum TemperatureUnit : bool
{
    CELCIUS,
    FAHRENHEIGHT,
} temp_unit;
//*/

// target shelf temperature in deg C
double ShelfTargetTemperature;

/// TODO: document/delete

double delta_pressure; // proportional pressure change
int pressure_unit; /// TODO: enumeerate
double pressure_;  // for the converted pressure in Pa
uint32_t chamber_pressure;  // raw pressure data in mTorr

bool vacError = false;
bool throwPopUp = false;

/*
 * Stored defaults pulled from memory
 */
double default_dry_temp;
bool freeze_cycle_status; 
bool dry_cycle_status;
bool complete_cycle_status;

// stored values for ground fault checker
bool fridge_fault_status;
bool vacuum_fault_status;
bool fan_fault_status;
bool heat_fault_status;

int default_freeze_hour, default_freeze_min, 
    default_dry_hour, default_dry_min;
int temp_unit_s, pressure_unit_s,
    freeze_day, dry_day, s_pressure;
int bsound, csound, d_fan, c_vac; 

// Structure and value initializers for different recipes
struct Recipe{
    int freeze_time;
    int dry_time;
    int pressure;
    double dry_temp;
};

bool freeze_zero = false;  // Freeze Zero will check if there is any actual freeze time set, candy modes don't require the freeze page.

// Candy cycle
bool is_candy = false;
bool start_warm_count = false;


// Initialize empty recipes to be given values from memory
Recipe current_recipe;

Recipe  Candy,
        Greens,
        Meat,
        Fruit,
        Liquid,
        Delicates,
        DelicatesSecondary;


// initialze default recipies
// conflicting legacy defaults are commented
const Recipe
    Candy_FactoryDefault{
		.freeze_time = (int) 0,
		.dry_time = (int) 2_hr,
		.pressure = (int) 1333 + OFFSET,
		.dry_temp = (int) 65,
	},

    Greens_FactoryDefault{
		.freeze_time = (int) 6_hr,
		.dry_time = (int) 17_hr,
		.pressure = (int) 146 + OFFSET,
		.dry_temp = (int) 47,
	},

    Meat_FactoryDefault{
		.freeze_time = (int) 9_hr,
		.dry_time = (int) 22_hr,
		.pressure = (int) 166 + OFFSET,
		.dry_temp = (int) 53,       
	},

    Fruit_FactoryDefault{
		.freeze_time = (int) 9_hr,
		.dry_time = (int) 24_hr,
		.pressure = (int) 146 + OFFSET,
		.dry_temp = (int) 46,
	},

    Liquid_FactoryDefault{
		.freeze_time = (int) 9_hr,
		.dry_time = (int) 24_hr,
		.pressure = (int) 146 + OFFSET,
		.dry_temp = (int) 48,
	},

    Delicates_FactoryDefault{
		.freeze_time = (int) 9_hr,
		.dry_time = (int) 24_hr,
		.pressure = (int) 80 + OFFSET,
		.dry_temp = (double) 1.111,
	},

  DelicatesSecondary_FactoryDefault{
    .freeze_time = (int) 0,
		.dry_time = (int) 24_hr,
		.pressure = (int) 80 + OFFSET,
		.dry_temp = (double) 29.444,
  };

/*
 * Time objects for storing default times and also for keeping track of
 * updating items to the screen
 */

/// TODO: Delete all these!
Time prev_dry_time;  // Variable for default changer
Time prev_freeze_time;  // Variable for default changer
Time freeze_time;  /// const?
Time dry_time;  // Holder for drying time
Time Current_Cycle_Duration; /// TODO: delete // Temporary time variable being updated on the freeze and dry cycle pages
Time remaining;  // remaining time after subtracting the elapsed time from a default time
Time test_time;  // time for each test time variable
Time recipe_dry_time;  // holder for recipe time
Time recipe_freeze_time;  // holder for freeze time
Time candy_warm_time;  // used for the candy pre-warm page
Time prev_candy_warm_time;  // variable for default changer
const Time defrost_time    = 90_min;  // Holder for defrost time
const Time candy_cool_time = 15_min;  // used for the candy cooldown page
const Time leak_rate_time = 2_min;  // Timer for the leak rate

/*
 * PID Objects for heating relay
 */
AutoPIDRelay pid_relay(&pressure_, &set_pressure, &relay, 1000, KP, KI, KD);  // Each page using the PID has its own settings and PID object
AutoPIDRelay defrost_relay((double*)& ShelfTemp.Celsius, &defrost_temp, &relay_2, 1000, KP, KI, KD);
AutoPIDRelay complete_relay((double*)&ShelfTemp.Celsius, &complete_temp, &relay_5, 1000, KP, KI, KD);
AutoPIDRelay manual_relay((double*)&ShelfTemp.Celsius, (double*)&manual_temp, &relay_6, 1000, KP, KI, KD);
AutoPIDRelay candy_pre_warm_relay((double*)&ShelfTemp.Celsius, &candy_pre_warm_temp, &relay_7, 1000, KP, KI, KD);
AutoPIDRelay candy_dry_relay((double*)&ShelfTemp.Celsius, &ShelfTargetTemperature, &relay_8, 1000, KP, KI, KD);

SoftwareSerial mySerial(1, 0);  // RX, TX // Assign pins to communicate via UART

int touch_reset;


// for testing coordinates via the command terminal
struct {
        uint16_t x0, y0;
        uint16_t x1, y1;
} DebugBox;




/// Helper functions ///

template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }

// returns the old prev_page
int setPage(int newPage)
{
    int oldPage = prev_page;
    prev_page = abs(page);
    page = abs(newPage);
    done = false;
    delay(10);
    return oldPage;
}

// restore time count and return to previous page
int unpause(Time count = 0)
{
    int oldPage = setPage(prev_page);
    prev_page *= -1;
    CycleTime = count;
    return oldPage;
}

// tri-state check
// @returns -1 if setting is too low
// @returns  1 if setting too high
// @returns  0 if setting within recommended values
template <typename T> int pressureSettingUnsafe(T setting)
{
    if      (setting <  80 + OFFSET) return -1;
    else if (setting > 200 + OFFSET) return  1;
    else                             return  0;
}

// camp pressure to within recommended values
template <typename T> void pressureSettingClamp(T& setting)
{
    if      (setting <  80 + OFFSET)  setting =  80 + OFFSET;
    else if (setting > 200 + OFFSET)  setting = 200 + OFFSET;
}


/// man functions ///

void setup()
{
    // Serial
    mySerial.begin(9600);  // start UART communication between buffer board and set baud rate
    mySerial.setTimeout(50);  // needed to overide the defualt 1 second timeout, this fixed errors where the screen was laggy and overall sped up the program
    Serial.begin(250000);  // start serial monitoring
    Serial.setTimeout(6);

    delay(150);

    pixels.begin();  // start LED 
    pixels.setBrightness(18);

    set_pixel_blue();

    //Flash initialization
    digitalWrite(10, HIGH);  // Needed to turn on backlight of display

    while (!flash.begin()) Serial.println("Flash could not begin");
    while (!fatfs.begin(&flash)) Serial.println("Flash system needs to be formatted");

    //TFT Display Initialization
    uint16_t ID = tft.readID();  // Gets ID of the display

    tft.begin(ID);  // Starts display
    tft.setRotation(1);  // Rotate to landscape  
    tft.fillScreen(TFT_WHITE);  // Splash screen backdrop

    tft.drawBitmap(10, 200, logo_1, 460, 41, 0x4229);  // Blue Alpine splash screen
    tft.drawBitmap(80, 30, alpine_1, 320, 213, 0x13B5);  // Splash screen part 2
    tft.setTextSize(1);
    delay(3000);


    ts.begin(40);  // Initialize the touchscreen
    Wire.setTimeout(100); // Don't let I2C clock-stretching trigger the watchdog


    page = 1;  // Start on the homepage                                         
    done = false;  // Page needs to be loaded
    f_reset = false;

    temp_unit = false;  // Default temperature unit is Celsius
    pressure_unit = 0;  // Default pressure unit is Pascals
    chamber_pressure = 0;  // Chamber pressure initialized to 0

    recipe_dry_time = Time(0, 0);  // Initialize the time for the recipe holder
    recipe_freeze_time = Time(0, 0);  // Initialize the time for the recipe holder
    candy_warm_time = Time(15, 0);  // Candy warm time for 10 min.


    pinMode(VAC, OUTPUT);  // Initialize hardware pins as outputs
    pinMode(FRIDGE, OUTPUT);
    pinMode(FAN, OUTPUT);
    pinMode(TRIAC, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(Buffer_Reset, OUTPUT);
    digitalWrite(Buffer_Reset, HIGH);

    generate_files();  // create files if needed  
    memory_check();  // Assign memory defaults if saved


    /// assign defaults from memory function
    ShelfTargetTemperature = default_dry_temp;
    temp_unit = (bool)temp_unit_s;
    pressure_unit = pressure_unit_s;
    freeze_time = Time(freeze_day, default_freeze_hour, default_freeze_min, 0);
    dry_time = Time(dry_day, default_dry_hour, default_dry_min, 0);

    CycleTime.begin();  // Start the clock and adjust to default time
    CycleTime = 0;

    // PID Relay value initialization
    relay = false;
    pid_relay.setTimeStep(500);

    relay_2 = false;
    defrost_relay.setTimeStep(500);

    relay_5 = false;
    complete_relay.setTimeStep(500);

    relay_6 = false;
    manual_relay.setTimeStep(500);

    relay_7 = false;
    candy_pre_warm_relay.setTimeStep(500);

    set_pressure = 550;                     // Desired pressure when running drying cycle
    defrost_temp = 65.5;         // 150 F   // Desired temperature for running defrost cycle
    complete_temp = 30;          //  86 F   // Desired Shelf Temperature for Completed Page
    candy_pre_warm_temp = 65.5;  // 120 F   // standard warming default temerature for candy pre warm

    ChmbrTemp.Begin();
    ShelfTemp.Begin();


    TempZero.init();

    // If power was lost return to freeze or dry cycle
    if (freeze_cycle_status || dry_cycle_status || complete_cycle_status) { page = 11; return; }

    // If power was lost during Fault Checker
    if (fridge_fault_status || vacuum_fault_status || fan_fault_status || heat_fault_status) { page = 47; return; }




    touch_reset = millis();



    /// check software version ///
    // Most of this is copied from generate_files() and I don't actually know what I'm doing :p
    const char swv_file_name[] = "software_version.txt";
    const char swv_key[] = "version";
    if (!fatfs.exists(swv_file_name))
    {
        // if no version file exists, we must be upgrading from v14 to v15
        // generate a version file and subtract 24 hours from the default freeze/dry times (consequence of mondifying the RTC, DateTime and TimeSpan classes)
        Serial.println("Generating software-version file");
        File swv_file = fatfs.open(swv_file_name, FILE_WRITE);

        char cbuf[64];
        StaticJsonDocument<64> swv_doc;

        swv_doc[swv_key] = VERSION;

        serializeJsonPretty(swv_doc, cbuf);
        swv_file.println(cbuf);
        swv_file.close();

        // we know we're running new software, but we don't want to modify settings that have already changed - for example, on the test machines in the shop.
        if (freeze_time > 24_hr && dry_time > 24_hr)
        {
            freeze_time -= 24_hr;
            dry_time -= 24_hr;

            store_values();
        }
    }

    /// TODO: figure out why this makes setup() hang
    //else
    //{
    //    char cbuf[64];
    //    StaticJsonDocument<64> swv_doc;
    //    File swv_file = fatfs.open(swv_file_name, O_RDWR);
    //
    //    if (swv_file)
    //    {
    //        swv_file.read(cbuf, swv_file.available() - 1);
    //    }
    //    deserializeJson(swv_doc, cbuf);
    //
    //    String stored_v = swv_doc[swv_key];
    //    if (String(VERSION) != stored_v)
    //    {
    //        swv_doc[swv_key] = VERSION;
    //
    //        serializeJsonPretty(swv_doc, cbuf);
    //        swv_file.println(cbuf);
    //    }
    //
    //    swv_file.close();
    //}


    // Watchdog timer
    //REG_WDT_CONFIG = WDT_CONFIG_PER_CYC1024;    // Set the WDT reset timeout to 1 second
    REG_WDT_CONFIG = WDT_CONFIG_PER_CYC16384;    // Set the WDT reset timeout to 16 second
    REG_WDT_CTRLA = WDT_CTRLA_ENABLE;           // Enable the WDT in normal mode
    while (WDT->SYNCBUSY.bit.ENABLE);           // Wait for synchronization
}



#define logT(x) x += micros() - t1;\
                t1 = micros();

void loop()
{
    // Clear the watchdog timer if the WDT registers are synchronized
    if (!WDT->SYNCBUSY.bit.CLEAR)
        REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;

    
    // variables for calculating excecution time
    unsigned t0 = micros(); // start of loop
    unsigned t1 = micros(); // advances after every log
    static unsigned long long t_program;
    static unsigned long long loopCounter;
    static unsigned long long t_console;
    static unsigned long long t_shelfRead;
    static unsigned long long t_pressRead;
    static unsigned long long t_heatCtl;
    static unsigned long long t_cpuTemp;
    static unsigned long long t_loop;
    static unsigned long long t_touch;

    loopCounter++;


    if (Serial.available())
        Command_Terminal();
    logT(t_console);

    
    // called every loop so we always have good sensor data
    ShelfTemp.AsyncRead();
    ChmbrTemp.AsyncRead();
    logT(t_shelfRead);

    AsyncRead_Pressure();
    logT(t_pressRead);

    // called every loop to prevent thermal runaway
    heating_control();
    logT(t_heatCtl);

    // check the storage file and record highest cpu temp
    cpu_temp();
    logT(t_cpuTemp);

    // The main loop runs by using a switch case to activate and use certain pages by using and changing the variable "page"
    switch (page)
    {
    case  1: homepage();                    hardware_control(0, 0, 0);  break;
    case  2: quickStart();                  hardware_control(0, 0, 0);  break;
    case  3: freeze();                                                  break;
    case  4: dry();                                                     break;
    case  5: freeze_warning();              hardware_control(0, 0, 0);  break;
    case  6: dry_warning();                                             break;
    case  7: pause_dry();                                               break;
    case  8: complete();                                                break;
    case  9: defrost();                                                 break;
    case 10: defaults();                    hardware_control(0, 0, 0);  break;
    case 11: power_lost();                  hardware_control(1, 1, 0);  break;
    case 12: manual_tests();                                            break;
    case 13: save_changes();                hardware_control(0, 0, 0);  break;
    case 14: vacuum_leak_error();                                       break;
    case 16: cooling_error();                                           break;
    case 17: settings_page();               hardware_control(0, 0, 0);  break;
    case 18: recipes();                     hardware_control(0, 0, 0);  break;
    case 19: diagnostics();                 hardware_control(0, 0, 0);  break;
    case 20: test_pages();                                              break;
    case 21: results();                     hardware_control(0, 0, 0);  break;
    case 22: recipe_info();                 hardware_control(0, 0, 0);  break;
    case 23: fatal_sensor_error();                                      break;
    case 24: defaults_1();                  hardware_control(0, 0, 0);  break;
    case 25: defaults_2();                  hardware_control(0, 0, 0);  break;
    case 26: defaults_3();                  hardware_control(0, 0, 0);  break;
    case 27: defaults_4();                  hardware_control(0, 0, 0);  break;
    //case 28: defaults_1_warning();          hardware_control(0, 0, 0);  break;
    case 29: edit_recipe_select();          hardware_control(0, 0, 0);  break;
    case 30: edit_recipe();                 hardware_control(0, 0, 0);  break;
    case 31: remove_food();                 hardware_control(0, 1, 0);  break;
    case 32: freeze_warning_recipes();      hardware_control(0, 0, 0);  break;
    case 33: candy_cooldown();              hardware_control(0, 1, 0);  break;
    case 34: quality_control_test();                                    break;
    case 35: sensor_testing();              hardware_control(1, 0, 1);  break;
    case 36: sensor_failed();               hardware_control(0, 0, 0);  break;
    case 37: qc_test_results();             hardware_control(0, 0, 0);  break;
    case 38: qc_test_pages();                                           break;
    case 39: release_pressure();            hardware_control(0, 0, 0);  break;
    case 40: leak_rate();                   hardware_control(0, 0, 0);  break;
    case 41: display_leak_rate();           hardware_control(0, 0, 0);  break;
    case 42: pre_warm_option();             hardware_control(0, 0, 0);  break;
    case 43: pre_warming_candy();                                       break;
    case 44: qc_test_warning();                                         break;
    case 45: defaults_5();                  hardware_control(0, 0, 0);  break;
    case 46: ground_fault_checker();                                    break;
    case 47: ground_fault_detected();                                   break;
    case 48: checkPressureSensorAccuracy();                             break;
    case 49: defaults_6();                  hardware_control(0,0,0);    break;
    case 50: popUpError();                                              break;
    
    // hopefully we never make it here
    default:

        REG_WDT_CTRLA &= ~WDT_CTRLA_ENABLE; // Disable WDT since we have a lot of delay() here

        Serial.printf("FATAL ERROR: INVALID PAGE ID: %u\n", page);
        delay(1000);
        tft.fillScreen(BACKGROUND);
        draw_line( 90, "FATAL ERROR: INVALID PAGE:", TFT_RED);
        draw_line(130, String(prev_page, 10) + " -> " + String(page, 10), TFT_RED);
        delay(5000);
        setPage(1);

        // re-enable watchdog
        REG_WDT_CTRLA |= WDT_CTRLA_ENABLE;

        return;
    }
    logT(t_loop);


    if (millis() - touch_reset > 10000) {
        ts.begin(40);
        touch_reset = millis();
    } 
    logT(t_touch);
    t_program += micros() - t0;

    /// STATUS PRINTS ///
    static unsigned nextDegugPrint  =  0;
    static unsigned lastPrintedPage = -1;
    
    if (millis() > nextDegugPrint || page != lastPrintedPage)
    {
        lastPrintedPage = page;
        nextDegugPrint += 1000;

        t_program   /= loopCounter;
        t_loop      /= loopCounter;
        t_shelfRead /= loopCounter;
        t_pressRead /= loopCounter;
        t_console   /= loopCounter;
        t_heatCtl   /= loopCounter;
        t_cpuTemp   /= loopCounter;
        t_touch     /= loopCounter;

        static char timePacket[24];
        static char timePress[24];
        Time(TimeSinceLastBufferPacket / 1000).toString(timePacket);
        Time(TimeSinceLastValidPresure / 1000).toString(timePress);

        bool rtcEn = RTC->MODE2.CTRLA.reg & RTC_MODE2_CTRLA_ENABLE;

        /*
        Serial.printf("\n\n\n");
        Serial.printf("Blue Alpine Verison %s", VERSION);
#ifdef EXPERIMENTAL
        Serial.println("E");
#else
        Serial.println();
#endif
        Serial.printf("Status\n");
        Serial.printf("   Page       %i (%i)\n", page, prev_page);
        Serial.printf("   RTC Time  %s%s%s\n", (rtcEn ? " " : "("), CycleTime.toString().c_str(), (rtcEn ? "" : ")"));
        Serial.printf("   Remaining  %s\n", remaining.toString().c_str());

        Serial.printf("Pressure Sensor\n");
        Serial.printf("   Current pressure kPa    %.3f\n", chamber_pressure * 0.13332237 / 1000);
        Serial.printf("   Current Pressure Rate   %.2f%%\n", delta_pressure * 100);
        Serial.printf("   Time since last packet  %s.%04u\n", timePacket, TimeSinceLastBufferPacket % 1000);
        Serial.printf("   Time since last data    %s.%04u\n", timePress,  TimeSinceLastValidPresure % 1000 );

        Serial.printf("Shelf Sensor\n");
        Serial.printf("   T in Celsius  %.2f\n", ShelfTemp.Celsius);
        Serial.printf("   Uptime        "); Serial.println(Time(abs(ShelfTemp.UpTime) / 1000).toString());
        Serial.printf("   Success Rate  %.1f%%\n", ShelfTemp.SuccessRate * 100);
        Serial.printf("   Fault code    0x%02X\n", ShelfTemp.Fault);

        Serial.printf("Chamber Sensor\n");
        Serial.printf("   T in Celsius  %.2f\n", ChmbrTemp.Celsius);
        Serial.printf("   Uptime        "); Serial.println(Time(abs(ChmbrTemp.UpTime) / 1000).toString());
        Serial.printf("   Success Rate  %.1f%%\n", ChmbrTemp.SuccessRate * 100);
        Serial.printf("   Fault code    0x%02X\n", ChmbrTemp.Fault);

        Serial.printf("Hardware\n");
        Serial.printf("   Heater  %s\n", digitalRead(TRIAC)  ? "ON" : "OFF");
        Serial.printf("   Fan     %s\n", digitalRead(FAN)    ? "ON" : "OFF");
        Serial.printf("   Fridge  %s\n", digitalRead(FRIDGE) ? "ON" : "OFF");
        Serial.printf("   Vacuum  %s\n", digitalRead(VAC)    ? "ON" : "OFF");

        Serial.printf("Other\n");
        Serial.printf("   done              %s\n", done ? "TRUE" : "FALSE");
        //Serial.printf("   set_pressure      %f (%f, %f)\n", set_pressure, double(200 + OFFSET), double(80 + OFFSET));
        //Serial.printf("   c_recipe.press    %f (%f, %f)\n", current_recipe.pressure, double(200 + OFFSET), double(80 + OFFSET));
        Serial.printf("   Watchdog Timer    %s\n", REG_WDT_CTRLA & WDT_CTRLA_ENABLE ? "ENABLED" : "DISABLED");
        Serial.printf("   ForceHeater       %s%s\n",ForceHeaterON ? "ON" : "", ForceHeaterOFF ? "OFF" : "");
        Serial.printf("   qc_test_num       %i\n", Tests.qc_test_num);
        Serial.printf("   test_num          %i\n", test_num);
        Serial.printf("   LeakOnly          %s\n", Tests.leak_only ? "TRUE" : "FALSE");
        Serial.printf("   I2C Timeout       %i\n", Wire.getTimeout());

        Serial.printf("Excecution Time (ms)\n");
        Serial.printf("   Program  %5.2f\n", t_program / 1000.0f);
        //Serial.printf("   GUI              %5.2f\n", t_loop / 1000.0f);
        //Serial.printf("   Pead pressure    %5.2f\n", t_pressRead / 1000.0f);
        //Serial.printf("   Read temperature %5.2f\n", t_shelfRead / 1000.0f);
        //Serial.printf("   Other            %5.2f\n", (t_heatCtl + t_cpuTemp + t_touch + t_console) / 1000.0f);
        Serial.printf("   Print    %5.2f\n", (micros() - t1) / 1000.0f);
        */

        t_program   = 0;
        t_loop      = 0;
        t_shelfRead = 0;
        t_pressRead = 0;
        t_console   = 0;
        t_heatCtl   = 0;
        t_cpuTemp   = 0;
        t_touch     = 0;
        loopCounter = 0;
    } 

    //Serial.print("Defrost Fan Status: "); Serial.println(defrost_fan);
    /// DEBUG BOX ///
    // see CommandTerminal() for details

    if (DebugBox.x0 || DebugBox.y0)
    {
        if (DebugBox.x1 == 0 && DebugBox.y1 == 0)
        {
            tft.drawFastVLine(DebugBox.x0, 0, 320, random());
            tft.drawFastHLine(0, DebugBox.y0, 460, random());
        }
        else
            tft.drawRect(DebugBox.x0, DebugBox.y0, DebugBox.x1, DebugBox.y1, random());
    }
}
