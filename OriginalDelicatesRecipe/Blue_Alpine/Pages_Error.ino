/*********************************************************************************

This file contains our functions dealing with errors. 

Functions in this section are:

power_lost: When running cycles the cycle state will be saved into memory so that on powerup
you can detect power outages. We are currently holding on an error page and keeping 
the food cold under refrigeration. The indicator light will blink red if a power
outage was detected.

vacuum_leak_error: Throws up an error page if the pressure sensor detects an error. This also turns off
everything except the fridge. 

cooling_error: Throws up an error page if it detects a cooling error. This leaves on the fan and fridge.

fatal_sensor_error: This function will draw the error screen when a sensor is disconnected, the onboard
LED is setup to blink for the different errors. If the pressure sensor triggers it
can self resolve, the cycle will not resume itself on temperature sensor errors.

**********************************************************************************/



/*********************************************************************************
Page 11: Power Lost

When running cycles the cycle state will be saved into memory so that on powerup
you can detect power outages. We are currently holding on an error page and keeping 
the food cold under refrigeration. The indicator light will blink red if a power
outage was detected.
**********************************************************************************/
void power_lost() {
    if (!done) {
        tft.fillScreen(BACKGROUND);
        draw_line(50, "POWER LOST DURING CYCLE", TFT_RED, &Nunito_Black14pt7b);
        draw_line(125, "Do you want to restart", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
        draw_line(165, "your previous cycle?", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    }

    if (millis() % 2000 >= 1000)
    {
        set_pixel_red();
    }
    else
    {
        pixels.clear();
        pixels.show();
    }

    if (draw_button(245, 220, 215, 90, MAIN_THEME, "RESTART", "CYCLE", &Nunito_Black10pt7b))  //draw skip freeze cycle button 
    {
        reset_values();
        setPage(3);
        return;
    }
    if (draw_button(20, 220, 215, 90, TFT_RED, "HOME", &Nunito_Black20pt7b))  //draw stop button + store elapsed time
    {
        setPage(1);
        return;
    }

    done = true;
}


// page 14
void vacuum_leak_error()
{
    const Time retry_delay_off = 3_min;
    const Time retry_delay_on  = 5_min;
    const unsigned max_retries = 5;

    //static unsigned long error_called_time;
    static Time error_thrown_time;
    //static unsigned long retry_time;
    static int retry_count;

    enum {
        x0 = 10,

        dy1 = 23,
        y0 = 60,
        y1 = y0 + dy1,
        y2 = y1 + dy1,

        dy2 = 20,
        y3 = 140,
        y4 = y3 + dy2,
        y5 = y4 + dy2,

        x1 = 20,
        y6 = 210,
    };

    if (!done)
    {
        error_thrown_time = CycleTime;
        CycleTime = 0;

        retry_count = 0;
        sound = false;

        tft.fillScreen(BACKGROUND);
        draw_title("Vacuum Pressure Error", &Nunito_Black14pt7b);
        draw_line(y0, "Optimal chamber pressure is not being achieved.", TFT_RED, &Rajdhani_SemiBold12pt7b);
        draw_line(y1, "Machine will attempt to bring down pressure to", TFT_RED, &Rajdhani_SemiBold12pt7b);
        draw_line(y2, "acceptable levels.", TFT_RED, &Rajdhani_SemiBold12pt7b);
        draw_line(x0, y3, "-Ensure that all hose fittings are tightly fastened.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(x0, y4, "-Ensure that the door and drain valve are closed.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(x0, y5, "-Ensure there is no liquid water in chamber or drain hose.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);

        draw_line(x1, y6, "Pressure when error thrown: ", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_pressure(x1+215, y6);


        Vaccum_Error_Reason = VACCUM_ERROR_DEFAULT;
        digitalWrite(VAC, 0);
    }


    // if the pressure does return to a good value, return to where we want to be.
    if (inval_press_time() < 2000)
    {
        unpause(error_thrown_time);
        return;
    }


    if(retry_count < max_retries)
    {
        if (digitalRead(VAC))
        {
            if (CycleTime > retry_delay_on)
            {
                CycleTime = 0;
                digitalWrite(VAC, 0);
                retry_count++;
            }
        }
        else
        {
            if (CycleTime > retry_delay_off)
            {
                CycleTime = 0;
                digitalWrite(VAC, 1);
            }
        }
    }



    if (draw_button(20, 220, 215, 90, TFT_RED, "HOME", &Nunito_Black20pt7b))
    {
        store_elapsed();
        setPage(1);
        return;
    }

    if (draw_button(245, 220, 215, 90, COMP_THEME, "RESUME", &Nunito_Black20pt7b))
    {
        //_temp = temp_temp; // Returns the heating temperature to what is was before paused   
        // So it doesn't throw an error imediately after resuming
        dry_cycle_status = false;
        unpause(error_thrown_time);
        return;
    }

    error_tone(&sound, 8);
    done = true;
}


/*********************************************************************************
Page 16: Cooling Error


**********************************************************************************/
void cooling_error() {

    if (!done)
    {
        tft.fillScreen(BACKGROUND);
        draw_title("Cooling Error", &Nunito_Black14pt7b);
        draw_line(60, "Optimal chamber cooling", TFT_RED, &Nunito_Black14pt7b);
        draw_line(85, "is not being achieved.", TFT_RED, &Nunito_Black14pt7b);
        draw_line(110, "Options: ", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(10, 130, "-Ensure that the chamber door is closed.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(10, 150, "-Clean the condenser to ensure it is free of dust.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(10, 170, "-Check condenser fan operation.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(10, 190, "-Manually check temperature of chamber.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(10, 210, "-Contact Blue Alpine Technical Support.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        ChmbrTemp.Begin();
        //hardware_control(1, 1, 0);  //keep fan and refrigeration on
    }

    //go back to freeze page when errors are fixed
    if (!cooling_checker(Time(3_hr), -5))
    {
        unpause();
        return;
    }

    if (draw_button(20, 220, 215, 90, MAIN_THEME, "HOME", &Nunito_Black20pt7b))
    {
        store_elapsed();
        setPage(16);
        return;
    }

    if (draw_button(245, 220, 215, 90, TFT_RED, "OVERRIDE", &Nunito_Black20pt7b))
    {
        IgnoreChmbrTemp = true;
        /// TODO: restore time??
        unpause();
        ///page = 3; done = false; delay(250);
        return;
    }

    done = true;

}

/*
Page 23: Sensor Error

This page is called when an active page (cycle is running) detects that one of the sensors has been disconnected for too long
If all sensors start working again, this page will return us to wherever we were previously
The cycle time when this page is loaded, and restored when the page returns so the user can resume their cycle
If there is only a small about of time remaining, we'll just finish the cycle
*/
void fatal_sensor_error()
{
    // storing the previous statuses helps limit drawing
    static bool pressureFault;
    static bool chamberFault;
    static bool shelfFault;
    static Time pausedRTC;

    enum {
        Y1 = 35, // Sensor error!
        Y4 = Y1 + 32, // Cycle paused because...
        Y2 = 130, // status
        Y3 = 220, // Consult owner's manual...

        X1 = 20,
        X2 = 190,
        X3 = 335,

        W0 = 131,
    };

    if (!done)
    {
        /// TODO: this doesn't work if the cycle hasn't started yet! What to do about that?
        /// Switch on page?
        /// Set remainng to a very large number?
        //if (remaining < 10_min)
        //{
        //    // pages should be checking this before they check sensors, so this is a safe(ish) way to end the cycle
        //    remaining = 0;
        //
        //    // no need to redraw the whole screen if we haven't overwritten anything yet
        //    done = true;
        //
        //    page = prev_page;
        //    delay(250);
        //    return;
        //}

        pausedRTC = CycleTime;

        hardware_control(digitalRead(FAN), digitalRead(FRIDGE), 0);

        pixels.clear();
        pixels.show();


        sound = false;
        tft.fillScreen(BACKGROUND);
        draw_line(    Y1,    "Sensor error!", TFT_RED, &Nunito_Black14pt7b);
        draw_line(X1, Y4+00, "Cycle paused because sensors are offline.", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(X1, Y4+22, "Cycle will resume if sensors are reconnected.", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        
        /// OR: "the following sensors are disconnected:" ...
        draw_line(X1, Y2+00, "Chamber Temp.:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(X1, Y2+22, "Shelf Temp.:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(X1, Y2+44, "Vacuum Sensor:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

        draw_line(X1, Y3+00, "Consult owner's manual and", TFT_CYAN, &Rajdhani_SemiBold12pt7b);
        draw_line(X1, Y3+22, "reconnect the disconnected", TFT_CYAN, &Rajdhani_SemiBold12pt7b);
        draw_line(X1, Y3+44, "sensor. If problem persists,", TFT_CYAN, &Rajdhani_SemiBold12pt7b);
        draw_line(X1, Y3+66, "contact Blue Alpine Support.", TFT_CYAN, &Rajdhani_SemiBold12pt7b);

        draw_line(5, 310, SOFTWARE_VERSION, TFT_WHITE, &Rajdhani_SemiBold9pt7b);
    }


    ChmbrTemp.Draw(X3, Y2, true);
    ShelfTemp.Draw(X3, Y2 + 22, true);
    draw_pressure (X3, Y2 + 44, true);


    if (draw_button(325, 220, 120, 70, TFT_RED, "HOME", &Nunito_Black14pt7b))
    {
        setPage(1);
        return;
    }

    /// TODO: We can't use standard error checking here becasue standard error checks no not include hysteresis
    ///       We can sort of use the opposite (ie, uptime > +TIMEOUT_ms)

    // using Fault for hysteresis

    /// TODO: show someething diffrent for chamber if INCL_CHMBR_IN_STD_CHECK is undefined?
    if (!done || !chamberFault != !ChmbrTemp.Fault)
    {
        chamberFault = ChmbrTemp.Fault;

        tft.fillRect(X2, Y2+2, W0, -20, BACKGROUND); // NOTE: charecters like g extend 3 pixels below the datum

        if (chamberFault)   draw_line(X2, Y2, "Disconnected", TFT_RED, &Rajdhani_SemiBold12pt7b);
        else                draw_line(X2, Y2, "Connected",  TFT_GREEN, &Rajdhani_SemiBold12pt7b);
    }

    if (!done || !shelfFault != !ShelfTemp.Fault)
    {
        shelfFault = ShelfTemp.Fault;

        tft.fillRect(X2, Y2+24, W0, -20, BACKGROUND);
        if (shelfFault)     draw_line(X2, Y2+22, "Disconnected", TFT_RED, &Rajdhani_SemiBold12pt7b);
        else                draw_line(X2, Y2+22, "Connected",  TFT_GREEN, &Rajdhani_SemiBold12pt7b);
    }

    if (!done || pressureFault != standardSensorCheck_pressure())
    {
        pressureFault = standardSensorCheck_pressure();

        tft.fillRect(X2, Y2+46, W0, -20, BACKGROUND);
        if (pressureFault)  draw_line(X2, Y2+44, "Disconnected", TFT_RED, &Rajdhani_SemiBold12pt7b);
        else                draw_line(X2, Y2+44, "Connected",  TFT_GREEN, &Rajdhani_SemiBold12pt7b);
    }


    // timing for blinking LED
    switch ((millis() / 2400) % 3)
    {
    case 0:
        if (pressureFault)
            set_pixel_red();
        break;

    case 1:
        if (chamberFault)
            set_pixel_yellow();
        break;

    case 2:
        if (shelfFault)
            set_pixel_purple();
        break;
    }

    error_tone(&sound, 8);


    done = true;


    // unpause if all sensors have reconnected

#ifdef INCL_CHMBR_IN_STD_CHECK
    if (!(pressureFault || shelfFault || chamberFault))
#else
    if (chamberFault || !testingErrorPage)
    {
        // list the pages that we should NOT retun to if the chamber sensor is disconnected
        switch (abs(prev_page)) {
        case 20: // diagnostic tests
            return; // at least one sensor is out; no need to check the other two
        }
    }


    if (!(pressureFault || shelfFault))
#endif
    {
        unpause(pausedRTC);
    }
}


void ground_fault_detected()
{
    if (!done)
    {
        tft.fillScreen(BACKGROUND);
        draw_line(40, "Ground Error", TFT_RED, &Nunito_Black20pt7b);

        draw_line(80, "You have a fault that reset the machine.", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(100, "This is most likely improper grounding, please", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(120, "check the wiring for the hardware below.", TFT_WHITE, &Rajdhani_SemiBold12pt7b);


        if (fridge_fault_status) {
            draw_line(170, "Refrigeration", TFT_GREEN, &Nunito_Black20pt7b);
        }
        else if (vacuum_fault_status) {
            draw_line(170, "Vacuum Pump", TFT_GREEN, &Nunito_Black20pt7b);
        }
        else if (fan_fault_status) {
            draw_line(170, "Chamber Fan", TFT_GREEN, &Nunito_Black20pt7b);
        }
        else if (heat_fault_status) {
            draw_line(170, "Heating Shelves", TFT_GREEN, &Nunito_Black20pt7b);
        }
    }

    if (draw_button(80, 210, 315, 90, MAIN_THEME, "EXIT", &Nunito_Black20pt7b))
    {
        fridge_fault_status = false;
        vacuum_fault_status = false;
        fan_fault_status = false;
        heat_fault_status = false;
        store_status();

        setPage(1);
        return;
    }

    error_tone(&sound, 8);
    done = true;
}

//page 50
void popUpError()
{
  static String title = " ";
  static String line2 = " ";
  static String line3 = " ";
  static String line4 = " ";
  static String line5 = " ";
  if (!done) {  //draw test box over current screen
      sound = false;

      pixels.clear();
      pixels.show();

      tft.fillRoundRect(50, 50, 380, 220, 10, MAIN_THEME);
      tft.drawRoundRect(50, 50, 380, 220, 10, TFT_WHITE);
    }

  if (invisible_touch_test() /*|| !sensorFault*/)
  {
      pixels.clear();
      pixels.show();
      press_button();
      setPage(prev_page);
      throwPopUp = false;
      return;
  }

  set_pixel_red();

  switch(prev_page)
  {
    case 4:
    case 12:    
      title = "VACUUM PUMP ERROR";
      line2 = "Ensure the pressure valve is shut";
      line3 = "Ensure vacuum pump switch is active";
      line4 = "Ensure vacuum pump is plugged in";
      line5 = "Ensure hose is properly connected";
      break;
  }

    draw_line(90, title, TFT_RED, &Nunito_Black14pt7b);
    draw_line(55, 120, line2, TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    draw_line(55, 150, line3, TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    draw_line(55, 180, line4, TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    draw_line(55, 210, line5, TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    draw_line(260, "Click anywhere to continue", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
    //draw_line(395, 80, "X", TFT_WHITE, &Rajdhani_SemiBold9pt7b);

    done = true;
}



