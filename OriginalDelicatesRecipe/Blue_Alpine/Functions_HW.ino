/*********************************************************************************

This file contains our hardware functions. These functions are used to run the actual hardware of the system.  

Functions in this section are:

AsyncRead_Pressure: Used to interact with the bufferboard using Uart (serial) communication. We use this function to recive the pressure data from the buffer board.
             The buffer board communicates with the pressure sensor using I2C. This function also ocasionaly resets the buffer board. 

hardware_control: This function is used to activate the hardware of the system. This function turns on and off the fan, fridge, vacuum, and heating shelves(triac).
                  To use this function just call it passing in bools.  hardware_control(fan, fridge, vacuum, shelves(triac))   

heating_control: Turns on PID Controls. USED Only by the dry, defrost, candy prewarm, and manual control page. The PID controls manages and controls
                 the heating shelves for better preformance

temp_read: Reads temperature data using timed delays to ensure the system isn't slowed down by temperature reads

cooling_checker: Checks for the cooling capacity of the refrigeration system and the temperature sensor functionality.
                 When a used sensor is disconnected at any time during the cycle it will be detected    

pressure_checker: Checks for the pressure level during the dry cycle. Must be below 600 to ensure proper freeze drying capability. A limit of 500Pa is used.

sensor_check: Checks the parameters which determine that sensors have been disconnected

hardware_serial_monitor: Hardware serial monitor that displays the cycle data to the serial monitor

store_elapsed: Store data for time-on time

get-elapsed: Gets the total time that the refrigeration hardware is turned on from memory

triac_visual: Test function. Alerts users to see when the heating pads are turned on.

cpu_temp: keep checking internal cpu hottest temprature. Store the hottest temperature in qspi flash

memory_check: Checks for stored items at startup and loads them as defaults. Everything is stored as an int. Default values are loaded if memory is invalid.

reset_values: Resets the values in memory with the values stored in QSPI flash

store_values: Store default values into flash filesystem

store_status: Store Status values

store-recipe_values: Store default recipe values into flash filesystem

store_data: Store cycle hardware data as file on flash filesystem

file-checker: Checks for available filename for use with store_data() function

logging: Stores cycle data values to file

generate_files: Generates files for storage if none are present. (Storage wipe, accidental file deletion etc)

test_values: Assigns fake sensor data values to variables. Used for testing cycle processes and hardware issues

void set_pixel_blue: Turns the LED blue

void set_pixel_green: Turns the LED green

void set_pixel_yellow: Turns the LED yellow

void set_pixel_red: Turns the LED red

void set_pixel_purple: Turns the LED purple

void set_pixel_white: Turns the LED white


**********************************************************************************/



void AsyncRead_Pressure()
{
    const float Pa_CONVERSION_FACTOR = 0.1333224;  // multiply raw reading by this to get pressure in pascals
    const uint32_t RESET_DELAY_ms = 5 * 1000;  // how much extra time to give the buffer for reset
    const uint32_t PATIENCE_LIMIT_ms = 2 * 1000;  // maximum time between polls before we throw an error
    static unsigned timeOfLastRestart = 0;

    // We mught get data from the buffer that contains invalud sensor data. In that case, we want to remember that the buffer is connected even if we don't use the data
    uint32_t now = millis();


    // only request data when 4 bytes are available to pull
    if (mySerial.available() >= sizeof(uint32_t))
    {
        TimeOfLastBufferPacket = now;

        uint32_t reading = mySerial.parseInt();

        // clear the buffer
        while (mySerial.available())
            mySerial.read();

        if (reading == 760000)
        {
            Serial.println("Vaccum sensor error: Buffer board returned 760000 (0xFFFF compressed). The sensor is likley dissonnected from the buffer board.");
            Serial.printf("T: %i    L: %i    N: %i\n", TimeSinceLastValidPresure, TimeOfLastPressureRead, now);
        }
        else if (reading < 4)  // we assume we can only measure pressure this low if we're experiencing integer-wrap
        {
            //VacSensorStatus = VacSensorStatus::VALUE_TOO_SMALL;
            Serial.print("Vacuum Sensor Error: Reading < 30 Pa: ");
            Serial.println(reading);

        }
        else  // no error
        {
            double prev_pressure = pressure_;
            chamber_pressure = reading;
            pressure_ = reading * Pa_CONVERSION_FACTOR;

            /// TODO: Filter
            delta_pressure = (pressure_ - prev_pressure) / prev_pressure / (now - TimeOfLastPressureRead) * 1000;

            TimeOfLastPressureRead = now;
        }
    }


    TimeSinceLastValidPresure = TimeOfLastPressureRead >= now ? 0 : now - TimeOfLastPressureRead;
    TimeSinceLastBufferPacket = TimeOfLastBufferPacket >= now ? 0 : now - TimeOfLastBufferPacket;

    if (TimeSinceLastValidPresure == 0)
    {
        PressureTimeOffset = 0;
    }
    else
    {
        TimeSinceLastValidPresure += PressureTimeOffset;
        TimeSinceLastBufferPacket += PressureTimeOffset;

        if (TimeSinceLastValidPresure < 0) TimeSinceLastValidPresure = 0;
        if (TimeSinceLastBufferPacket < 0) TimeSinceLastBufferPacket = 0;
    }


    // keep track of uptime
    if (TimeSinceLastValidPresure > PRES_INV_TIMEOUT_STRICT_ms)
    {
        PressureReconnectTime = 0;
    }
    else if (PressureReconnectTime == 0)
    {
        PressureReconnectTime = now;
    }


    // restart buffer uif needed
    if (TimeSinceLastValidPresure > PATIENCE_LIMIT_ms && now - timeOfLastRestart > 10000)
    {


        if (TimeSinceLastValidPresure > 30 * 1000 || pressure_ < 15)
        {
            Serial.println("Restarting Buffer Board");
            digitalWrite(Buffer_Reset, LOW);
            delay(50);
            digitalWrite(Buffer_Reset, HIGH);
            delay(50);


            // add an extra delay to allow the sensor time to initialize
            // without this, we poll too quickly and get a false error which prompts another reset
            // basically, we need to be more patient.
            timeOfLastRestart = now;
        }
    }
}



/*
 * USED by every page to ensure that the hardware is in
 * the correct states 
 */
void hardware_control(bool fan, bool fridge, bool vac)
{
    static int currMillisHard;
    static int prevMillisHard;
    if(!done)
    {
      currMillisHard = millis();
      prevMillisHard = millis();
    }
    if(fan == 0)
    {
      digitalWrite(FAN, fan);                                       
    }
    if(fridge == 0)
    {
      digitalWrite(FRIDGE, fridge);
    }
    if(vac == 0)
    {
      digitalWrite(VAC, vac);
    }
    currMillisHard = millis();
    bool currStatusFridge = digitalRead(FRIDGE);
    bool currStatusVac = digitalRead(VAC);
    bool currStatusFan = digitalRead(FAN);
    if((fan == currStatusFan) && (fridge == currStatusFridge) && (vac == currStatusVac))
    {
      digitalWrite(FRIDGE, fridge);  // turns fridge on or off depending on input
      digitalWrite(FAN, fan);  // turns fan on or off depending on input                                        
      digitalWrite(VAC, vac);  // turns vacuum on or off depending on input
    }
    else
    {
      if(currMillisHard - prevMillisHard >= 3000)
      {
        if(fridge != currStatusFridge)
        {
          digitalWrite(FRIDGE, fridge);
        }
        else if(vac != currStatusVac)
        {
          digitalWrite(VAC, vac);
        }
        else if(fan != currStatusFan)
        {
          digitalWrite(FAN, fan);
        }
        prevMillisHard = millis();
      }
    }
}

/*
 * Turns on PID Controls
 * USED Only by the dry page
 */
void heating_control()
{
  // in case of thermal runaway, sensor fault, ect, disable heater and abort
    if (standardSensorCheck_pressure()
     || ShelfTemp.Celsius > 75
     || (ShelfTemp.Fault && (ChmbrTemp.Fault || ChmbrTemp.Celsius > 75))
     || ForceHeaterOFF
     || page == 23
    ) {
        digitalWrite(TRIAC, LOW);
        return;
    }

    switch (page)
    {
  // dry page
    case 4:
        if (ShelfTemp.Celsius > ShelfTargetTemperature)
        {
            digitalWrite(TRIAC, LOW);
        }
        else if (recipe_cycle && recipe_sel == 1)
        {
            candy_dry_relay.run();
            digitalWrite(TRIAC, relay_8);
        }
        else
        {
            pid_relay.run();
            digitalWrite(TRIAC, relay);
        }
        break;

  // comeplete page
    case 8:
        //Serial.print("Complete Temp: "); Serial.println(complete_temp);
        if (ShelfTemp.Celsius < complete_temp)
        {
            complete_relay.run();
            digitalWrite(TRIAC, relay_5);
        }
        else digitalWrite(TRIAC, LOW);

        break;

  // defrost page
    case 9:
  // run the Defrost PID heating control and heat pads to 150F
        if (ShelfTemp.Celsius < defrost_temp)
        {
            defrost_relay.run();
            digitalWrite(TRIAC, relay_2);
        }
        else
            digitalWrite(TRIAC, LOW);

        break;

  // manual pages
    case 12:
    case 20:
    case 38:
    case 46:
  // run the Manual PID heating control and heat pads to 120F
        if (ForceHeaterON && ShelfTemp.Celsius < manual_temp)
        {
            manual_relay.run();
            digitalWrite(TRIAC, relay_6);
        }
        else
            digitalWrite(TRIAC, LOW);

        break;

    case 43:
  // activates the heating triac for pre-warming page
        if (ShelfTemp.Celsius < candy_pre_warm_temp)
        {
            candy_pre_warm_relay.run();
            digitalWrite(TRIAC, relay_7);
        }
        else
            digitalWrite(TRIAC, LOW);

        break;
    case 35:
        if (ForceHeaterON && ShelfTemp.Fahrenheit < 80)
            {
                manual_relay.run();
                digitalWrite(TRIAC, relay_6);
            }
            else
                digitalWrite(TRIAC, LOW);

            break;


    default:
        digitalWrite(TRIAC, LOW);
        break;
    }
}



 // Checks for the cooling capacity of the refrigeration system and the temperature sensor functionality. When a used sensor is  disconnected at any time during the cycle it will be detected 
bool cooling_checker(Time timeout, int temp, bool shelfSensorBackup = false)
{
    if (IgnoreChmbrTemp) return false;  // if override is checked then do not throw an error  

    bool faultDetected = ChmbrTemp.Fault;
    double degC = ChmbrTemp.Celsius;

    // if the chamber sensor is out, try using the shelf sensor
    if (faultDetected && shelfSensorBackup)
    {
        faultDetected = ShelfTemp.Fault;
        degC = ShelfTemp.Celsius;
    }

    // checks if the chamber is cold enough after the specified time
    if (CycleTime > timeout && degC > temp && !faultDetected)
        return true;

    return false;  // if temperature is within range simply return
}


// returns the amount of time that has passed since the monitored pressure was within the specified range
// use negative values (or no values) to keep the last-used values
unsigned long time_pressure_went_invalid;

unsigned long inval_press_time(int lowerBound = -1, int upperBound = -1)
{

    //static unsigned long time_pressure_went_invalid;

    static double _lowerBound;
    static double _upperBound;


    if (lowerBound >= 0) _lowerBound = lowerBound;
    if (upperBound >= 0) _upperBound = upperBound;

    if (pressure_ < _lowerBound || pressure_ > _upperBound)
    {
        if (!time_pressure_went_invalid)
        {
            time_pressure_went_invalid = millis();
        }

        return millis() - time_pressure_went_invalid;
    }
    else
    {
        time_pressure_went_invalid = 0;
        return 0;
    }
}
// resets the counter by setting the bounds such that the current pressure cannot be out of range
void reset_inval_press_time() { inval_press_time(0, INT_MAX); }


// Positive if invalid; negative if valid
int time_delta_pressure_went_invalid;

int inval_press_rate_time(double lowerBound, double upperBound)
{
    const int LATCH_TIME_MS = 5000;

    //static int nextPrint = 0;
    //static int time_delta_pressure_went_invalid;
    static int time_delta_pressure_went_valid;

    int dt = 0;


  // reset by forcing dt > LATCH_TIME_MS
    if (lowerBound == 0 && upperBound == 0)
    {
        time_delta_pressure_went_valid   = -LATCH_TIME_MS;
        time_delta_pressure_went_invalid = -LATCH_TIME_MS;
    }


    if (delta_pressure < lowerBound || delta_pressure > upperBound)
    {
        if (!time_delta_pressure_went_invalid)
            time_delta_pressure_went_invalid = millis();

        dt = millis() - time_delta_pressure_went_invalid;
        
        if (dt > LATCH_TIME_MS)
         time_delta_pressure_went_valid = 0;

        //if(millis() > nextPrint)
        //{
        //    nextPrint += 1000;

        //    Serial.print("Change in Pressure out of bounds. Range is: ");
        //    Serial.print(lowerBound);
        //    Serial.print("-");
        //    Serial.print(upperBound);
        //    Serial.print("   Time: ");
        //    print_ms(millis() - time_delta_pressure_went_invalid);
        //    Serial.println();
        //}

        return dt;
    }
    else
    {
        if (!time_delta_pressure_went_valid)
            time_delta_pressure_went_valid = millis();

        dt = millis() - time_delta_pressure_went_valid;
        
        if (dt > LATCH_TIME_MS)
         time_delta_pressure_went_invalid = 0;


        //if (millis() > nextPrint)
        //{
        //    nextPrint += 1000;
        //    Serial.println("Pressure okay.");
        //}

        return -dt;
    }
}
// resets the counter by setting the bounds such that the current pressure cannot be in range
void reset_inval_press_rate_time() { inval_press_rate_time(0, 0); }

//store data for time-on time
void store_elapsed(){
  
  int elapsed_hr, elapsed_day, elapsed_min; Time tally;  // create my variables  
  Time elapsed = CycleTime;  // save whatever time is elapsed by the rtc  
  get_elapsed(&elapsed_hr, &elapsed_day, &elapsed_min);  // get elapsed prev values  
  Time prev_total(elapsed_day, elapsed_hr, elapsed_min, 0);  // tally up the time
  tally = prev_total + elapsed;
  
  char cont[128];  // containers for json
  StaticJsonDocument<200> elapse;
  
  elapse["hr"]  = (unsigned)tally.Hours;  // Add values
  elapse["min"] = (unsigned)tally.Minutes;
  elapse["day"] = (unsigned)tally.Days;
  
  File time_file = fatfs.open("on_time.txt", O_RDWR);  // open and write to file
  serializeJsonPretty(elapse, cont); 
  time_file.println(cont);  // open and write to file
  time_file.close();
}

/*
 * Gets the total time that the refrigeration hardware is turned on from memory
 */
void get_elapsed(int *elapsed_hr, int *elapsed_day, int *elapsed_min){
  
  StaticJsonDocument<200> elapsed;  // JSON container for values
  char buf[128];    
  File time_file = fatfs.open("on_time.txt",  FILE_READ);  // Parse Json and set variables
    
  if(time_file){  // Read the file
    int rlen = time_file.available();
    time_file.read(buf, rlen - 1);  // Read to buffer
  }  
  deserializeJson(elapsed, buf);  // deserialize json file
  
  *elapsed_day = elapsed["day"];  // assign to variables
  *elapsed_hr = elapsed["hr"];
  *elapsed_min = elapsed["min"];  
}


  //keep checking internal cpu hottest temprature
  //store the hottest temperature in qspi flash
void cpu_temp(){  

  static int update_millis;  // variables to be used 
  static float temperature;
  float new_temp;
  char buf[128];
  char temp[128];
  StaticJsonDocument<512> doc;
  
  if(millis() > update_millis + 1000*60){  // check the internal cpu temperature every minute
    new_temp = TempZero.readInternalTemperature();  // Read temperature
  //Serial.print("Internal Temperature is : "); // Print the temperature to serial monitor
  //Serial.println(temperature);
      
    File temp_file = fatfs.open("max_temp.txt", FILE_READ);  // get data from the file to compare
    if(temp_file){
      int rlen = temp_file.available();  // Read file to buffer
      temp_file.read(buf, rlen - 1); 
    }
    temp_file.close();  // close file
    deserializeJson(doc, buf);  // deserialize data to get previously stored temperature
    temperature = doc["temp"];                                     
    if(new_temp > temperature){  // compare temperatures
      File file = fatfs.open("max_temp.txt", O_RDWR);  // if new temperature is higher then overwrite stored value
      doc["temp"] = new_temp;
      serializeJsonPretty(doc, temp);
      file.println(temp);
      file.close();
    }    
    update_millis = millis();
  }
}


/*
 * Checks for stored items at startup and loads them as defaults
 * Everything is stored as an int. Default values are loaded if 
 * memory is invalid.
 */
void memory_check()
{
    char buf[1024];
    StaticJsonDocument<512> doc;

    File conf_file = fatfs.open("config.txt", FILE_READ);

    if (conf_file)
    {
        // Read the file
        int rlen = conf_file.available();
        conf_file.read(buf, rlen - 1);
    }

    deserializeJson(doc, buf);

    // assign to variables
    default_dry_temp = doc["stemp"];
    default_freeze_hour = doc["fhour"];
    default_freeze_min = doc["fmin"];
    default_dry_hour = doc["dhour"];
    default_dry_min = doc["dmin"];
    temp_unit_s = doc["tunit"];
    pressure_unit_s = doc["punit"];
    freeze_day = doc["fday"];
    dry_day = doc["dday"];
    bsound = doc["button"];
    csound = doc["chyme"];
    s_pressure = doc["pressure"];
    d_fan = doc["d_fan"];
    c_vac = doc["c_vac"];

    conf_file.close();

    // Check Recipe Values
    StaticJsonDocument<512> doc1;
    conf_file = fatfs.open("recipes.txt", FILE_READ);

    if (conf_file) {  // Read the file
        int rlen = conf_file.available();
        conf_file.read(buf, rlen - 1);  // Read to buffer
    }

    deserializeJson(doc1, buf);

    Candy = { doc1["c_ftime"], doc1["c_dtime"], doc1["c_pressure"], doc1["c_temp"] };
    Greens = { doc1["g_ftime"], doc1["g_dtime"], doc1["g_pressure"], doc1["g_temp"] };
    Meat = { doc1["m_ftime"], doc1["m_dtime"], doc1["m_pressure"], doc1["m_temp"] };
    Fruit = { doc1["f_ftime"], doc1["f_dtime"], doc1["f_pressure"], doc1["f_temp"] };
    Liquid = { doc1["l_ftime"], doc1["l_dtime"], doc1["l_pressure"], doc1["l_temp"] };
    Delicates = { doc1["d_ftime"], doc1["d_dtime"], doc1["d_pressure"], doc1["d_temp"] };
    DelicatesSecondary = {doc1["ds_ft"], doc1["ds_dt"], doc1["ds_pres"], doc1["ds_temp"]};
    conf_file.close();

    // Check power status
    StaticJsonDocument<512> doc2;
    conf_file = fatfs.open("status.txt", FILE_READ);
    
    // Read the file
    if (conf_file)
    {  
        int rlen = conf_file.available();
        conf_file.read(buf, rlen - 1);  // Read to buffer
    }
    deserializeJson(doc2, buf);

    freeze_cycle_status = doc2["f_power"];
    dry_cycle_status = doc2["d_power"];
    complete_cycle_status = doc2["c_power"];
    recipe_cycle = doc2["r_cycle"];
    recipe_sel = doc2["r_select"];

    // For ground fault checker
    fridge_fault_status = doc2["fridge_status"];
    vacuum_fault_status = doc2["vacuum_status"];
    fan_fault_status = doc2["fan_status"];
    heat_fault_status = doc2["heat_status"];


    conf_file.close();
}


/*
 * Resets the values in memory with the values stored
 * in QSPI flash
 */
void reset_values(){  
  
  memory_check();  // get memory values

  if (recipe_cycle == true){
    switch(recipe_sel){
      case 1:
        current_recipe = Candy;
        recipe_dry_time = Time(current_recipe.dry_time);
        recipe_freeze_time = Time(current_recipe.freeze_time);

        temp_unit = (bool)temp_unit_s;
        pressure_unit = pressure_unit_s;
        button_sound = bsound;
        cycle_sound = csound;
        defrost_fan = d_fan;
        is_candy = true;
        completeVaccumOnAlways = c_vac;
        break;

      case 2:
        current_recipe = Greens;
        recipe_dry_time = Time(current_recipe.dry_time);
        recipe_freeze_time = Time(current_recipe.freeze_time);

        temp_unit = (bool)temp_unit_s;
        pressure_unit = pressure_unit_s;
        button_sound = bsound;
        cycle_sound = csound;
        defrost_fan = d_fan;
        completeVaccumOnAlways = c_vac;
        break;

      case 3:
        current_recipe = Meat;
        recipe_dry_time = Time(current_recipe.dry_time);
        recipe_freeze_time = Time(current_recipe.freeze_time);

        temp_unit = (bool)temp_unit_s;
        pressure_unit = pressure_unit_s;
        button_sound = bsound;
        cycle_sound = csound;
        defrost_fan = d_fan;
        completeVaccumOnAlways = c_vac;
        break;

      case 4:
        current_recipe = Fruit;
        recipe_dry_time = Time(current_recipe.dry_time);
        recipe_freeze_time = Time(current_recipe.freeze_time);

        temp_unit = (bool)temp_unit_s;
        pressure_unit = pressure_unit_s;
        button_sound = bsound;
        cycle_sound = csound;
        defrost_fan = d_fan;
        completeVaccumOnAlways = c_vac;
        break;

      case 5:
        current_recipe = Liquid;
        recipe_dry_time = Time(current_recipe.dry_time);
        recipe_freeze_time = Time(current_recipe.freeze_time);

        temp_unit = (bool)temp_unit_s;
        pressure_unit = pressure_unit_s;
        button_sound = bsound;
        cycle_sound = csound;
        defrost_fan = d_fan;       
        completeVaccumOnAlways = c_vac;
        break;

      case 6:
        current_recipe = Delicates;
        recipe_dry_time = Time(current_recipe.dry_time);
        recipe_freeze_time = Time(current_recipe.freeze_time);

        temp_unit = (bool)temp_unit_s;
        pressure_unit = pressure_unit_s;
        button_sound = bsound;
        cycle_sound = csound;
        defrost_fan = d_fan;
        completeVaccumOnAlways = c_vac;
        break;
      
      case 7:
        current_recipe = DelicatesSecondary;
        recipe_dry_time = Time(current_recipe.dry_time);
        recipe_freeze_time = Time(current_recipe.freeze_time);

        temp_unit = (bool)temp_unit_s;
        pressure_unit = pressure_unit_s;
        button_sound = bsound;
        cycle_sound = csound;
        defrost_fan = d_fan;
        completeVaccumOnAlways = c_vac;
        break;

      default: break;
    }
  }
  else{
    ShelfTargetTemperature = default_dry_temp;  // assign defaults from memory function 
    temp_unit = (bool)temp_unit_s;
    pressure_unit = pressure_unit_s;
    freeze_time = Time(freeze_day, default_freeze_hour, default_freeze_min, 0);
    dry_time = Time(dry_day, default_dry_hour, default_dry_min, 0);  
    button_sound = bsound;
    cycle_sound = csound;
    set_pressure = s_pressure;
    defrost_fan = d_fan;
    completeVaccumOnAlways = c_vac;
  }
}

/*
 * Store default values into flash filesystem
 */
void store_values(){  
  
  char cont[1024];  // container for json  
  StaticJsonDocument<500> doc;  // create new Json document

  Serial.println("Setting Default Values");

  doc["fhour"] = (unsigned)freeze_time.Hours;
  doc["fmin"] = (unsigned)freeze_time.Minutes;
  doc["dhour"] = (unsigned)dry_time.Hours;
  doc["dmin"] = (unsigned)dry_time.Minutes;
  doc["punit"] = (unsigned)pressure_unit;
  doc["tunit"] = (unsigned)temp_unit;
  doc["dday"] = (unsigned)dry_time.Days;
  doc["fday"] = (unsigned)freeze_time.Days;
  doc["stemp"] = (unsigned)ShelfTargetTemperature;
  doc["pressure"] = (unsigned)set_pressure;
  doc["button"] = (unsigned)button_sound;
  doc["chyme"] = (unsigned)cycle_sound;
  doc["d_fan"] = (unsigned)defrost_fan;
  doc["c_vac"] = (unsigned)completeVaccumOnAlways;

  serializeJsonPretty(doc, cont);  // serialize  
  File config_file = fatfs.open("config.txt", O_RDWR);  // open and write to file

  //Serial.println(cont); // print data to serial monitor
  config_file.println(cont);  // write to file
  config_file.close();  // close file  
}

/*
 *  Store Status values
 */
void store_status(){
  char cont[1024];  // container for json  
  StaticJsonDocument<500> doc;  // create new Json document
  
  doc["f_power"] = (unsigned)freeze_cycle_status;
  doc["d_power"] = (unsigned)dry_cycle_status;
  doc["c_power"] = (unsigned)complete_cycle_status;
  doc["r_cycle"] = (unsigned)recipe_cycle;
  doc["r_select"] = (unsigned)recipe_sel;

  doc["fridge_status"] = (unsigned)fridge_fault_status;  // Ground Fault Detection
  doc["vacuum_status"] = (unsigned)vacuum_fault_status;
  doc["fan_status"] = (unsigned)fan_fault_status;
  doc["heat_status"] = (unsigned)heat_fault_status;

  serializeJsonPretty(doc, cont);  // serialize  
  File config_file = fatfs.open("status.txt", O_RDWR);  // open and write to file

  //Serial.println(cont); // print data to serial monitor
  config_file.println(cont);  // write to file
  config_file.close();  // close file 
}
/*
 * Store default recipe values into flash filesystem
 */
void store_recipe_values(){  
  
  char cont[1024];  // container for json  
  StaticJsonDocument<500> doc;  // create new Json document
  
  doc["c_ftime"] = (unsigned)Candy.freeze_time;  // Add values
  doc["c_dtime"] = (unsigned)Candy.dry_time;
  doc["c_temp"] = (unsigned)Candy.dry_temp;
  doc["c_pressure"] = (unsigned)Candy.pressure;
  doc["g_ftime"] = (unsigned)Greens.freeze_time;  // Add values
  doc["g_dtime"] = (unsigned)Greens.dry_time;
  doc["g_temp"] = (unsigned)Greens.dry_temp;
  doc["g_pressure"] = (unsigned)Greens.pressure;
  doc["m_ftime"] = (unsigned)Meat.freeze_time;  // Add values
  doc["m_dtime"] = (unsigned)Meat.dry_time;
  doc["m_temp"] = (unsigned)Meat.dry_temp;
  doc["m_pressure"] = (unsigned)Meat.pressure;
  doc["f_ftime"] = (unsigned)Fruit.freeze_time;  // Add values
  doc["f_dtime"] = (unsigned)Fruit.dry_time;
  doc["f_temp"] = (unsigned)Fruit.dry_temp;
  doc["f_pressure"] = (unsigned)Fruit.pressure;
  doc["l_ftime"] = (unsigned)Liquid.freeze_time;  // Add values
  doc["l_dtime"] = (unsigned)Liquid.dry_time;
  doc["l_temp"] = (unsigned)Liquid.dry_temp;
  doc["l_pressure"] = (unsigned)Liquid.pressure;
  doc["d_ftime"] = (unsigned)Delicates.freeze_time;  // Add values
  doc["d_dtime"] = (unsigned)Delicates.dry_time;
  doc["d_temp"] = (unsigned)Delicates.dry_temp;
  doc["d_pressure"] = (unsigned)Delicates.pressure;

  doc["ds_ft"] = (unsigned)DelicatesSecondary.freeze_time;  // Add values
  doc["ds_dt"] = (unsigned)DelicatesSecondary.dry_time;
  doc["ds_temp"] = (unsigned)DelicatesSecondary.dry_temp;
  doc["ds_pres"] = (unsigned)DelicatesSecondary.pressure;

  
  serializeJsonPretty(doc, cont);  // serialize  
  File config_file = fatfs.open("recipes.txt", O_RDWR);  // open and write to file

  //Serial.println(cont); // print data to serial monitor
  config_file.println(cont);  // write to file
  config_file.close();  // close file  
}

/*
 * Store cycle hardware data as file on flash filesystem
 */ 
/*
void store_data(double test_minutes, bool dry_){

  String freeze_cycle_title, dry_cycle_title, defrost_title, manual_page_title, test_page_title, elapsed_time_title,
   chamber_temp_title, shelf_temp_title, chamber_press_title, triac_title, relay_title, relay_2_title, relay_3_title,
    relay_4_title, relay_state, relay_2_state, relay_3_state, relay_4_state, elapsed, c_temp, s_temp, c_press, triac_state, store;  //temporary variables to store current data


  freeze_cycle_title = "FREEZE";  // Value Titles that will be written into the csv file
  dry_cycle_title = "DRY";
  defrost_title = "DEFROST";
  manual_page_title = "MANUAL";
  test_page_title = "TEST_PAGE";
  elapsed_time_title = "ELAPSED_TIME";
  chamber_temp_title = "CHAMBER_TEMP";
  shelf_temp_title = "SHELF_TEMP";
  chamber_press_title = "PRESSURE";
  triac_title = "TRIAC_STATE";

  relay_title = "RELAY_STATE";  // Value Titles that will be written into the csv file  
  relay_2_title = "RELAY_2_STATE";
  relay_3_title = "RELAY_3_STATE";
  relay_4_title = "RELAY_4_STATE";

  elapsed = String(test_minutes, 3);  // cast data numbers to Strings
  c_temp = String(chamber_temp_c, 1);
  s_temp = String(shelf_temp_c, 1);
  c_press = String(pressure_, 1);
  triac_state = String(digitalRead(TRIAC));

  //Serial.print("Triac: "); // Serial printouts for testing
  //Serial.println(triac_state);

  relay_state = String(relay);
  relay_2_state = String(relay_2);
  relay_3_state = String(relay_3);
  relay_4_state = String(relay_4);

  /*
  Serial.print("Relay: ");  // Serial printouts for testing
  Serial.println(relay_state);

  Serial.print("Relay 2: ");
  Serial.println(relay_2_state);

  Serial.print("Relay 3: ");
  Serial.println(relay_3_state);

  Serial.print("Relay 4: ");
  Serial.println(relay_4_state);
  
    
  if(page == 4) store = dry_cycle_title + "," + elapsed_time_title + "," + elapsed + "," + chamber_temp_title + "," + c_temp + "," + shelf_temp_title + ","
   + s_temp + "," + chamber_press_title + "," + c_press + "," + triac_title + "," + triac_state + "," + relay_title + "," + relay_state + "," 
   + relay_2_title + "," + relay_2_state + "," + relay_3_title + "," + relay_3_state + "," + relay_4_title + "," + relay_4_state + "\n";  // if on the dry page, merge the strings to be logged 

  else if(page == 3) store = freeze_cycle_title + "," + elapsed_time_title + "," + elapsed + "," + chamber_temp_title + "," + c_temp + "," + shelf_temp_title + ","
   + s_temp + "," + chamber_press_title + "," + c_press + "," + triac_title + "," + triac_state + "," + relay_title + "," + relay_state + "," 
   + relay_2_title + "," + relay_2_state + "," + relay_3_title + "," + relay_3_state + "," + relay_4_title + "," + relay_4_state + "\n";  // if on the freeze page, merge the strings to be logged 

  else if(page == 9) store = defrost_title + "," + elapsed_time_title + "," + elapsed + "," + chamber_temp_title + "," + c_temp + "," + shelf_temp_title + ","
   + s_temp + "," + chamber_press_title + "," + c_press + "," + triac_title + "," + triac_state + "," + relay_title + "," + relay_state + "," 
   + relay_2_title + "," + relay_2_state + "," + relay_3_title + "," + relay_3_state + "," + relay_4_title + "," + relay_4_state + "\n";  // if on the defrost page, merge the strings to be logged 

  else if(page == 12) store = manual_page_title + "," + elapsed_time_title + "," + elapsed + "," + chamber_temp_title + "," + c_temp + "," + shelf_temp_title + ","
   + s_temp + "," + chamber_press_title + "," + c_press + "," + triac_title + "," + triac_state + "," + relay_title + "," + relay_state + "," 
   + relay_2_title + "," + relay_2_state + "," + relay_3_title + "," + relay_3_state + "," + relay_4_title + "," + relay_4_state + "\n";  // if on the manual page, merge the strings to be logged 

  else if(page == 20) store = test_page_title + "," + elapsed_time_title + "," + elapsed + "," + chamber_temp_title + "," + c_temp + "," + shelf_temp_title + ","
   + s_temp + "," + chamber_press_title + "," + c_press + "," + triac_title + "," + triac_state + "," + relay_title + "," + relay_state + "," 
   + relay_2_title + "," + relay_2_state + "," + relay_3_title + "," + relay_3_state + "," + relay_4_title + "," + relay_4_state + "\n";  // if on the test page, merge the strings to be logged 

  logging(store);  //store data to flash memory   
} 
*/

/*
 * Checks for available filename for use with store_data() function
 */
 /*
void file_checker(){
  // Construct the filename to be the next incrementally indexed filename in the set [000-999].
  for (int i = 1; i <= 999; i++) {  // check before modifying target filename.
    if (!fatfs.exists(file_name)) {  // make sure file doesn't exist.
      File logfile = fatfs.open(file_name, FILE_WRITE);  // write to flush cache.
      logfile.close();
      break;  // break out of the for loop.
    }   
     
    if(i < 10) file_string = "LOGGER00" + String(i) + ".CSV";  // If we reach this point the file exists so increment the 2 digit filename index.
    else if(i >= 10 && i <= 99) file_string = "LOGGER0" + String(i) + ".CSV";
    else if(i >= 100) file_string = "LOGGER" + String(i) + ".CSV";        
    file_string.toCharArray(file_name, 28);  // convert to cstring for use in logging    
  }
  Serial.print("Logging Data in: "); Serial.println(file_name);  // print the data to the serial monitor  
}
*/


/*
 * Generates files for storage if none are present. (Storage wipe, accidental
 * file deletion etc)
 */
void generate_files() {
    // Delete files if factory reset is selected
    if (f_reset)
    {
        f_reset = false;
        File config_file = fatfs.open("status.txt", FILE_WRITE);
        config_file.remove();
        config_file = fatfs.open("recipes.txt", FILE_WRITE);
        config_file.remove();
        config_file = fatfs.open("config.txt", FILE_WRITE);
        config_file.remove();
    }


    if (!fatfs.exists("status.txt")) {
        File config_file = fatfs.open("status.txt", FILE_WRITE);
        Serial.println("Generating status.txt file");  // Print status
        char cont[1024];  // Container for json    
        StaticJsonDocument<500> doc;

        doc["f_power"] = false;
        doc["d_power"] = false;
        doc["c_power"] = false;
        doc["r_cycle"] = false;  // Default Recipe cycle false
        doc["r_select"] = false;

        doc["fridge_status"] = false;  // Ground Fault Detection
        doc["vacuum_status"] = false;
        doc["fan_status"] = false;
        doc["heat_status"] = false;

        serializeJsonPretty(doc, cont);  // Serialize   
        config_file.println(cont);  // Write to file
        config_file.close();  // Close file   
    }


    // Generate Recipe file
    if (!fatfs.exists("recipes.txt")) {
        File config_file = fatfs.open("recipes.txt", FILE_WRITE);  // Create file and set defaults 
        Serial.println("Generating recipes.txt file");  // Print status

        char cont[1024];  // Container for json    
        StaticJsonDocument<500> doc;

        Candy = Candy_FactoryDefault;
        Greens = Greens_FactoryDefault;
        Meat = Meat_FactoryDefault;
        Fruit = Fruit_FactoryDefault;
        Liquid = Liquid_FactoryDefault;
        Delicates = Delicates_FactoryDefault;
        DelicatesSecondary = DelicatesSecondary_FactoryDefault;

        doc["c_ftime"] = Candy.freeze_time;  // Add values
        doc["c_dtime"] = Candy.dry_time;
        doc["c_temp"] = Candy.dry_temp;
        doc["c_pressure"] = Candy.pressure;
        doc["g_ftime"] = Greens.freeze_time;  // Add values
        doc["g_dtime"] = Greens.dry_time;
        doc["g_temp"] = Greens.dry_temp;
        doc["g_pressure"] = Greens.pressure;
        doc["m_ftime"] = Meat.freeze_time;  // Add values
        doc["m_dtime"] = Meat.dry_time;
        doc["m_temp"] = Meat.dry_temp;
        doc["m_pressure"] = Meat.pressure;
        doc["f_ftime"] = Fruit.freeze_time;  // Add values
        doc["f_dtime"] = Fruit.dry_time;
        doc["f_temp"] = Fruit.dry_temp;
        doc["f_pressure"] = Fruit.pressure;
        doc["l_ftime"] = Liquid.freeze_time;  // Add values
        doc["l_dtime"] = Liquid.dry_time;
        doc["l_temp"] = Liquid.dry_temp;
        doc["l_pressure"] = Liquid.pressure;
        doc["d_ftime"] = Delicates.freeze_time;  // Add values
        doc["d_dtime"] = Delicates.dry_time;
        doc["d_temp"] = Delicates.dry_temp;
        doc["d_pressure"] = Delicates.pressure;
        doc["ds_ft"] = DelicatesSecondary.freeze_time;
        doc["ds_dt"] = DelicatesSecondary.dry_time;
        doc["ds_temp"] = DelicatesSecondary.dry_temp;
        doc["ds_pres"] = DelicatesSecondary.pressure;

        serializeJsonPretty(doc, cont);  // serialize   
        config_file.println(cont);  // write to file
        config_file.close();  // close file    
    }


    // Generate Configuration file
    if (!fatfs.exists("config.txt")) {  // Generate blank config file      

        File config_file = fatfs.open("config.txt", FILE_WRITE);  // Create file and set defaults 
        Serial.println("Generating config.txt file");  // Print status

        char cont[1024];  // container for json    
        StaticJsonDocument<500> doc;

        doc["fhour"] = 8;  // Add values
        doc["fmin"] = 0;
        doc["dhour"] = 16;
        doc["dmin"] = 0;
        doc["punit"] = 1;
        doc["tunit"] = true;  // true is fahrenheit false is celsius
        doc["dday"] = 0;
        doc["fday"] = 0;
        doc["stemp"] = 49;
        doc["button"] = true;
        doc["chyme"] = true;
        doc["pressure"] = 120 + OFFSET;
        doc["d_fan"] = true;
        doc["c_vac"] = true;

        serializeJsonPretty(doc, cont);  // serialize   
        config_file.println(cont);  // write to file
        config_file.close();  // close file    
    }

    if (!fatfs.exists("on_time.txt")) {  // Generate blank on-time file
        File time_file = fatfs.open("on_time.txt", FILE_WRITE);  // Create file and set defaults   
        Serial.println("Generating on_time.txt file");

        char cont[128];  // containers for json
        StaticJsonDocument<200> elapse;

        elapse["hr"] = 0;  // Add values to Json document
        elapse["min"] = 0;
        elapse["day"] = 0;

        serializeJsonPretty(elapse, cont);  // serialize json to cstring    
        time_file.println(cont);  // write to file
        time_file.close();  // close file     
    }

    if (!fatfs.exists("max_temp.txt")) {  // Generate new max temperature file
        Serial.println("Generating max_temp.txt file");

        File temp_file = fatfs.open("max_temp.txt", FILE_WRITE);  // Create file and set defaults

        char cont[128];  // containers for json
        StaticJsonDocument<200> temp;

        temp["temp"] = 0.0;  // Add values   

        serializeJsonPretty(temp, cont);  // serialize the data to buffer string     
        temp_file.println(cont);  // open and write to file
        temp_file.close();  // close file    
    }
}


enum StatusColor
{
    RED = 255 << 16 | 0 << 8 | 0,
    ORANGE = 250 << 16 | 85 << 8 | 0,
    YELLOW = 165 << 16 | 110 << 8 | 15,
    GREEN = 0 << 16 | 255 << 8 | 0,
    BLUE = 0 << 16 | 50 << 8 | 255,
    PURPLE = 255 << 16 | 0 << 8 | 220,

    WHITE = 255 << 16 | 200 << 8 | 170,
    PINK = 255 << 16 | 95 << 8 | 120,
    TURQUOISE = 0 << 16 | 255 << 8 | 200,

    OFF = 0
};

// Functions for setting neopixel on digital circuit board.
void set_pixel_blue(){
  pixels.clear();
  pixels.setPixelColor(0, BLUE);
  pixels.show();
}

void set_pixel_green(){
  pixels.clear();
  pixels.setPixelColor(0, GREEN);
  pixels.show();
}

void set_pixel_yellow(){
  pixels.clear();
  pixels.setPixelColor(0, YELLOW);
  pixels.show();
}

void set_pixel_red(){
  pixels.clear();
  pixels.setPixelColor(0, RED);
  pixels.show();
}

void set_pixel_purple(){
  pixels.clear();
  pixels.setPixelColor(0, PURPLE);
  pixels.show();
}

void set_pixel_white(){
  pixels.clear();
  pixels.setPixelColor(0, WHITE);
  pixels.show();
}

void reset_screen(){
  //TFT Display Initialization
    uint16_t ID = tft.readID();    
    tft.begin(ID);  // Starts display
    tft.setRotation(1);  // Rotate to landscape  
    ts.begin(40);
    delay(250);
}

// Checks if any of the sensors have been offline longer than their respective default timeouts
// If so, sets page to 23 and returns TRUE
int standardSensorCheck()
{
    int numDisconnected = 0;
    if (standardSensorCheck_pressure()) {
        setPage(23);
        numDisconnected++;
    }

    if(standardSensorCheck_shelf()) {
        setPage(23);
        numDisconnected++;
    }

#ifdef INCL_CHMBR_IN_STD_CHECK
    if (standardSensorCheck_chamber()) {
        setPage(23);
        numDisconnected++;
    }
#endif
        
    return numDisconnected;
}

// TRUE if pressure sensor is offline
bool standardSensorCheck_pressure() {
    return (TimeSinceLastBufferPacket > PRES_POLL_TIMEOUT_ms) || (TimeSinceLastValidPresure > PRES_INV_TIMEOUT_ms);
}

// TRUE if shelf sensor is offline
bool standardSensorCheck_shelf() {
    return ShelfTemp.UpTime < (int)(-SHELF_TEMP_TIMEOUT_ms);
}

// TRUE if chamber sensor is offline
bool standardSensorCheck_chamber() {
    return ChmbrTemp.UpTime < (int)(-CHMBR_TEMP_TIMEOUT_ms);
}