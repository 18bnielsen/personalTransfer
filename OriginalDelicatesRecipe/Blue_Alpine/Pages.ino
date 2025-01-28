/*********************************************************************************
This file contains our diffrent pages. The pages are the structure of how we run our system as well as display to the user. 

Functions in this section are:

homepage: First page shown at start, from here you can go to the recipe, settings, and quick start pages.

quickStart: This is the quick start page from here you can go back to the hompage, the freeze warning page, or the dry warning page.

freeze: This page starts up the fridge and fan on the machine with a timer. This page moves on to dry page.

dry: This page runs the vacuum and the heating shelves with a timer going. This page allows us to adjust the shelf temp and time. This page moves to the complete page(page 8).

freeze_warning: Warning page. Moves to freeze page.

freeze_warning_recipes: Warning page for the recipes page. Moves to freeze page.

dry_warning: warning page. Moves to dry page.

pause_dry: The pause_dry page will turn off the vacuum and turn on the refrigeration. The cycle can be continued without losing your previous cycle time that was completed.

complete: The complete cycle is meant to keep food fresh after a cycle is finished. The refrigeration is turned on and the heating shelves are set to 75% of the shelf
          temp used during the latest cycle unless that temp was over 86 degrees fahrenheit. We also maintain the vacuum by maintaining a pressure between 3,000 - 10,000 pascals.

defrost: The defrost cycle turns on the shelf units and heats up the chamber to remove ice buildup.

recipes: This displays the diffrent recipe pages.

recipe_info:

edit_recipe_select: This displays the diffrent recipe pages avalible to edit

edit_recipe: This page allows us to to edit the settings and varibles of the chosen recipe. This is then saved into flash memory. 

remove_food: Food Removal warning, done right before the defrost cycle.

candy_cooldown: The candy cooldown page is only used when you select the candy recipe, it gives 15 minutes after the dry cycle of refrigeration
                to allow the candy to cooldown. Once completed it goes to the cycle complete page.

Pre-warming_candy: warms candy when selected from Page 42. Moves on to page 4 after.

Pre-warm_candy_option: This option is for whenever a candy recipe is used, it asks if they want to pre-warm there candy. Also gives options for time and temp
                       If yes, it goes to the Pre-warming candy page. If no, bypass to freeze.

quality_control_test:

sensor_testing:

sensor_failed:

qc_test_results:

release_pressure:

leak_rate:

display_leak_rate:

**********************************************************************************/




/*********************************************************************************
* Page 1: Homepage
*
*
*
**********************************************************************************/
void homepage() {  // Page 1

    if (!done) {  //initialize items
        set_pixel_blue();

        tft.fillScreen(BACKGROUND);

        /// TODO: Display Popup window to tell user their software was updated successfully ///

        CycleTime = 0;
        reset_values();  //grab variable data from the stored values
        freeze_cycle_status = false;
        dry_cycle_status = false;
        complete_cycle_status = false;
        recipe_cycle = false;
        IgnoreChmbrTemp = false;
        ForceHeaterON = false;
        store_status();
        draw_line(5, 310, SOFTWARE_VERSION, TFT_WHITE, &Rajdhani_SemiBold9pt7b);
    }

    if (draw_button(20, 45, 440, 113, MAIN_THEME, "   QUICK START", &Nunito_Black20pt7b))
    {
        setPage(2);
        return;
    }

    if (draw_button(20, 183, 215, 102, COMP_THEME, "    RECIPES", &Nunito_Black14pt7b))
    {
        setPage(18);
        return;
    }

    if (draw_button(245, 183, 215, 102, COMP_THEME, "      SETTINGS", &Nunito_Black14pt7b))
    {
        setPage(17);
        return;
    }

    if (!done)
    {
        tft.drawBitmap(50, 70, power, 60, 60, ICON);  //draw images on buttons
        tft.drawBitmap(30, 207, menu, 50, 50, ICON);
        tft.drawBitmap(255, 207, settings, 50, 50, ICON);
    }

    done = true;  //page done loading
}

/*********************************************************************************
*Page 2: Quickstart
*
*
**********************************************************************************/
void quickStart() {  // Page 2

    if (!done) {
        set_pixel_blue();

        tft.fillScreen(BACKGROUND);
        CycleTime = 0;
    }


    if (draw_button(33, 120, 415, 90, COMP_THEME, "BACK", &Nunito_Black20pt7b))
    {
        setPage(1);
        return;
    }

    if (draw_button(33, 20, 415, 90, MAIN_THEME, "START", &Nunito_Black20pt7b))
    {
        setPage(5);
        return;
    }

    if (draw_button(247, 220, 200, 90, COMP_THEME, "SKIP", "FREEZE CYCLE", &Nunito_Black10pt7b))
    {
        setPage(6);
        return;
    }

    if (draw_button(33, 220, 200, 90, COMP_THEME, "DEFROST", &Nunito_Black14pt7b))
    {
        setPage(9);
        return;
    }

    
    /// error handling ///

    if ((TimeSinceLastValidPresure > PRES_INV_TIMEOUT_ms || TimeSinceLastBufferPacket > 30000 || testingErrorPage) && !benchDevelopment)
    {
        setPage(23);
        return;
    }

    if ((ShelfTemp.UpTime < -30000) && !benchDevelopment)
    {
        setPage(23);
        return;
    }

    done = true;
}

/*********************************************************************************
*Page 3: Freeze Cycle
*
*
**********************************************************************************/
void freeze()
{
    static Time cooling_checker_time;
    //draw single draw items and initialize values
    if (!done)
    {
        set_pixel_blue();
        tft.fillScreen(BACKGROUND);
        draw_title("FREEZE CYCLE");
        draw_line(20, 80, "Time", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(20, 110, "Left:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(20, 155, "Chamber Temp.:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(20, 190, "Elapsed Time:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        //static colons for time
        draw_line(320, 190, ":", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(265, 190, ":", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        //if (freeze_cycle_status) reset_values();

        if(prev_page > 0)
        {
            CycleTime = 0;

            if (!recipe_cycle)
            {
                Current_Cycle_Duration = freeze_time;
            }
            else
            {
                Current_Cycle_Duration = recipe_freeze_time;          
            }
        }

        //file_checker(); //for file storage
        dry_cycle_status = false;
        freeze_cycle_status = true;
        complete_cycle_status = false;
        store_status();

        
        cooling_checker_time = CycleTime + 3_hr;
    }

    hardware_control(1, 1, 0);  //turn on both fan and freezer
    /// active UI ///
    time_countdown(&Current_Cycle_Duration);  //run time countdown
    elapsed_time_medium(220, 190);  //draw elapsed time to the screen
    temp_unit_changer(413, 150, 60);  //draw temp. unit changer button
    (ChmbrTemp.Fault ? ShelfTemp : ChmbrTemp).Draw(220, 155);

    if (draw_button(245, 220, 215, 90, MAIN_THEME, "SKIP", "FREEZE CYCLE", &Nunito_Black10pt7b))
    {
        setPage(6);
        return;
    }

    if (draw_button(20, 220, 215, 90, TFT_RED, "STOP", &Nunito_Black20pt7b))
    {
        store_elapsed();
        setPage(1);
        return;
    }
    
    //switch pages after time runs out
    if (remaining <= 0)
    {
        store_elapsed();
        setPage(4);
        return;
    }

    // System Hardware Error Checking & Monitoring
    if (cooling_checker(cooling_checker_time, -5, true) && !IgnoreChmbrTemp)
    {
        setPage(16);
        return;
    }

    done = true;

    /// TODO: Use shelf sensor for calculations if the chamber sensor is disconnected
    //standardSensorCheck();
    
    // Don't throw error unless both temperature sensors are offline (we only need one when not heating)
    bool sensorError;
    sensorError  = standardSensorCheck_pressure();
    sensorError |= standardSensorCheck_shelf() && standardSensorCheck_chamber();
    if ((sensorError) && !benchDevelopment)
        setPage(23);
}


// page 4
void dry()
{
    static Time ignorePressureUntil;
    static Time cooling_checker_time;
    //static Time vacuumDelay;
    static bool titlePrinted;

    enum {
        VACUUM_TEMPERATURE_THRESHOLD = 0, // maximum temperature before the vaccum pump can engage (deg C)
        VACUUM_DELAY_sec = 2, // to mitigate electrical noise, don't start everything right away
    };

    if (!done)
    {
        tft.fillScreen(BACKGROUND);
        if(recipe_cycle)
        {
          if(recipe_sel == 6)
          {
            draw_title("PRIMARY DRY");
          }
          else if(recipe_sel == 7)
          {
            draw_title("SECONDARY DRY");
          }
          else
          {
            draw_title("DRY CYCLE");
          }
        }
        else
        {
          draw_title("DRY CYCLE");
        }
        vacError = false;
        draw_line(10, 80, "Time", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(10, 110, "Left:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(10, 160, "Max. Shelf", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(10, 190, "Temperature:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(150, 234, "Chamb. Temp.:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(150, 256, "Shelf Temp.:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(150, 278, "Pressure:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(150, 300, "Elapsed Time:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        set_pixel_blue();

        if(prev_page > 0 )//|| recipe_sel == 7
        {
            CycleTime = 0;
            IgnoreChmbrTemp = false;

            if (!recipe_cycle)
            {
                Current_Cycle_Duration = dry_time;
                ShelfTargetTemperature = default_dry_temp;
                set_pressure = s_pressure;
            }
            else
            {
                Current_Cycle_Duration = recipe_dry_time;
                ShelfTargetTemperature = current_recipe.dry_temp;
                set_pressure = current_recipe.pressure;
            }
        }

        //vacuumDelay = CycleTime + 2_sec;
        cooling_checker_time = CycleTime + 3_hr;

        // Used to come back to this page if power is lost
        dry_cycle_status = true;
        freeze_cycle_status = false;
        complete_cycle_status = false;
        titlePrinted = false;

        store_status();
    }

    /// Update UI Components ///
    ChmbrTemp.Draw(280, 234);
    ShelfTemp.Draw(280, 256);
    draw_pressure(280, 278);

    elapsed_time_small(280, 300);           // draw elapsed time  
    shelf_temp_changer(175, 415, 280, 135); // shelf temperature change UI  
    temp_unit_changer(413, 210, 60);        // temp unit button  
    pressure_unit_changer(413, 265, 60);    // pressure unit button

    if (draw_button(10, 220, 120, 90, TFT_RED, "PAUSE", &Nunito_Black14pt7b))
    {
        dry_cycle_status = false;
        setPage(7);
        return;
    }

    if ((ChmbrTemp.Celsius <= VACUUM_TEMPERATURE_THRESHOLD || ChmbrTemp.Fault || IgnoreChmbrTemp))
    {
        IgnoreChmbrTemp = true; // if we start the cycle with the chamber sensor disconnected, then reconnect it, we don't want to then stop the cycle and "prepare the cold chamber", essentially moving the cycle backwards.

        if (!done || !titlePrinted)
        {
            tft.fillRect(0, 0, 480, 50, BACKGROUND);
            if(recipe_cycle)
            {
              if(recipe_sel == 6)
              {
                draw_title("PRIMARY DRY");
              }
              else if(recipe_sel == 7)
              {
                draw_title("SECONDARY DRY");
              }
              else
              {
                draw_title("DRY CYCLE");
              }
            }
            else
            {
              draw_title("DRY CYCLE");
            }
            titlePrinted = true;

            // ignore sensor readings until certian time -after- vacuum starts
            ignorePressureUntil = CycleTime + 15_min;
            reset_inval_press_time();
            reset_inval_press_rate_time();

            // reset clock if we were previously "preparing cold chamber"
            if (prev_page > 0) CycleTime = 0;
        }


        time_countdown(&Current_Cycle_Duration);

        //Serial.println()
        
        hardware_control(0, 1, 1); // fridge on, vac on
        //Serial.print("vac Error: "); Serial.println(vacError);
        if(CycleTime.totalMinutes() < 2)
        {
        //Serial.print("Elapsed Min: "); Serial.println(CycleTime.totalMinutes());
          if(chamber_pressure >= 700000)
          {
            vacError = true;
          }
          else
          {
            vacError = false;
          }
        }
        else if(vacError)
        {
          dry_cycle_status = false;
          setPage(50);
          throwPopUp = true;
          vacError = false;
          return;
        }      

        /// Pressure Sensor Error Checking & Monitoring ///
        // set the compare values to change how long something needs to be wrong before and error is thrown
        unsigned presUpperBound = (int)(set_pressure * 2);  // Check for pressure lower than set pressure + 25%
        unsigned long invPresTime = inval_press_time(10, is_candy ? 3333 : presUpperBound);
        unsigned long invPresRateTime = inval_press_rate_time(-1.0, -0.02);

        //Serial.printf("Invalid pressure: %u    rate: %u    t(p): %s\n", invPresTime, invPresRateTime, ignorePressureUntil.toString().c_str());

        if ((CycleTime > ignorePressureUntil) && !benchDevelopment)
        {
            Serial.println("CycleTime > ignorePressureUntil");
            if (TimeSinceLastValidPresure > PRES_INV_TIMEOUT_ms)
            {
                Serial.println("TimeSinceLastValidPresure > PRES_INV_TIMEOUT_ms");

                // is the reason for no valid pressure a buffer disconnect?
                if (TimeSinceLastBufferPacket > TimeSinceLastValidPresure * 0.85)
                {
                    Serial.println("Aborting dry cycle because TimeSinceLastBufferPacket > 5 min");
                    Vaccum_Error_Reason = VACCUM_ERROR_BUFFER_DISCONNECT;
                }
                else
                {
                    Serial.println("Aborting dry cycle because TimeSinceLastValidPresure > 5 min");
                }

                //Serial.println("<Throw error>");
                setPage(23);
                return;
            }
            else if (invPresTime > PRES_INV_TIMEOUT_ms && invPresRateTime > (45_sec * 1000))
            {
                dry_cycle_status = false;
                Serial.println("Aborting dry cycle because pressure has been out of range for > 3 min");

                //Serial.println("<Throw error>");
                setPage(14);
                return;
            }
        }
        
    }
    else
    {
        if (!done || titlePrinted)
        {
            tft.fillRect(0, 0, 480, 50, BACKGROUND);
            draw_line(20, "Preparing Cold Chamber...", TFT_CYAN, &Rajdhani_SemiBold12pt7b);
            draw_line(35, "Vacuum Pump on Standby", TFT_RED, &Rajdhani_SemiBold9pt7b);
            titlePrinted = false;
        }


        time_countdown(&Current_Cycle_Duration);
        hardware_control(0, 1, 0); // fridge on, vac off

        if (cooling_checker(cooling_checker_time, 0))
        {
            setPage(16);
            return;
        }
    }

    if (remaining <= 0)
    {
        store_elapsed();
        if(recipe_sel == 6)
        {
          Serial.println("Changing to 7");
          recipe_sel = 7;
          current_recipe = DelicatesSecondary;
          recipe_dry_time = Time(current_recipe.dry_time);
          Serial.print("Recipe Dry Time: "); Serial.println(recipe_dry_time.totalHours());
          prev_page = 4;
          //recipe_freeze_time = Time(current_recipe.freeze_time);  // divide by 3600 puts the hours into seconds
          recipe_cycle = true;
          done = false;
          return;
        }
        else
        {
          setPage(is_candy ? 33 : 8);
        }
        return;
    }

    done = true;
    if (standardSensorCheck_shelf() && !benchDevelopment)
    {
        setPage(23);
        return;
    }

    // make sure the heaters don't melt our food trying to maintain a pressure without any pressure data
    ForceHeaterOFF = (TimeSinceLastValidPresure > 3_sec * 1000);
}



/*********************************************************************************
Page 5: Freeze Warning


**********************************************************************************/
void freeze_warning() {  // Page 5

    if (!done) {
        set_pixel_blue();
        tft.fillScreen(BACKGROUND);  //static items                                                                           
        draw_line(50, "Ensure that the door", TFT_WHITE);
        draw_line(90, "is shut, and the drain", TFT_WHITE);
        draw_line(130, "valve is closed.", TFT_WHITE);
        recipe_cycle = false;  // If not coming from a recipe, set recipe cycle to false
    }


    if (draw_button(20, 210, 215, 90, MAIN_THEME, "CANCEL", &Nunito_Black20pt7b)) {  //Cancel button
        setPage(1);
        return;
    }
    if (draw_button(245, 210, 215, 90, MAIN_THEME, "CONTINUE", &Nunito_Black20pt7b)) {  //Start button
        setPage(3);
        return;
    }


    done = true;

    standardSensorCheck();
}

/*********************************************************************************
Page 32: Freeze Warning Recipes


**********************************************************************************/
void freeze_warning_recipes() {

    if (!done)
    {
        set_pixel_blue();
        tft.fillScreen(BACKGROUND);                                                                       
        draw_line(50, "Ensure that the door", TFT_WHITE);
        draw_line(90, "is shut, and the drain", TFT_WHITE);
        draw_line(130, "valve is closed.", TFT_WHITE);
        recipe_cycle = true;
    }

    if (draw_button(20, 210, 215, 90, MAIN_THEME, "CANCEL", &Nunito_Black20pt7b))
    {
        is_candy = false;
        setPage(1);
        return;
    }

    if (draw_button(245, 210, 215, 90, MAIN_THEME, "CONTINUE", &Nunito_Black20pt7b))
    {
        if(recipe_sel == 6 || recipe_sel == 7)
        {
          setPage(3);
          return;
        }
        if (freeze_zero)  setPage(is_candy ? 42 : 4);
        else              setPage(3);
        
        return;
    }

    done = true;
    standardSensorCheck();
}

/*********************************************************************************
Page 6: Dry Warning


**********************************************************************************/
void dry_warning() {  // Page 6

    if (!done) {
        set_pixel_blue();
        tft.fillScreen(BACKGROUND);  //static items
        draw_title("SKIP FREEZE CYCLE");
        draw_line(80, "WARNING", TFT_RED, &Nunito_Black20pt7b);
        draw_line(115, "Vacuum Error may occur if cold food is put in a warm", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(135, "chamber. To prevent this, cool chamber for 30 minutes", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(155, "before inserting frozen food. Adding cold food to a warm", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(175, "chamber may also cause the pump to ingest more water", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
    }  //draw single draw items


    


  //if coming from the freeze page keep fridge running
    if (abs(prev_page) == 3) 
        hardware_control(0, 1, 0);

  //draw continue button
    if (draw_button(245, 220, 215, 90, MAIN_THEME, "CONTINUE", &Nunito_Black20pt7b))
    {
        ///skipSensor = true;
        store_elapsed();
        
        setPage(4);
        return;
    }

  //draw cancel button, has special functions depending on the previous page
    if (draw_button(20, 220, 215, 90, MAIN_THEME, "CANCEL", &Nunito_Black20pt7b))
    {
        // page 3 = freeze
        setPage(abs(prev_page) == 3 ? 3 : 1);
        return;
    }
    done = true;
}

/*
    Page 7: Pause
    The pause_dry page will turn off the vacuum and turn on the refrigeration.
    The cycle can be continued without losing your previous cycle time that was completed.
    Renamed to pause_dry becasue it doesn't make sense to stop any other part of the cycle (eg, freeze)
*/
void pause_dry()
{
    static Time pausedRTC;
    static bool blinkState; // whether or not "CYCLE PAUSED" is currently written to the screen

    hardware_control(0, 1, 0);  // Turn off vacuum when paused

    if (!done) {
        set_pixel_yellow();
        tft.fillScreen(BACKGROUND);
        draw_line(80, "CYCLE", TFT_WHITE, &Nunito_Black30pt7b);
        draw_line(160, "PAUSED", TFT_WHITE, &Nunito_Black30pt7b);
        pausedRTC = CycleTime;
    }
    
    if (millis() % 1250 < 500)
    {
        if(blinkState)
        {
            tft.fillRect(0, 0, 480, 215, BACKGROUND);
            blinkState = false;
        }
    }
    else if(!blinkState)
    {
        draw_line(80, "CYCLE", TFT_WHITE, &Nunito_Black30pt7b);
        draw_line(160, "PAUSED", TFT_WHITE, &Nunito_Black30pt7b);
        blinkState = true;
    }


    if (draw_button(245, 220, 215, 90, MAIN_THEME, "RESUME", &Nunito_Black20pt7b))
    {
        unpause(pausedRTC);
        return;
    }
    if (draw_button(20, 220, 215, 90, TFT_RED, "STOP", &Nunito_Black20pt7b))
    {
        CycleTime = pausedRTC;
        prev_page = setPage(8);
        return;
    }

    if(throwPopUp)
    {
      setPage(50);
      return;
    }
    done = true;
}


/*********************************************************************************
* Page 8: Complete
* The complete cycle is meant to keep food fresh after a cycle is finished. The
* refrigeration is turned on and the heating shelves are set to 75% of the shelf
* temp used during the latest cycle unless that temp was over 86 degrees fahrenheit.
* We also maintain the vacuum by maintaining a pressure between 3,000 - 10,000 pascals.
**********************************************************************************/
void complete()
{
    static Time completeRTC;
    static bool complete_vacuum;

    if (!done)
    {
        set_pixel_green();
        tft.fillScreen(BACKGROUND);  //static items                                                     
        draw_line(80, "CYCLE", TFT_WHITE, &Nunito_Black30pt7b);
        draw_line(160, "COMPLETE", TFT_WHITE, &Nunito_Black30pt7b);
        freeze_cycle_status = false;
        dry_cycle_status = false;
        freeze_zero = false;
        complete_cycle_status = true;
        store_status();
        Serial.println("Set Shelf Temp: ");
        Serial.print(ShelfTargetTemperature);

        // if the set shelf temp was less than 86f then set the complete temp of shelves to 75% of the set shelf temp form the dry cycle.
        if (!recipe_cycle) {  //if it is not a recipe, then keep the temperature the same as dry
            complete_temp = ShelfTargetTemperature;
        }
        else {
            if (recipe_sel == 1) {
                complete_temp = 49;  //sets shelf temp to 120F for candy
            }
            else if (recipe_sel == 2 || recipe_sel == 6) {  //for greens and delicates
                complete_temp = ShelfTargetTemperature * 0.8;  //sets to 80% of original shelf temp
            }
            else if (recipe_sel == 7 || recipe_sel == 6)
            {
              complete_temp = 21.11;
            }
            else {
                complete_temp = ShelfTargetTemperature;  //any other recipe sets to what shelf temp was
            }
        }

        completeRTC = CycleTime;
    }

    // If presssure is below 10,000 and (presumably?) above 3,000 pascals


    if (draw_button(163, 220, 153, 90, MAIN_THEME, "DEFROST", &Nunito_Black14pt7b))
    {
        store_elapsed();
        is_candy = false;
        setPage(31);
        return;
    }
    if (draw_button(5, 220, 153, 90, TFT_RED, "HOME", &Nunito_Black20pt7b))
    {
        store_elapsed();
        is_candy = false;
        setPage(1);
        return;
    }
    if (draw_button(321, 220, 153, 90, TFT_BLUE, "ADD MORE", "TIME", &Nunito_Black10pt7b))
    {
        Current_Cycle_Duration = completeRTC + remaining + 2_hr;
        unpause(completeRTC);
        page = 4;
        return;
    }
    if(!completeVaccumOnAlways)
    {
      if (inval_press_time(400, 1333) >= 3000) 
      {
        complete_vacuum = false;
      }
      else if (pressure_ > 400 && pressure_ < 3000)
      {
        complete_vacuum = true;
      }
      else
      {
        complete_vacuum = false;
      }
    }
    else
    {
      complete_vacuum = true;
    }
    hardware_control(0, 1, complete_vacuum);

    done = true;
}


/*********************************************************************************
* Page 9: Defrost
* The defrost cycle turns on the shelf units and heats up the chamber to remove ice
* buildup.
**********************************************************************************/

// Page 9
void defrost()
{
    //static toggle items
    static bool toggle_refresh_3, toggle_3;

    //static items
    if (!done)
    {
        set_pixel_red();
        tft.fillScreen(BACKGROUND);  //fill screen with background    
        draw_title("DEFROST");
        draw_line(10, 80, "Time", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(10, 110, "Left:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(55, 165, "Chamber Fan", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(55, 185, "Control:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        CycleTime = 0;
        Current_Cycle_Duration = defrost_time;
        
        hardware_control(0, 0, 0);  // turn off unused hardware

        freeze_cycle_status = false;
        dry_cycle_status = false;
        complete_cycle_status = false;
        store_status();
    }

    //draw countdown timer
    time_countdown(&Current_Cycle_Duration);

    // we may not need this function anymore since we've fixed the triac and have not seen heating issues.

    hardware_control(defrost_fan, 0, 0);

    //draw BACK button
    if (draw_button(20, 220, 440, 90, TFT_RED, "CANCEL", &Nunito_Black20pt7b))
    {
        setPage(1);
        return;
    }

    //switch pages after timer finishes
    if (remaining <= 0)
    {
        setPage(1);
        return;
    }

    //Draw SOUND TOGGLE buttons 
    if (!defrost_fan)
    {
        if (draw_toggle(250, 140, 200, 60, &toggle_refresh_3, defrost_fan, &toggle_3))
        {
            defrost_fan = !defrost_fan;
            toggle_refresh_3 = true;
            delay(150);
        }
    }
    else
    {
        if (draw_toggle(250, 140, 200, 60, &toggle_refresh_3, defrost_fan, &toggle_3))
        {
            defrost_fan = !defrost_fan; toggle_refresh_3 = true; delay(150);
        }
    }


    done = true;

    standardSensorCheck();
}


/*********************************************************************************
* Page 18: Recipes
*
*
**********************************************************************************/
void recipes(){  // Page 18
  if(!done){
    set_pixel_blue();
    tft.fillScreen(BACKGROUND);  //static items
    draw_title("RECIPES");
  }

  //draw the different buttons
  if(draw_button(7, 200, 150, 90, COMP_THEME, "      Fruit", &Rajdhani_SemiBold12pt7b)){  
    current_recipe = {Fruit.freeze_time, Fruit.dry_time, Fruit.pressure, Fruit.dry_temp};
    recipe_sel = 4;
    setPage(22);
    return;
  }
  if(draw_button(165, 200, 150, 90, COMP_THEME, "      Liquid", &Rajdhani_SemiBold12pt7b)){
    current_recipe = {Liquid.freeze_time, Liquid.dry_time, Liquid.pressure, Liquid.dry_temp};
    recipe_sel = 5;
    setPage(22);
    return;
  }
  if(draw_button(322, 90, 150, 90, COMP_THEME, "      Meat", &Rajdhani_SemiBold12pt7b)){
    current_recipe = {Meat.freeze_time, Meat.dry_time, Meat.pressure, Meat.dry_temp};
    recipe_sel = 3;
    setPage(22);
    return;
  }
  if(draw_button(7, 90, 150, 90, COMP_THEME, "        Candy", &Rajdhani_SemiBold12pt7b)){
    current_recipe = {Candy.freeze_time, Candy.dry_time, Candy.pressure, Candy.dry_temp};
    recipe_sel = 1;
    setPage(22);
    return;
  }
  if(draw_button(165, 90, 150, 90, COMP_THEME, "      Greens", &Rajdhani_SemiBold12pt7b)){
    current_recipe = {Greens.freeze_time, Greens.dry_time, Greens.pressure, Greens.dry_temp};
    recipe_sel = 2;
    setPage(22);
    return;
  }
  if(draw_button(322, 200, 150, 90, COMP_THEME, "Delicates", &Rajdhani_SemiBold12pt7b)){
    current_recipe = {Delicates.freeze_time, Delicates.dry_time, Delicates.pressure, Delicates.dry_temp};
    recipe_sel = 6;
    setPage(22);
    return;
  }
  //exit button
  if(draw_button(5, 3, 110, 65, TFT_RED, "BACK", &Nunito_Black10pt7b)){
      setPage(1);
      return;
  }
  // Edit Button
  if(draw_button(365, 3, 110, 65, COMP_THEME, "EDIT", &Nunito_Black10pt7b)){
      setPage(29);
      return;
  }
  
  if(!done){  //draw images over buttons
    tft.drawBitmap(20, 110, candy_img, 50, 50, TFT_WHITE);
    tft.drawBitmap(173, 110, greens, 50, 50, TFT_WHITE);
    tft.drawBitmap(15, 220, fruit, 50, 50, TFT_WHITE);
    tft.drawBitmap(173, 220, liquid, 50, 50, TFT_WHITE);
    tft.drawBitmap(330, 115, meat, 50, 50, TFT_WHITE);
  }  
  done = true;  
}

/*********************************************************************************
* Page 22: Recipe Info
*
*
**********************************************************************************/
void recipe_info() {  // Page 22
    static String display_freeze_time;
    static String display_dry_time;
    if (!done) {
    set_pixel_blue();
    tft.fillScreen(BACKGROUND);  //static items
    switch (recipe_sel) {
    case 1:draw_title("CANDY");     break;
    case 2:draw_title("GREENS");     break;
    case 3:draw_title("MEAT");      break;
    case 4:draw_title("FRUIT");     break;
    case 5:draw_title("LIQUID");    break;
    case 6:draw_title("PRIMARY DELICATE"); break;
    case 7:draw_title("SECONDARY DELICATE"); break;
    }

// Change minutes from 1 zero to 2 zeros if it is an even hour.
    if (current_recipe.freeze_time / 60 % 60 == 0) display_freeze_time = String(current_recipe.freeze_time / 3600) + ":00";
    else {

// This will check if the time set is under an hour and under 10 minutes, if so it draws the extra zeros for the freeze time
        if (current_recipe.freeze_time < 3600 && (current_recipe.freeze_time / 60) < 10) {  // check if its in the single digits for minutes left
            display_freeze_time = "00:0" + String(current_recipe.freeze_time / 60);
        }
        else {
            display_freeze_time = String(current_recipe.freeze_time / 3600) + ":" + String(current_recipe.freeze_time / 60 % 60);
        }
    }

    if (current_recipe.dry_time / 60 % 60 == 0) display_dry_time = String(current_recipe.dry_time / 3600) + ":00";
    else {

// This will check if the time set is under an hour and under 10 minutes, if so it draws the extra zeros for the freeze time
        if (current_recipe.dry_time < 3600 && (current_recipe.dry_time / 60) < 10) {  // check if its in the single digits for minutes left
            display_dry_time = "00:0" + String(current_recipe.dry_time / 60);
        }
        else {
            display_dry_time = String(current_recipe.dry_time / 3600) + ":" + String(current_recipe.dry_time / 60 % 60);
        }
    }

// if there is no freeze time, set the variable to true, use this variable to skip the freeze page on page 5
    if (current_recipe.freeze_time == 0) {
        freeze_zero = true;
    }
    else {
        freeze_zero = false;
    }

// Draw the 4 values on the screen
    draw_line(80, "Freeze Time: " + display_freeze_time, TFT_WHITE);
    draw_line(120, "Dry Time: " + display_dry_time, TFT_WHITE);
    if (recipe_sel == 1) {
        if (temp_unit) draw_line(160, "Dry/Prewarm Temp: " + String(int(round(current_recipe.dry_temp * 9 / 5 + 32))) + " F", TFT_WHITE);
        else draw_line(160, "Dry Temperature: " + String(int(round(current_recipe.dry_temp))) + " C", TFT_WHITE);
    }
    else {
        if (temp_unit) draw_line(160, "Dry Temperature: " + String(int(round(current_recipe.dry_temp * 9 / 5 + 32))) + " F", TFT_WHITE);
        else draw_line(160, "Dry Temperature: " + String(int(round(current_recipe.dry_temp))) + " C", TFT_WHITE);
    }

    int convert_unit_test;

// Code to change the pressure units
    switch (pressure_unit) {
    case 0:
        draw_line(200, "Dry Pressure: " + String(current_recipe.pressure) + " PA", TFT_WHITE);
        break;

    case 1:
        convert_unit_test = (current_recipe.pressure * 7.5006167382113);
        draw_line(200, "Dry Pressure: " + String(convert_unit_test) + " mTorr", TFT_WHITE);
//draw_line(200, 200, pressure_ * 7.5006167382113, 2, " mTorr", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        break;

    case 2:
        convert_unit_test = (current_recipe.pressure * 0.01);
        draw_line(200, "Dry Pressure: " + String(convert_unit_test) + " mBar", TFT_WHITE);
//draw_line(200, 200, pressure_ * 0.01, 1, " mBar", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        break;

    case 3:
        convert_unit_test = (current_recipe.pressure * 0.00014503773772954);
        draw_line(200, "Dry Pressure: " + String(convert_unit_test) + " PSI", TFT_WHITE);
//draw_line(200, 200, pressure_ * 0.00014503773772954, 3, " PSI", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        break;
    }

  if(recipe_sel == 1){
      is_candy = true;
    }
  }   
  
  if(draw_button(245, 220, 215, 90, MAIN_THEME, "CONTINUE", &Nunito_Black20pt7b)){  //draw continue button  
    if(recipe_sel == 7)
    {
      current_recipe = Delicates;
      recipe_sel = 6;
    }  
    recipe_dry_time = Time(current_recipe.dry_time);
    recipe_freeze_time = Time(current_recipe.freeze_time);  // divide by 3600 puts the hours into seconds
    recipe_cycle = true;
  //store_status();
    setPage(32);
    return;
  }
  
  if(draw_button(20, 220, 215, 90, TFT_RED, "CANCEL", &Nunito_Black20pt7b)){  //draw cancel button
      is_candy = false;
      setPage(18);
      return;
  }

  
  
  if(recipe_sel == 6)
  {
    if (draw_button(5, 60, 60, 50, TFT_RED, "SEC", &Nunito_Black10pt7b))
    {
      recipe_sel = 7;
      current_recipe = DelicatesSecondary;
      done = false;
      return;
    }
  }
  else if(recipe_sel == 7)
  {
    if (draw_button(5, 60, 60, 50, TFT_RED, "PRI", &Nunito_Black10pt7b))
    {
      recipe_sel = 6;
      current_recipe = Delicates;
      done = false;
      return;
    }
  }
  
  done = true;
}

/*********************************************************************************
* Page 29: Edit Recipe Select
*
*
**********************************************************************************/
void edit_recipe_select() {  // Page 29
  if(!done){
    set_pixel_blue();
    tft.fillScreen(BACKGROUND);  //static items 
  }

  //draw the different buttons
  if(draw_button(7, 130, 150, 90, COMP_THEME, "    edit", "    Fruit", &Rajdhani_SemiBold12pt7b)){ 
    current_recipe = Fruit;
    recipe_sel = 4;
    setPage(30);
    return;
  }
  if(draw_button(165, 130, 150, 90, COMP_THEME, "    edit", "    Liquid", &Rajdhani_SemiBold12pt7b)){
    current_recipe = Liquid;
    recipe_sel = 5;
    setPage(30);
    return;
  }
  if(draw_button(322, 20, 150, 90, COMP_THEME, "    edit", "    Meat", &Rajdhani_SemiBold12pt7b)){
    current_recipe = Meat;
    recipe_sel = 3;
    setPage(30);
    return;
  }
  if(draw_button(7, 20, 150, 90, COMP_THEME, "    edit", "    Candy", &Rajdhani_SemiBold12pt7b)){
    current_recipe = Candy;
    recipe_sel = 1;
    setPage(30);
    return;
  }
  if(draw_button(165, 20, 150, 90, COMP_THEME, "    edit", "    Greens", &Rajdhani_SemiBold12pt7b)){
    current_recipe = Greens;
    recipe_sel = 2;
    setPage(30);
    return;
  }
  if(draw_button(322, 130, 150, 90, COMP_THEME, "edit", "Delicates", &Rajdhani_SemiBold12pt7b)){
    current_recipe = Delicates;
    recipe_sel = 6;
    setPage(30);
    return;
  }
  // Saves new values to memory
  if(draw_button(20, 235, 215, 80, TFT_RED, "SAVE", "& EXIT", &Nunito_Black14pt7b)){          
      store_recipe_values();
      setPage(18);
      return;
  }
  // Reverts back to what was stored in memory
  if(draw_button(245, 235, 215, 80, MAIN_THEME, "DISCARD", "CHANGES", &Nunito_Black14pt7b)){  
      memory_check();
      setPage(18);
      return;
  }
  
  if(!done){  //draw images over buttons
    tft.drawBitmap(10, 40, candy_img, 50, 50, TFT_WHITE);
    tft.drawBitmap(168, 40, greens, 50, 50, TFT_WHITE);
    tft.drawBitmap(10, 150, fruit, 50, 50, TFT_WHITE);
    tft.drawBitmap(168, 150, liquid, 50, 50, TFT_WHITE);
    tft.drawBitmap(325, 40, meat, 50, 50, TFT_WHITE);
  }  
  done = true;  
}

/*********************************************************************************
* Page 30: Edit Recipe
*
*
**********************************************************************************/
void edit_recipe()
{
    if (!done)
    {
        set_pixel_blue();
        
        tft.fillScreen(BACKGROUND);

        switch (recipe_sel)
        {
            case 1: draw_title("CANDY");     break;
            case 2: draw_title("GREENS");    break;
            case 3: draw_title("MEAT");      break;
            case 4: draw_title("FRUIT");     break;
            case 5: draw_title("LIQUID");    break;
            case 6: 
            case 7: draw_title("DELICATES"); break;
        }
    }

    default_recipe_changer(70,  &current_recipe.freeze_time, "Freeze Time:", &temp_refresh);
    default_recipe_changer(135, &current_recipe.dry_time,    "Dry Time:",    &pressure_refresh);
    default_recipe_changer(200, &current_recipe.dry_temp,    "Dry Temp:",    &override_refresh);

    // display warning if user moves pressure out of range; don't display if pressure already our of range
    bool pressWasSafe = !pressureSettingUnsafe(current_recipe.pressure);
    default_recipe_changer(265, &current_recipe.pressure,    "Pressure:",    &diagnostic_refresh_1);
    
    /* We are taking this out for the future revisions
    if (pressWasSafe && pressureSettingUnsafe(current_recipe.pressure))
    {
        setPage(28);
        return;
    } */


    //exit button
    if (draw_button(5, 3, 110, 50, TFT_RED, "BACK", &Nunito_Black10pt7b))
    {
        // This will temporarily save the values to the recipes so multiple recipes can be saved at once.
        switch (recipe_sel)
        {
            case 1:Candy = current_recipe;     break;
            case 2:Greens = current_recipe;    break;
            case 3:Meat = current_recipe;      break;
            case 4:Fruit = current_recipe;     break;
            case 5:Liquid = current_recipe;    break;
            case 6:Delicates = current_recipe; break;
            case 7:DelicatesSecondary = current_recipe; break;
        }
        setPage(29);
        return;
    }

    // Reset Button
    if(recipe_sel == 6 || recipe_sel == 7)
    {
      if (draw_button(365, 3, 100, 50, COMP_THEME, "SWITCH", &Nunito_Black10pt7b))
      {
          if(recipe_sel == 6)
          {
            Delicates = current_recipe;
            current_recipe = DelicatesSecondary;
            recipe_sel = 7;
          }
          else
          {
            DelicatesSecondary = current_recipe;
            current_recipe = Delicates;
            recipe_sel = 6;
          }
          done = false;
          return;
      }
    }
    else
    {
      if (draw_button(365, 3, 100, 50, COMP_THEME, "RESET", &Nunito_Black10pt7b))
      {
          recipe_reset();
          done = false;
          return;
      }
    }

    done = true;
}


/*********************************************************************************
* Page 31: Remove Food
* Food Removal warning, done right before the defrost cycle.
**********************************************************************************/
void remove_food(){  // Page 31
  
  if(!done){  //static items    
    set_pixel_green();
    tft.fillScreen(BACKGROUND);
    draw_title("Please Remove Food");
    draw_line(80, "WARNING", TFT_RED, &Nunito_Black20pt7b);
    draw_line(120, "Only proceed with the", TFT_WHITE);
    draw_line(160, "defrost cycle once the food", TFT_WHITE);
    draw_line(200, "has been removed.", TFT_WHITE);
  }   
  
  if (draw_button(20, 210, 215, 90, MAIN_THEME, "CANCEL", &Nunito_Black20pt7b)) {  //Cancel button
      setPage(8);
      return;
  }
  if (draw_button(245, 210, 215, 90, MAIN_THEME, "CONTINUE", &Nunito_Black20pt7b)) {  //Start button
      setPage(9);
      return;
  }

  //hardware_serial_monitor(1, 15000); 
  
  done = true;
}

/*********************************************************************************
* Page 33: Candy Cooldown
* The candy cooldown page is only used when you select the candy recipe, it gives
* 15 minutes after the dry cycle of refrigeration to allow the candy to cooldown.
* Once completed it goes to the cycle complete page
**********************************************************************************/
void candy_cooldown(){  // Page 33
  
  if(!done){  //static items    
    set_pixel_blue();
    tft.fillScreen(BACKGROUND);  //fill screen with background    
    draw_line(40, "CANDY", TFT_WHITE, &Nunito_Black20pt7b);
    draw_line(100, "COOLDOWN", TFT_WHITE, &Nunito_Black20pt7b);

    draw_line(10, 160, "Time", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    draw_line(10, 190, "Left:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    CycleTime = 0;
    Current_Cycle_Duration = candy_cool_time;    
  }  

  time_countdown_candy(&Current_Cycle_Duration);  //draw countdown timer 
  
  if (draw_button(245, 220, 215, 90, MAIN_THEME, "ADD MORE", "DRY TIME", &Nunito_Black10pt7b)) {  //draw resume button
      setPage(4); return;
  }

  if (draw_button(20, 220, 215, 90, TFT_RED, "STOP", &Nunito_Black20pt7b)) {  //draw stop button
  //ShelfTargetTemperature = temp_temp; // Returns the heating temperature to what is was before paused
      setPage(8);
      return;
  }
  
  if (remaining <= 0) {
      setPage(8);
      return;
  }
  

  done = true;

  standardSensorCheck();
}

// Page 34
void quality_control_test()
{
    if (!done)
    {
        set_pixel_blue();
        tft.fillScreen(BACKGROUND);  //fill screen with background    
        draw_title("Quality Control Test");
        draw_line(70, "Notice: QC Test duration takes", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(90, " 1 hour and 15 minutes to complete.", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

        draw_line(120, "Ensure that the door is shut,", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(140, "and the drain valve is closed", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        CycleTime = 0;
    }

    if (draw_button(85, 235, 310, 70, MAIN_THEME, "EXIT", &Nunito_Black10pt7b))
    {
        setPage(17);
        return;
    }

    if (draw_button(55, 150, 175, 70, TFT_RED, "START", &Nunito_Black10pt7b))
    {
        test_num = 0;
        Tests.qc_test_num = 0;
        Tests.off_gas = true;
        Tests.leak_only = false;
        Tests.off_gas = true;
        Tests.heat_const = false;
        setPage(46);
        return;
    }

    if (draw_button(245, 150, 175, 70, TFT_RED, "LEAK TEST", &Nunito_Black10pt7b))
    {
        Tests.leak_only = true;
        setPage(38);
        return;
    }

    done = true;

}

//page 35
void sensor_testing()
{
    static bool shelfTestDone;
    static bool chmbrTestDone;
    static bool pressTestDone;
    static bool errorPromptDrawn;
    
    ForceHeaterON = true;
    
    if (!done)
    {
        set_pixel_blue();
        tft.fillScreen(BACKGROUND);  //fill screen with background    
        draw_title("Testing Sensors");

        draw_line(10, 90, "Shelf Sensor:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(10, 140, "Chamber Sensor:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(10, 190, "Buffer Board:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

        draw_line(335, 90, "Waiting...", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(335, 140, "Waiting...", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(335, 190, "Waiting...", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

        /// NOTE: This is pretty hacky, since draw_static_items already puts stuff in these locations
        // shelf
        tft.fillRect(325, 70, 135, 30, BACKGROUND);
        draw_line(335, 90, "In Progress...", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

        // chamber
        tft.fillRect(325, 120, 135, 30, BACKGROUND);
        draw_line(335, 140, "In Progress...", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

        // pressure
        tft.fillRect(325, 170, 135, 30, BACKGROUND);
        draw_line(335, 190, "In Progress...", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

        //count_loop = 0;
        //count_chamber_errors = 0;
        //count_shelf_errors = 0;
        //count_pressure_errors = 0;
        Tests.shelf_resistance = ShelfTemp.RTD() * RREF / 32768;
        Tests.chamber_resistance = ChmbrTemp.RTD() * RREF / 32768;

        shelfTestDone = false;
        chmbrTestDone = false;
        pressTestDone = false;

        errorPromptDrawn = false;

        CycleTime = 0;
    }

    //check shelf sensor
    if ((CycleTime > SHELF_TEMP_TIMEOUT || ShelfTemp.UpTime > (int)SHELF_TEMP_TIMEOUT_ms) && !shelfTestDone)
    {
        tft.fillRect(325, 70, 135, 30, BACKGROUND);
        draw_line(335, 90, String(ShelfTemp.SuccessRate * 100, 0) + "%", ChmbrTemp.Fault ? TFT_RED : TFT_GREEN, &Rajdhani_SemiBold12pt7b);

        shelfTestDone = true;
    }

    // check chamber sensor (same as shelf timeout for now)
    if ((CycleTime > SHELF_TEMP_TIMEOUT || ChmbrTemp.UpTime > (int)SHELF_TEMP_TIMEOUT_ms) && !chmbrTestDone)
    {
        tft.fillRect(325, 120, 135, 30, BACKGROUND);
        draw_line(335, 140, String(ChmbrTemp.SuccessRate * 100, 0) + "%", ChmbrTemp.Fault ? TFT_RED : TFT_GREEN, &Rajdhani_SemiBold12pt7b);

        chmbrTestDone = true;
    }

    // check pressure sensor
    if (((CycleTime > PRES_POLL_TIMEOUT) || (PressureReconnectTime && (millis() > PressureReconnectTime + PRES_POLL_TIMEOUT_ms))) && !pressTestDone)
    {
        tft.fillRect(325, 170, 135, 30, BACKGROUND);
        if(standardSensorCheck_pressure())   draw_line(335, 190, "Fail",    TFT_RED,   &Rajdhani_SemiBold12pt7b);
        else                                 draw_line(335, 190, "Success", TFT_GREEN, &Rajdhani_SemiBold12pt7b);

        pressTestDone = true;
    }



    if (shelfTestDone && chmbrTestDone && pressTestDone)
    {
        setPage(38);
        return;
    }


    if (draw_button( 55, 235, 175, 70, MAIN_THEME, "EXIT", &Nunito_Black10pt7b))
    {
        setPage(17);
        return;
    }
    if (draw_button(245, 235, 175, 70, MAIN_THEME, "SKIP", &Nunito_Black10pt7b))
    {
        setPage(38);
        return;
    }

    done = true;
}



// page 36
void sensor_failed__ifPassed(bool failed, String& s, int& c)
{
    if (!failed)
    {
        s = "PASS";
        c = TFT_GREEN;
    }
    else
    {
        s = "FAIL";
        c = TFT_RED;
    }
}
void sensor_failed()
{
    enum : int {
        x0 = 5,
        x1 = 195,
        x2 = 340,

        y0 = 65,
        dy = 60,

        y1 = y0 + dy,
        y2 = y1 + dy,

        y3 = y0 + 25,
        y4 = y3 + dy,
        y5 = y4 + dy,

        by = 235,
        bw = 215,
        bh = 70,
        bs = 16,

        bx1 = 240 - (bs / 2 + bw),
        bx2 = 240 + bs / 2,
    };

    if (!done)
    {
        sound = false;
        set_pixel_red();
        tft.fillScreen(BACKGROUND);
        draw_title("Sensor Test Results");
        String shelf_resist_string = String(Tests.shelf_resistance);
        String chamber_resist_string = String(Tests.chamber_resistance);
        String pass_string;
        int pass_colour;


        draw_line(x0, y0, "Shelf Sensor:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(x1, y0, "Resistance:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(x2, y0, "Success Rate:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

        draw_line(x1, y1, "Resistance:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(x0, y1, "Chamber Sensor:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(x2, y1, "Success Rate:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

        draw_line(x0, y2, "Buffer Board:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(x2, y2, "Success Rate:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

        sensor_failed__ifPassed(ShelfTemp.Fault, pass_string, pass_colour);
        draw_line(x0, y3, pass_string, pass_colour, &Rajdhani_SemiBold12pt7b);
        draw_line(x1, y3, shelf_resist_string, pass_colour, &Rajdhani_SemiBold12pt7b);
        draw_line(x2, y3, ShelfTemp.SuccessRate * 100, 1, "%", pass_colour, &Rajdhani_SemiBold12pt7b);

        sensor_failed__ifPassed(ChmbrTemp.Fault, pass_string, pass_colour);
        draw_line(x0, y4, pass_string, pass_colour, &Rajdhani_SemiBold12pt7b);
        draw_line(x1, y4, chamber_resist_string, pass_colour, &Rajdhani_SemiBold12pt7b);
        draw_line(x2, y4, ChmbrTemp.SuccessRate * 100, 1, "%", pass_colour, &Rajdhani_SemiBold12pt7b);

        sensor_failed__ifPassed(standardSensorCheck_pressure(), pass_string, pass_colour);
        draw_line(x0, y5, pass_string, pass_colour, &Rajdhani_SemiBold12pt7b);
        draw_line(x2, y5, (float)(standardSensorCheck_pressure() ? 0.0 : 100.0), 1, "%", pass_colour, &Rajdhani_SemiBold12pt7b);

    }

    //if (draw_button(bx1, by, bw, bh, TEST_BUTTON, "EXIT", &Nunito_Black20pt7b))
    //{
    //    setPage(17);
    //    return;
    //}

    if (draw_button(bx2, by, bw, bh, TEST_BUTTON, "NEXT", &Nunito_Black20pt7b))
    {
        setPage(37);
        return;
    }

    error_tone(&sound, 4);
    done = true;
}



/// page 37
void qc_test_results()
{
    if (!done)
    {
        tft.fillScreen(BACKGROUND);

        draw_title("Test Results");

        draw_line(10, 70, "Pressure Test 1:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(10, 110, "Pressure Test 2:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(10, 150, "Refrigeration Test:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(10, 185, "Cold Pressure Test:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(10, 220, "Heating Test:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);


        // Convert Values to strings for displaying
        String ambient_pressure_1_string = String(Tests.ambient_pressure_1);
        String ambient_pressure_2_string = String(Tests.ambient_pressure_2);
        String chamber_test_string = String(Tests.chamber_test_f);
        String cold_pressure_1_string = String(Tests.cold_pressure_1);
        String heating_test_string = String(Tests.heating_test_f);

        // Ambient Pressure 1
        if (Tests.ambient_pressure_1 < 1125 && Tests.ambient_pressure_1 > 50)
        {
            draw_line(410, 70, "PASS", TFT_GREEN, &Rajdhani_SemiBold12pt7b);
            draw_line(265, 70, ambient_pressure_1_string, TFT_GREEN, &Rajdhani_SemiBold12pt7b);
        }
        else if (Tests.ambient_pressure_1 == 760000)
        {
            draw_line(270, 70, "Test Not Completed", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        }
        else if(Tests.ambient_pressure_1 < 50)
        {
            draw_line(270, 70, "Disconnected", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
        }
        else
        {
            draw_line(420, 70, "FAIL", TFT_RED, &Rajdhani_SemiBold12pt7b);
            draw_line(265, 70, ambient_pressure_1_string, TFT_RED, &Rajdhani_SemiBold12pt7b);
        }

        //Ambient Pressure 2
        if (Tests.ambient_pressure_2 < 1125 && Tests.ambient_pressure_2 > 50)
        {
            draw_line(410, 110, "PASS", TFT_GREEN, &Rajdhani_SemiBold12pt7b);
            draw_line(265, 110, ambient_pressure_2_string, TFT_GREEN, &Rajdhani_SemiBold12pt7b);
        }
        else if (Tests.ambient_pressure_2 == 760000)
        {
            draw_line(270, 110, "Test Not Completed", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        }
        else if(Tests.ambient_pressure_2 < 50)
        {
            draw_line(270, 110, "Disconnected", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
        }
        else
        {
            draw_line(420, 110, "FAIL", TFT_RED, &Rajdhani_SemiBold12pt7b);
            draw_line(265, 110, ambient_pressure_2_string, TFT_RED, &Rajdhani_SemiBold12pt7b);
        }

        //Refrigeration Test
        if (Tests.chamber_test_f < -25)
        {
            draw_line(410, 150, "PASS", TFT_GREEN, &Rajdhani_SemiBold12pt7b);
            draw_line(265, 150, chamber_test_string, TFT_GREEN, &Rajdhani_SemiBold12pt7b);
        }
        else if (Tests.chamber_test_f == 255)
        {
            draw_line(270, 150, "Test Not Completed", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        }
        else if (Tests.chamber_test_f == 0)
        {
            draw_line(270, 150, "Disconnected/0", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
        }
        else {
            draw_line(420, 150, "FAIL", TFT_RED, &Rajdhani_SemiBold12pt7b);
            draw_line(265, 150, chamber_test_string, TFT_RED, &Rajdhani_SemiBold12pt7b);
        }

        //Cold Pressure 1
        if (Tests.cold_pressure_1 < 500 && Tests.cold_pressure_1 > 50)
        {
            draw_line(410, 185, "PASS", TFT_GREEN, &Rajdhani_SemiBold12pt7b);
            draw_line(265, 185, cold_pressure_1_string, TFT_GREEN, &Rajdhani_SemiBold12pt7b);
        }
        else if (Tests.cold_pressure_1 == 760000)
        {
            draw_line(270, 185, "Test Not Completed", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        }
        else if(Tests.cold_pressure_1 < 50)
        {
            draw_line(270, 185, "Disconnected", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
        }
        else
        {
            draw_line(420, 185, "FAIL", TFT_RED, &Rajdhani_SemiBold12pt7b);
            draw_line(265, 185, cold_pressure_1_string, TFT_RED, &Rajdhani_SemiBold12pt7b);
        }

        //Heating Test
        if (Tests.heating_test_f > 118 && Tests.heating_test_f < 122) {
            draw_line(410, 220, "PASS", TFT_GREEN, &Rajdhani_SemiBold12pt7b);
            draw_line(265, 220, heating_test_string, TFT_GREEN, &Rajdhani_SemiBold12pt7b);
        }
        else if (Tests.heating_test_f == 0)
        {
            draw_line(270, 220, "Test Not Completed", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        }
        else
        {
            draw_line(420, 220, "FAIL", TFT_RED, &Rajdhani_SemiBold12pt7b);
            draw_line(265, 220, heating_test_string, TFT_RED, &Rajdhani_SemiBold12pt7b);
        }
    }


    if (draw_button(20, 235, 215, 70, TEST_BUTTON, "BACK", &Nunito_Black20pt7b))
    {
        setPage(36);
        return;
    }

    if (draw_button(245, 235, 215, 70, TEST_BUTTON, "NEXT", &Nunito_Black20pt7b))
    {
        setPage(41);
        return;
    }

    done = true;
}

/// page 39
void release_pressure()
{
    static bool advance;
    if (!done)
    {
        tft.fillScreen(BACKGROUND);
        draw_title("Valve Check");
        advance = false;

        draw_line(100, "1: Release Pressure From the Drain Valve.", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(140, "2: Close Drain Valve and Continue test.", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(170, "Ensure that the door remains closed.", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        if(Tests.qc_test_num == 1)
        {
          if(Tests.ambient_pressure_1 <= 1125 && Tests.ambient_pressure_1 >= 50)
          {
            draw_line(10, 210, "Previous Pressure Result: ", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
            advance = true;
          }
          else if(Tests.ambient_pressure_1 == 760000 || Tests.ambient_pressure_1 < 50)
          {
            draw_line(10, 210, "Previous Test Result: ", TFT_YELLOW, &Rajdhani_SemiBold12pt7b); 
          }
          else
          {
            draw_line(10, 210, "Previous Pressure Result: ", TFT_RED, &Rajdhani_SemiBold12pt7b);
          }
          String test_result = String(Tests.ambient_pressure_1);
          draw_line(270, 210, test_result, TFT_WHITE, &Rajdhani_SemiBold12pt7b);
          
        }
        else if(Tests.qc_test_num == 2)
        {
          if(Tests.ambient_pressure_2 <= 1125 && Tests.ambient_pressure_2 >= 50)
          {
            draw_line(10, 210, "Previous Pressure Result: ", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
            advance = true;
          }
          else if(Tests.ambient_pressure_2 == 760000 || Tests.ambient_pressure_2 < 50)
          {
            draw_line(10, 210, "Previous Pressure Result: ", TFT_YELLOW, &Rajdhani_SemiBold12pt7b); //
          }
          else
          {
            draw_line(10, 210, "Previous Pressure Result: ", TFT_RED, &Rajdhani_SemiBold12pt7b);
          }
          String test_result2 = String(Tests.ambient_pressure_2);
          draw_line(270, 210, test_result2, TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        }
        \
        draw_line(350, 210, "mTorr", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        
    }
    //draw_pressure(250, 210);
    if(test_num == 1)
    {
      if (draw_button(20, 235, 215, 70, TEST_BUTTON, "More", "Vacuum", &Nunito_Black20pt7b))
      {
          setPage(38);
          Tests.qc_test_num -= 1;
          return;
      }
    }
    else
    {
      if (draw_button(20, 235, 215, 70, TEST_BUTTON, "EXIT", &Nunito_Black20pt7b))
      {
          setPage(17);
          return;
      }
    }

    if (draw_button(245, 235, 215, 70, TEST_BUTTON, "NEXT", &Nunito_Black20pt7b))
    {
        setPage(38);
        return;
    }
    Serial.println(advance);
    if (chamber_pressure > 10000 && advance == true)
    {
        setPage(38);
        return;
    }

    error_tone(&sound, 4);
    done = true;

}

/// Page 40
void leak_rate()
{
    if (!done)
    {
        tft.fillScreen(BACKGROUND);
        draw_line(40, "CHECKING LEAK", TFT_WHITE, &Nunito_Black20pt7b);
        draw_line(100, "RATE", TFT_WHITE, &Nunito_Black20pt7b);
        draw_line(10, 160, "Time", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(10, 190, "Left:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

        CycleTime = 0;
        Tests.leak_rate_init = chamber_pressure;
        Serial.print("Chamber Initial: ");
        Serial.println(Tests.leak_rate_init);

        Current_Cycle_Duration = leak_rate_time;
    }

    time_countdown_candy(&Current_Cycle_Duration);


    if (draw_button(20, 235, 215, 70, TEST_BUTTON, "EXIT", &Nunito_Black20pt7b))
    {
        setPage(17);
        return;
    }

    if (draw_button(245, 235, 215, 70, TEST_BUTTON, "NEXT", &Nunito_Black20pt7b))
    {
        if(Tests.leak_only)
        {
          setPage(41);
          return;
        }
        else
        {
          setPage(48);
          return;
        }
    }

    if (remaining <= 0)
    {
        Tests.leak_rate_final = chamber_pressure;

        Tests.leak_rate_test = ((Tests.leak_rate_final - Tests.leak_rate_init) / 120);
        Serial.print("Chamber Pressure: ");
        Serial.println(chamber_pressure);

        Serial.print("Leak Rate millitorrs per second: ");
        Serial.println(Tests.leak_rate_test);

        if (Tests.leak_rate_init - chamber_pressure > 900)
        {
            Serial.print("Leak Rate too HIGH");
        }

        if(Tests.leak_only)
        {
          setPage(41);
          return;
        }
        else
        {
          setPage(48);
          return;
        }
    }

    done = true;
}

/// TODO: Verify leak rate is being calculated correctly
// page 41
void display_leak_rate()
{
    if (!done)
    {
        tft.fillScreen(BACKGROUND);

        draw_line(40, "LEAK RATE", TFT_WHITE, &Nunito_Black20pt7b);
        draw_line(75, "mTorr / Second", TFT_WHITE, &Nunito_Black20pt7b);
        draw_line(70, 215, "Chamber Pressure: ", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

        draw_line(95, 100, "Passing Rate is Under 8 mTorr ", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

        // Convert Values to strings for displaying
        String leak_rate_string = String(Tests.leak_rate_test);

        //Refrigeration Test
        if (Tests.leak_rate_test < 8 && Tests.leak_rate_test > 0)
        {
            draw_line(325, 175, "PASS", TFT_GREEN, &Rajdhani_SemiBold12pt7b);
            draw_line(75, 175, leak_rate_string, TFT_GREEN, &Nunito_Black30pt7b);
        }
        else if (Tests.leak_rate_test <= 0)
        {
            draw_line(140, "Leak Rate Under 0, This typically indicates", TFT_RED, &Rajdhani_SemiBold12pt7b);
            draw_line(160, "that system is not under vacuum.", TFT_RED, &Rajdhani_SemiBold12pt7b);
            draw_line(180, "Please test again..", TFT_RED, &Rajdhani_SemiBold12pt7b);
        }
        else
        {
            draw_line(335, 175, "FAIL", TFT_RED, &Rajdhani_SemiBold12pt7b);
            draw_line(75, 175, leak_rate_string, TFT_RED, &Nunito_Black30pt7b);
        }

    }

    draw_pressure(290, 215);


    if (Tests.leak_only == true)
    {
        if (draw_button(90, 235, 310, 70, TEST_BUTTON, "Exit", &Nunito_Black20pt7b))
        {
            setPage(17);
            return;
        }
    }
    else
    {
        if (draw_button(20, 235, 215, 70, TEST_BUTTON, "BACK", &Nunito_Black20pt7b))
        {
            setPage(37);
            return;
        }

        if (draw_button(245, 235, 215, 70, TEST_BUTTON, "NEXT", &Nunito_Black20pt7b))
        {
            setPage(44);
            return;
        }
    }

    done = true;
}

/*********************************************************************************
* Page 42: Pre-warm candy option
* This option is for whenever a candy recipe is used, it asks if they want to
* pre-warm there candy. Also gives options for time and temp
* If yes, it goes to the Pre-warming candy page. If no, bypass to freeze
**********************************************************************************/
void pre_warm_option(){  // Page 42

    if(!done){
      set_pixel_blue();
      tft.fillScreen(BACKGROUND);  //fill screen with background    
      draw_title("Pre-warm Candy?");
      draw_line(10, 155, "Dry/PreWarm", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
      draw_line(10, 185, "Temperature:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
      draw_line(10, 80, "Warm Time:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
      draw_line(10, 110, "Hours : Minutes", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    }
    candy_pre_warm_temp = temp_changer_manual(185, 140, candy_pre_warm_temp, 74, 30, done);  // displays the temperature then brings back the potentially changed value

    current_recipe.dry_temp = candy_pre_warm_temp;

    prewarm_changer(0, 60, &candy_warm_time, &prev_candy_warm_time, &candy_selector, &prev_candy_selector);  //allows the modification of the time for warming

    if (draw_button(20, 210, 215, 90, MAIN_THEME, "No", &Nunito_Black20pt7b)) {  //No button
        setPage(4);
        return;
    }
    if (draw_button(245, 210, 215, 90, MAIN_THEME, "Yes", &Nunito_Black20pt7b)) {  //Yes button
        setPage(43);
        return;
    }

    done = true;
}

/*********************************************************************************
* Page 43: Pre-warming candy
* warms candy when selected from Page 42
* Moves on to page 4 after
**********************************************************************************/

void pre_warming_candy() {
    if (!done) {
        Serial.println("!Done");
        set_pixel_blue();
        tft.fillScreen(BACKGROUND);  //fill screen with background    
        draw_title("Pre-Warming Candy");
        draw_line(10, 80, "Time", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(10, 110, "Remaining:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(10, 155, "Chamber Temp:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(10, 195, "Shelf Temp:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        CycleTime = 0;
        recipe_cycle = true;
        Current_Cycle_Duration = candy_warm_time;  //this allows us to change our time but not mess with the default
  //start_warm_count = false;
        

    }

    hardware_control(0, 1, 0);
    time_countdown_manual(&Current_Cycle_Duration, 0, 0, -10);  //draw countdown timer with candy warm time

    if (remaining <= 0) {
        start_warm_count = false;
        setPage(4);
        return;
    }


    ChmbrTemp.Draw(203, 154);
    ShelfTemp.Draw(203, 195);

    if (draw_button(245, 220, 215, 90, MAIN_THEME, "Continue to", "Dry Cycle", &Nunito_Black10pt7b)) {  //Continue button to skip pre-warming
        start_warm_count = false;
        setPage(4);
        return;
    }

    if (draw_button(20, 220, 215, 90, TFT_RED, "Exit", &Nunito_Black20pt7b)) {  //draw exit button
  //ShelfTargetTemperature = temp_temp; // Returns the heating temperature to what is was before paused
        start_warm_count = false;
        setPage(18);
        return;
    }

    done = true;
    
    standardSensorCheck();
}

/*********************************************************************************
* Page 44: qc_test_warning
* QC warning, Will remind user to record data before exiting.
**********************************************************************************/
void qc_test_warning()
{
  if(!done)
  {
    tft.fillScreen(BACKGROUND);

    draw_title("WARNING");
    draw_line(80, "Data will not be saved.", TFT_RED, &Nunito_Black20pt7b);
    draw_line(120, "Only proceed if all the", TFT_WHITE);
    draw_line(160, "Quality Control Test", TFT_WHITE);
    draw_line(200, "data has been recorded.", TFT_WHITE);
  }   
  
  if(draw_button( 20, 210, 215, 90, MAIN_THEME, "BACK", &Nunito_Black20pt7b)){
      setPage(41);
      return;
  } 
  if(draw_button(245, 210, 215, 90, MAIN_THEME, "FINISH", &Nunito_Black20pt7b)){
      setPage(1);
      return;
  }  

  //hardware_serial_monitor(1, 15000); 
  
  done = true;
}

/*********************************************************************************
* Page 46: ground_fault_checker
* This function is meant to test the hardware before the rest of our tests, if
* we have a grounding issue the machine will reset on the hardware with the 
* grounding issue.
**********************************************************************************/
void ground_fault_checker()
{
    static bool fridgeStatusDrawn;
    static bool fridgeHardwareDrawn;
    static bool vacuumStatusDrawn;
    static bool vacuumHardwareDrawn;
    static bool heatingStatusDrawn;
    static bool heatingHardwareDrawn;
    static bool fanStatusDrawn;
    static bool fanHardwareDrawn;
    static bool drawnComplete;
    static bool faultSound;

    enum {
        // column starts, first row, row separation
        x0 = 47,
        x1 = 308,
        y0 = 90,
        dy = 40,

        y1 = y0 + dy,
        y2 = y1 + dy,
        y3 = y2 + dy,

        // bounding-box offsets
        w = 125,
        h0 = 5,
        h1 = -21,

        by = 0,
        bw = 0,
    };

    if (!done)
    {
        fridgeStatusDrawn    = false;
        fridgeHardwareDrawn  = false;
        vacuumStatusDrawn    = false;
        vacuumHardwareDrawn  = false;
        heatingStatusDrawn   = false;
        heatingHardwareDrawn = false;
        fanStatusDrawn       = false;
        fanHardwareDrawn     = false;
        drawnComplete        = false;

        ForceHeaterON = false;

        tft.fillScreen(BACKGROUND);
        draw_title("Checking Ground");

        draw_line(x0, y0, "Refrigeration:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(x0, y1, "Vacuum Pump:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(x0, y2, "Heating Shelves:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(x0, y3, "Chamber Fan:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

        draw_line(x1, y0, "Waiting...", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(x1, y1, "Waiting...", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(x1, y2, "Waiting...", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(x1, y3, "Waiting...", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

        CycleTime = 0;
    }


    // Start progress after 2 seconds
    if (CycleTime >= 2_sec)
    {
        // fill shelf Status
        if (!fridgeStatusDrawn)
        {
            tft.fillRect(x1, y0+h0, w, h1, BACKGROUND);
            draw_line(x1, y0, "In Progress...", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

            fridge_fault_status = true;
            vacuum_fault_status = false;
            fan_fault_status = false;
            heat_fault_status = false;

            store_status();

            fridgeStatusDrawn = true;
        }
    }

    // Start progress after 4 seconds
    if (CycleTime >= 4_sec)
    {
        // fill shelf Status
        if (!fridgeHardwareDrawn)
        {

            hardware_control(0, 1, 0);  // Fridge On
            fridgeHardwareDrawn = true;
        }
    }

    // Start progress after 14 seconds
    if (CycleTime >= 14_sec)
    {
        // fill shelf Status
        if (!vacuumStatusDrawn)
        {

            hardware_control(0, 0, 0);  // All Hardware Off

            tft.fillRect(x1, y0+h0, w, h1, BACKGROUND);
            draw_line(x1, y0, "Completed", TFT_GREEN, &Rajdhani_SemiBold12pt7b);

            tft.fillRect(x1, y1+h0, w, h1, BACKGROUND);
            draw_line(x1, y1, "In Progress...", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

            fridge_fault_status = false;
            vacuum_fault_status = true;
            fan_fault_status = false;
            heat_fault_status = false;

            store_status();
            vacuumStatusDrawn = true;
        }
    }

    // Start progress after 16 seconds
    if (CycleTime >= 16_sec)
    {
        // fill shelf Status
        if (!vacuumHardwareDrawn)
        {

            hardware_control(0, 0, 1);  // Vacuum On
            vacuumHardwareDrawn = true;
        }
    }

    // Start progress after 26 seconds
    if (CycleTime >= 26_sec)
    {
        // fill shelf Status
        if (!heatingStatusDrawn)
        {

            hardware_control(0, 0, 0);

            tft.fillRect(x1, y1+h0, w, h1, BACKGROUND);
            draw_line(x1, y1, "Completed", TFT_GREEN, &Rajdhani_SemiBold12pt7b);

            tft.fillRect(x1, y2+h0, w, h1, BACKGROUND);
            draw_line(x1, y2, "In Progress...", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

            fridge_fault_status = false;
            vacuum_fault_status = false;
            fan_fault_status = false;
            heat_fault_status = true;

            store_status();
            heatingStatusDrawn = true;
        }
    }

    // Start progress after 28 seconds
    if (CycleTime >= 28_sec)
    {
        // fill shelf Status
        if (!heatingHardwareDrawn)
        {

            //  ShelfTargetTemperature = 1000;
            //hardware_control(0,0,0); // Heat On
            ForceHeaterON = true;
            heatingHardwareDrawn = true;
        }
    }

    // Start progress after 38 seconds
    if (CycleTime >= 38_sec)
    {
        ForceHeaterON = false;

        // fill shelf Status
        if (!fanStatusDrawn)
        {

            hardware_control(0, 0, 0);

            tft.fillRect(x1, y2+h0, w, h1, BACKGROUND);
            draw_line(x1, y2, "Completed", TFT_GREEN, &Rajdhani_SemiBold12pt7b);

            tft.fillRect(x1, y3+h0, w, h1, BACKGROUND);
            draw_line(x1, y3, "In Progress...", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

            fridge_fault_status = false;
            vacuum_fault_status = false;
            fan_fault_status = true;
            heat_fault_status = false;

            store_status();
            fanStatusDrawn = true;
        }
    }

    // Start progress after 40 seconds
    if (CycleTime >= 40_sec)
    {
        // fill shelf Status
        if (!fanHardwareDrawn)
        {

            hardware_control(1, 0, 0);  // Fan On
            fanHardwareDrawn = true;
        }
    }

    // Start progress after 50 seconds
    if (CycleTime >= 50_sec)
    {
        // fill shelf Status
        if (!drawnComplete)
        {
            hardware_control(0, 0, 0);

            tft.fillRect(x1, y3+h0, w, h1, BACKGROUND);
            draw_line(x1, y3, "Completed", TFT_GREEN, &Rajdhani_SemiBold12pt7b);

            fridge_fault_status = false;
            vacuum_fault_status = false;
            fan_fault_status = false;
            heat_fault_status = false;

            store_status();
            drawnComplete = true;
        }
    }

    // on SKIP, complete test instantly
    if (draw_button(140, 235, 200, 65, MAIN_THEME, "SKIP", &Nunito_Black10pt7b))
    {
        CycleTime = 51_sec;
    }

    if (CycleTime >= 52_sec)
    {
        setPage(35);
        return;
    }

    done = true;
}


/*********************************************************************************
* Page 48: checkPressureSensorAccuracy
* This function is meant to test if our machine pressure value is within 10% of
* the value shown on an external pressure monitor.
**********************************************************************************/
void checkPressureSensorAccuracy()
{
  if(!done){            
    tft.fillScreen(BACKGROUND);  //fill screen with background    
    draw_title("Check Sensor Accuracy");
    draw_line(80, "To ensure that the sensor is accurate,", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    draw_line(105, "please verify that the pressure sensor", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    draw_line(130, "measures within 10\% of the measured", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    draw_line(155, "value from an external pressure monitor.", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    draw_line(70, 215, "Chamber Pressure: ", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
  }   

  draw_pressure(290, 215);


  if(draw_button(90, 235, 310, 70, TEST_BUTTON, "Proceed", &Nunito_Black20pt7b)){  //Start button
      prev_page = 40;

      if (Tests.leak_only == true)
      {
          setPage(41);
          return;
      }
      else
      {
          setPage(39); return;
      }

  }  

  error_tone(&sound, 4);
  done = true;
}
