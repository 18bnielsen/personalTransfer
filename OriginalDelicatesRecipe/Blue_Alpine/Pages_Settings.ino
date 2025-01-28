/*********************************************************************************
Functions in this section are:

settings_page:
defaults: Default Freeze and Dry Times
defaults_1: Default Dry Temp and Pressure Settings
defaults_1_warning: 
defaults_2: Unit Preferences
defaults_3: Sound Settings
defaults_4: Factory Reset
defaults_5: Defrost Fan Setting
manual_tests:
save_changes:
diagnostics:
test_pages:
results:
**********************************************************************************/

void settings_page(){  // Page 17

  generate_files();  // generate files if none are present on filesystem                                                                       
  f_reset = false;  // set factory reset to false
  reset_values();  // reset values by pulling from filesystem
  
  if (!done) {
      tft.fillScreen(BACKGROUND);
      draw_title_two("SETTINGS", "PAGE", &Nunito_Black14pt7b);
      //Defaults button label
      draw_line(50, 230, "Edit Default", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
      draw_line(42, 250, "Dryer Settings", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
      draw_line(200, 230, "Diagnostic", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
      draw_line(220, 250, "Tests", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
      //diagnostics button
      draw_line(360, 230, "Manual", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
      draw_line(325, 250, "Hardware Control", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
  }  
  
  // QC Auto Tests button
  if (invisible_button(365, 3, 110, 70)) {  //exit button
      setPage(34);
      return;
  }

  if(draw_button(5, 3, 110, 70, MAIN_THEME, "EXIT", &Nunito_Black10pt7b)){  // exit button
      setPage(1);
      return;
  }
  if(draw_button(40, 110, 105, 105, COMP_THEME, " ", &Nunito_Black10pt7b)){  // defaults button
      setPage(10);
      return;
  }
 if(draw_button(185, 110, 105, 105, COMP_THEME, " ", &Nunito_Black10pt7b)){  // diagnostics button
      setPage(19);
      return;
  }
  if(draw_button(330, 110, 105, 105, COMP_THEME, " ", &Nunito_Black10pt7b)){  // hardware control button
      setPage(12);
      return;
  }
 
  if(!done){  // button images
    tft.drawBitmap(343, 123, manual, 80, 80, TFT_WHITE);
    tft.drawBitmap(198, 123, diagnose, 80, 80, TFT_WHITE);
    tft.drawBitmap(53, 123, edit, 80, 80, TFT_WHITE);    
  }
   
  int elapsed_day, elapsed_hr, elapsed_min;  // elapsed time variables  
  get_elapsed(&elapsed_hr, &elapsed_day, &elapsed_min);  // get elapsed prev values
  draw_line(5, 310, SOFTWARE_VERSION, TFT_WHITE, &Rajdhani_SemiBold9pt7b);  // draw version number 
  draw_line(180, 310, "Cycle Run-Time:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);  // draw time on info
  draw_line(350, 310, elapsed_day, 0, "Days",  TFT_WHITE, &Rajdhani_SemiBold9pt7b);
  draw_line(420, 310, elapsed_hr, 0, "Hrs",  TFT_WHITE, &Rajdhani_SemiBold9pt7b);  
    
  //draw_line(410, 310, elapsed_min, 0, "Mins",  TFT_WHITE, &Rajdhani_SemiBold9pt7b); // Minutes not displayed but still saved for good time keeping
  
  done = true;
}


// Page 10
void defaults()
{

    if (!done)
    {
        tft.fillScreen(BACKGROUND);
        draw_title_two("DEFAULT", "CYCLE TIMES", &Nunito_Black14pt7b);

        draw_line(5, 115, "Default",      TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(5, 140, "Freeze Time:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(5, 190, "Default",      TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(5, 215, "Dry Time:",    TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    }

    //changer for the default times
    default_changer(0,  90, &freeze_time, &prev_freeze_time, &freeze_selector, &prev_freeze_selector);
    default_changer(0, 165, &dry_time,    &prev_dry_time,    &dry_selector,    &prev_dry_selector);

    if (draw_button(365, 3, 110, 70, MAIN_THEME, "NEXT", "PAGE", &Rajdhani_SemiBold12pt7b))
    {
        setPage(24);
        return;
    }

    if (draw_button(20, 235, 215, 80, TFT_RED, "SAVE", "& EXIT", &Nunito_Black14pt7b))
    {
        store_values();
        setPage(17);
        return;
    }

    if (draw_button(245, 235, 215, 80, MAIN_THEME, "DISCARD", "CHANGES", &Nunito_Black14pt7b))
    {
        reset_values();
        setPage(17);
        return;
    }

    if (!done)
    {
        // draw button arrow
        tft.drawBitmap(453, 13, right_arrow, 20, 50, TFT_WHITE);
    }

    done = true;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* DEFAULTS_1: 
* Default Dry Temp and Pressure Settings: Page 24
* 
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void defaults_1()
{
    if (!done)
    {
        tft.fillScreen(BACKGROUND);

        draw_title_two("DEFAULT", "DRY SETTINGS", &Nunito_Black14pt7b);
        draw_line(5, 115, "Default", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(5, 140, "Dry Temperature:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(5, 190, "Default", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

        switch (pressure_unit) {
        case 0: draw_line(5, 215, "Set Pressure(Pa):", TFT_WHITE, &Rajdhani_SemiBold12pt7b); break;
        case 1: draw_line(5, 215, "Set Pressure(mT):", TFT_WHITE, &Rajdhani_SemiBold12pt7b); break;
        case 2: draw_line(5, 215, "Set Pressure(mB):", TFT_WHITE, &Rajdhani_SemiBold12pt7b); break;
        case 3: draw_line(5, 215, "Set Pressure(PSI):", TFT_WHITE, &Rajdhani_SemiBold12pt7b); break;
        }
    }

    if (draw_button(5, 3, 110, 70, MAIN_THEME, "PREV.", "PAGE", &Rajdhani_SemiBold12pt7b))
    {
        setPage(10);
        return;
    }

    if (draw_button(365, 3, 110, 70, MAIN_THEME, "NEXT", "PAGE", &Rajdhani_SemiBold12pt7b))
    {
        setPage(25);
        return;
    }

    if (draw_button(20, 235, 215, 80, TFT_RED, "SAVE", "& EXIT", &Nunito_Black14pt7b))
    {
        store_values();
        setPage(17);
        return;
    }

    if (draw_button(245, 235, 215, 80, MAIN_THEME, "DISCARD", "CHANGES", &Nunito_Black14pt7b))
    {
        reset_values();
        setPage(17);
        return;
    }

    //draw button arrows
    if (!done)
    {
        tft.drawBitmap(8, 13,   left_arrow,  20, 50, TFT_WHITE);
        tft.drawBitmap(453, 13, right_arrow, 20, 50, TFT_WHITE);
    }

    shelf_temp_changer(185, 410, 275, 90);

    bool pressWasSafe = !pressureSettingUnsafe(set_pressure);
    set_pressure_changer(185, 410, 275, 165);

    /* Eliminating this for future revisions
    if (pressWasSafe && pressureSettingUnsafe(set_pressure))
    {
        setPage(28);
        return;
    } */

    done = true;


}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* DEFAULTS_1_WARNING:   Page 28
* Only called from pages 24 & 30
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
void defaults_1_warning()
{
    enum {
        y0 = 120,
        dy = 20,

        y1 = 235,
        x0 = 240,
        x1 =  17,
        w1 = 140,
        h1 =  40,

        h2 = 65,
        h3 = 240
    };

    static int x;
    if (!done)
    {

        tft.fillRoundRect(50, y0 - h2, 380, h3, 10, MAIN_THEME);
        tft.drawRoundRect(50, y0 - h2, 380, h3, 10, TFT_WHITE);
        draw_line(y0 - 30, "WARNING", TFT_RED, &Nunito_Black14pt7b);

        int i = 0; // line
        x = abs(prev_page) == 30 ? pressureSettingUnsafe(current_recipe.pressure) : pressureSettingUnsafe(set_pressure);

        switch(x)
        {
        case 1:
            draw_line(y0 + dy * i++, "Going above our recommended pressure limit can",    TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(y0 + dy * i++, "result in a shorter dry cycle times as well as ",   TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(y0 + dy * i++, "more aggressive heating of the product. Increased", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(y0 + dy * i++, "pressure is also related to an increase in water ", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(y0 + dy * i++, "ingested by the vacuum pump. Only proceed if you ", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(y0 + dy * i++, "fully understand the freeze-drying process.",       TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            break;

        case -1:

            draw_line(y0 + dy * i++, "Going below our recommended pressure limit can",    TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(y0 + dy * i++, "result in longer dry cycle times as well as ",      TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(y0 + dy * i++, "lower cycle temperatures. Only proceed if you",     TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(y0 + dy * i++, "fully understand the freeze-drying process.",       TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            break;
        }
    }

    if (draw_button(x0 + x1, y1,      w1, h1, TFT_BLUE, "Continue", &Rajdhani_SemiBold12pt7b))
    {
        unpause();
        return;
    }

    if (draw_button(x0 - x1 - w1, y1, w1, h1, TFT_BLUE, "Undo", &Rajdhani_SemiBold12pt7b))
    {
        if (abs(prev_page) == 30)  pressureSettingClamp(current_recipe.pressure);
        else                       pressureSettingClamp(set_pressure);
        unpause();
        return;
    }

    done = true;
} */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* DEFAULTS_2:    
* Unit Preferences:   Page 25
* 
* Used to change default pressure and temperature units.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void defaults_2(){                    
  
    if (!done) {  //draw static items to the screen
        tft.fillScreen(BACKGROUND);
        draw_title_two("UNIT", "PREFERENCES", &Nunito_Black14pt7b);
        draw_line(5, 115, "Default", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(5, 140, "Pressure Unit:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(5, 190, "Default", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        draw_line(5, 215, "Temperature Unit:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
        tft.fillRoundRect(230, 95, 225, 55, 11, TFT_GRAY);
        tft.drawRoundRect(230, 95, 225, 55, 11, TFT_BLACK);
        tft.fillRoundRect(230, 165, 225, 55, 11, TFT_GRAY);
        tft.drawRoundRect(230, 165, 225, 55, 11, TFT_BLACK);
  }  
  if(draw_button(5, 3, 110, 70, MAIN_THEME, "PREV.", "PAGE",  &Rajdhani_SemiBold12pt7b)){  //draw buttons
      setPage(24);
      return;
  }
  if (draw_button(365, 3, 110, 70, MAIN_THEME, "NEXT", "PAGE", &Rajdhani_SemiBold12pt7b)) {
      setPage(26);
      return;
  }
  if(draw_button(20, 235, 215, 80, TFT_RED, "SAVE", "& EXIT", &Nunito_Black14pt7b)){          
      store_values();
      setPage(17);
      return;
  }
  if(draw_button(245, 235, 215, 80, MAIN_THEME, "DISCARD", "CHANGES", &Nunito_Black14pt7b)){  
      reset_values();
      setPage(17);
      return;
  }
   
  if(!done){  //draw button arrows
    tft.drawBitmap(8, 13, left_arrow, 20, 50, TFT_WHITE);
    tft.drawBitmap(453, 13, right_arrow, 20, 50, TFT_WHITE);
  }   
  if(temp_refresh || !done){  //display current Unit
    tft.fillRect(235, 168, 110, 45, TFT_GRAY);
    if(!temp_unit) draw_line(250, 205, "C", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    else draw_line(250, 205, "F", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
  }  
  if(pressure_refresh || !done){  //display current pressure unit                                               
    tft.fillRect(235, 98, 110, 45, TFT_GRAY);
    if(pressure_unit == 0) draw_line(250, 135, "  Pa", TFT_WHITE, &Rajdhani_SemiBold20pt7b);    
    else if(pressure_unit == 1)draw_line(250, 135, "mTorr", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    else if(pressure_unit == 2)draw_line(250, 135, "mBar", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    else if(pressure_unit == 3)draw_line(250, 135, "  PSI", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
  }    
  pressure_unit_changer(350, 100, 100);  //unit changer button
  temp_unit_changer(350, 170, 100);  
  
  done = true;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* DEFAULTS_3:    
* Sound Settings: Page 26
* 
* Sound Settings for button press and cycle complete
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void defaults_3(){          

  static bool toggle_refresh_1, toggle_refresh_2, toggle_1, toggle_2;  //static toggle items
  
  if (!done) {  //draw static items to the screen
      tft.fillScreen(BACKGROUND);
      draw_title_two("SOUND", "SETTINGS", &Nunito_Black14pt7b);
      draw_line(5, 115, "Button Press", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
      draw_line(5, 140, "Sound:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
      draw_line(5, 190, "Cycle Complete", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
      draw_line(5, 215, "Chime:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    f_reset = false;
  }  
  if(draw_button(5, 3, 110, 70, MAIN_THEME, "PREV.", "PAGE",  &Rajdhani_SemiBold12pt7b)){  //draw buttons
      setPage(25);
      return;
  }
  if (draw_button(365, 3, 110, 70, MAIN_THEME, "NEXT", "PAGE", &Rajdhani_SemiBold12pt7b)) {
      setPage(45);
      return;
  }
  if(draw_button(20, 235, 215, 80, TFT_RED, "SAVE", "& EXIT", &Nunito_Black14pt7b)){          
      store_values();
      setPage(17);
      return;
  }
  if(draw_button(245, 235, 215, 80, MAIN_THEME, "DISCARD", "CHANGES", &Nunito_Black14pt7b)){  
      reset_values();
      setPage(17);
      return;
  }
  
  if(!done){  //draw arrows for buttons
    tft.drawBitmap(8, 13, left_arrow, 20, 50, TFT_WHITE);
    tft.drawBitmap(453, 13, right_arrow, 20, 50, TFT_WHITE);
  }
  /// TODO: These look like they are copies for both buttons, do we need the big if statements?
//   if (!button_sound) {  //Draw sound toggle buttons 
    if(draw_toggle(250, 90, 200, 60, &toggle_refresh_1, button_sound, &toggle_1)){
      button_sound = !button_sound; toggle_refresh_1 = true; delay(150); } //}
//   else{
//     if(draw_toggle(250, 90, 200, 60, &toggle_refresh_1, button_sound, &toggle_1)){
//       button_sound = !button_sound; toggle_refresh_1 = true; delay(150); } }
  
//   if(!cycle_sound){
    if(draw_toggle(250, 165, 200, 60, &toggle_refresh_2, cycle_sound, &toggle_2)){
      cycle_sound = !cycle_sound; toggle_refresh_2 = true; delay(150); } //}
//   else{
//     if(draw_toggle(250, 165, 200, 60, &toggle_refresh_2, cycle_sound, &toggle_2)){
//       cycle_sound = !cycle_sound; toggle_refresh_2 = true; delay(150); } }
      
  done = true;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* DEFAULTS_4:    
* Factory Reset:  Page 27
* 
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void defaults_4(){                    
  static bool toggle_refresh, toggle_pressed;
  
  if(!done){    
      tft.fillScreen(BACKGROUND);
      draw_title_two("FACTORY", "RESET", &Nunito_Black14pt7b);
  }
  //draw buttons
  if (draw_button(5, 3, 110, 70, MAIN_THEME, "PREV.", "PAGE", &Rajdhani_SemiBold12pt7b)) {
      setPage(45);
      return;
  }

  if(draw_button(20, 235, 215, 80, TFT_RED, "SAVE", "& EXIT", &Nunito_Black14pt7b)){          
      store_values();
      setPage(17);
      return;
  }

  if(draw_button(245, 235, 215, 80, MAIN_THEME, "DISCARD", "CHANGES", &Nunito_Black14pt7b)){  
      f_reset = false;
      reset_values();
      setPage(17);
      return;
  }
  
  if(!done){  //images on buttons
    tft.drawBitmap(8, 13, left_arrow, 20, 50, TFT_WHITE);
  }
  //draw reset info on screen
  draw_line(25, 110, "Selecting this option will restore all", PG_TXT, &Rajdhani_SemiBold9pt7b);
  draw_line(25, 130, "settings to their default 'factory' values.", PG_TXT, &Rajdhani_SemiBold9pt7b);
  draw_line(25, 150, "This cannot be reversed after saving.", PG_TXT, &Rajdhani_SemiBold9pt7b);
  draw_line(25, 170, "Press the reset button and save to", PG_TXT, &Rajdhani_SemiBold9pt7b);
  draw_line(25, 190, "perform reset.", PG_TXT, &Rajdhani_SemiBold9pt7b);
  
//   if(f_reset){  //draw toggle buttons
    if(draw_button_change(340, 100, 120, 100, f_reset ? TFT_RED : TFT_GRAY, f_reset ? "RESET ON" : "RESET OFF", 
      &Rajdhani_SemiBold12pt7b, &toggle_refresh, &toggle_pressed)){
      f_reset = !f_reset; toggle_refresh = true; }
//   }
//   else{
//     if(draw_button_change(340, 100, 120, 100, TFT_GRAY, "RESET OFF", 
//       &Rajdhani_SemiBold12pt7b, &toggle_refresh, &toggle_pressed)){
//       f_reset = true; toggle_refresh = true; }
//   }  
  
  done = true;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* DEFAULTS_5:    
* Defrost Fan Settings:   Page 45
*
* This Settings page allows the user to turn on or off the
* fan during the defrost cycle. This setting is persistant
* in flash but can also be changed during the defrost cycle
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void defaults_5(){                

  static bool toggle_refresh_3, toggle_3;  //static toggle items
  
  if(!done){  //draw static items to the screen
      tft.fillScreen(BACKGROUND);
      draw_title_two("DEFROST", "SETTINGS", &Nunito_Black14pt7b);
      draw_line(100, "Turning on the chamber", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
      draw_line(120, "fan can help speed up", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
      draw_line(140, "the defrost process", TFT_WHITE, &Rajdhani_SemiBold12pt7b);

      draw_line(55, 185, "Chamber Fan", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
      draw_line(55, 205, "Control:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    f_reset = false;
  }  
  if(draw_button(5, 3, 110, 70, MAIN_THEME, "PREV.", "PAGE",  &Rajdhani_SemiBold12pt7b)){  //draw buttons
      setPage(26);
      return;
  }
  if (draw_button(365, 3, 110, 70, MAIN_THEME, "NEXT", "PAGE", &Rajdhani_SemiBold12pt7b)) {
      setPage(49);
      return;
  }
  if(draw_button(20, 235, 215, 80, TFT_RED, "SAVE", "& EXIT", &Nunito_Black14pt7b)){          
      store_values();
      setPage(17);
      return;
  }
  if(draw_button(245, 235, 215, 80, MAIN_THEME, "DISCARD", "CHANGES", &Nunito_Black14pt7b)){  
      reset_values();
      setPage(17);
      return;
  }
  
  if(!done){  //draw arrows for buttons
    tft.drawBitmap(8, 13, left_arrow, 20, 50, TFT_WHITE);
    tft.drawBitmap(453, 13, right_arrow, 20, 50, TFT_WHITE);
  }  

/// TODO: Do we need the extra here?
//   if(!defrost_fan){  //Draw sound toggle buttons 
    if(draw_toggle(250, 160, 200, 60, &toggle_refresh_3, defrost_fan, &toggle_3)){
      defrost_fan = !defrost_fan; toggle_refresh_3 = true; delay(150); } //}
//   else{
//     if(draw_toggle(250, 160, 200, 60, &toggle_refresh_3, defrost_fan, &toggle_3)){
//       defrost_fan = !defrost_fan; toggle_refresh_3 = true; delay(150); } }
      
  done = true;
}

void defaults_6(){    //page 49             

  static bool toggle_refresh_4, toggle_4;  //static toggle items
  
  if(!done){  //draw static items to the screen
      tft.fillScreen(BACKGROUND);
      draw_title_two("COMPLETE PAGE", "SETTINGS", &Nunito_Black14pt7b);
      
      draw_line(90, "If on, the pump will stay on", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
      draw_line(110, " the entire time on this page", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
      draw_line(130, "If off, the pump may turn", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
      draw_line(150, "on and off intermittently", TFT_WHITE, &Rajdhani_SemiBold12pt7b);      

      draw_line(55, 185, "Vacuum Pump", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
      draw_line(55, 205, "Control:", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    f_reset = false;
  }  
  if(draw_button(5, 3, 110, 70, MAIN_THEME, "PREV.", "PAGE",  &Rajdhani_SemiBold12pt7b)){  //draw buttons
      setPage(45);
      return;
  }
  if (draw_button(365, 3, 110, 70, MAIN_THEME, "NEXT", "PAGE", &Rajdhani_SemiBold12pt7b)) {
      setPage(27);
      return;
  }
  if(draw_button(20, 235, 215, 80, TFT_RED, "SAVE", "& EXIT", &Nunito_Black14pt7b)){          
      store_values();
      setPage(17);
      return;
  }
  if(draw_button(245, 235, 215, 80, MAIN_THEME, "DISCARD", "CHANGES", &Nunito_Black14pt7b)){  
      reset_values();
      setPage(17);
      return;
  }
  
  if(!done){  //draw arrows for buttons
    tft.drawBitmap(8, 13, left_arrow, 20, 50, TFT_WHITE);
    tft.drawBitmap(453, 13, right_arrow, 20, 50, TFT_WHITE);
  }  

/// TODO: Do we need the extra here?
//   if(!completeVaccumOnAlways){  //Draw sound toggle buttons 
    if(draw_toggle(250, 160, 200, 60, &toggle_refresh_4, completeVaccumOnAlways, &toggle_4)){
      completeVaccumOnAlways = !completeVaccumOnAlways; toggle_refresh_4 = true; delay(150); } //}
//   else{
//     if(draw_toggle(250, 160, 200, 60, &toggle_refresh_4, completeVaccumOnAlways, &toggle_4)){
//       completeVaccumOnAlways = !completeVaccumOnAlways; toggle_refresh_4 = true; delay(150); } }
      
  done = true;
}

// Page 12
void manual_tests()
{
    static bool vacuum, fridge;
    static bool vacuum_override;
    static Time pause_time;
    static Time vacuum_time;

    // Invisible Redraw Button
    if (invisible_button(5, 265, 80, 50))
    {
        tft.fillScreen(BACKGROUND);
        done = false;
        reset_screen();
    }
    
    if (!done)
    {
        set_pixel_blue();
        tft.fillScreen(BACKGROUND);
        draw_title("MANUAL TESTS", &Nunito_Black14pt7b);
        draw_line(100, 244, "Chamb. Temp.:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(100, 266, "Shelf Temp.:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(100, 288, "Pressure:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(100, 310, "Elapsed Time:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);

        //Defaults button label
        draw_line(55, 190, "Vacuum", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(65, 210, "Pump", TFT_WHITE, &Rajdhani_SemiBold9pt7b);

        //Override button label
        draw_line(185, 190, "Refrigeration", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(210, 210, "& Fan", TFT_WHITE, &Rajdhani_SemiBold9pt7b);

        //diagnostics button
        draw_line(350, 190, "Heating", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(350, 210, "Shelves", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        ForceHeaterON = fridge = vacuum = false;  //set our default states as off    
        CycleTime = 0;
        pause_time = 0;
        vacuum_time = 0;
        vacuum_override = 0;
        draw_line(5, 310, SOFTWARE_VERSION, TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        
    }

    if (ForceHeaterON || fridge || vacuum) pause_time = CycleTime;  //start rtc when any component is selected    
    else                                   CycleTime = pause_time;   //pause rtc when all components are deselected

    if (vacuum && (vacuum_time == 0))
    {
        vacuum_time = CycleTime;
    } 
    else if (!vacuum)
    {
        vacuum_time = 0;
    }

    unsigned int vacuum_on_time = CycleTime.totalSeconds() - vacuum_time.totalSeconds();
    if((vacuum_on_time > 120) && (chamber_pressure > 700000) && vacuum_time != 0)
    {
        setPage(21);
        test_num = 99;
        return;
    }

    if ((vacuum_on_time > 7 * 60) && (chamber_pressure > 200000) && vacuum_time != 0)
    {
        vacuum_override = true;
        vacuum_time = 0;
    }

    if (draw_button(5, 3, 110, 50, TFT_RED, "BACK", &Nunito_Black10pt7b))
    {
        setPage(17);
        return;
    }


    ChmbrTemp.Draw(270, 244);
    ShelfTemp.Draw(270, 266);
    draw_pressure(270, 288);

    temp_unit_changer(413, 215, 60);  //temp unit button  
    pressure_unit_changer(413, 270, 60);  //pressure unit button  
    diagnostic_buttons(&vacuum, &fridge, &ForceHeaterON, &vacuum_override);  //draw diagnostic buttons to screen

    hardware_control(fridge, fridge, vacuum);  //turn on hardware specified //vacuum is disabled for now


    if (ForceHeaterON || fridge || vacuum)
        //start rtc when any component is selected    
        pause_time = CycleTime;
    else
        //pause rtc when all components are deselected
        CycleTime = pause_time;

    elapsed_time_small(270, 310);
    //hardware_serial_monitor(1, 15000); //draw elapsed time

    done = true;

}

void save_changes(){  // Page 13
  
    if (!done) {  //static items
        tft.fillScreen(BACKGROUND);
        draw_line(80, "SAVING", TFT_WHITE, &Nunito_Black30pt7b);
        draw_line(160, "CHANGES...", TFT_WHITE, &Nunito_Black30pt7b);
        draw_line(305, "DO NOT TURN OFF DEVICE", TFT_RED, &Nunito_Black10pt7b);
        /* Items to save
         * Default Freeze Time, Default Dry time, Default dry temp.
         * default temperature unit, default pressure unit
         */     
        store_values();  //store values in config file    
        tft.fillScreen(BACKGROUND);  //fill screen with background
        draw_line(80, "SAVE", TFT_WHITE, &Nunito_Black30pt7b);
        draw_line(160,"COMPLETE", TFT_WHITE, &Nunito_Black30pt7b);
  } 
  if(draw_button(140, 220, 200, 90, TFT_RED, "HOME", &Nunito_Black20pt7b)){  //draw home button
      setPage(1);
      return;
  }
  
  done = true;
}


void diagnostics(){  // Page 19
    
  if(!done){  //static items 
    set_pixel_blue();
    tft.fillScreen(BACKGROUND);  //backgroung color    
    draw_title("DIAGNOSTICS", &Nunito_Black14pt7b);  //title
  }
 
  if(draw_button(5, 3, 110, 50, TFT_RED, "BACK", &Nunito_Black10pt7b)){  //exit button
      setPage(17);
      return;
  }
  if(draw_button(5, 55, 470, 85, COMP_THEME, "", &Nunito_Black20pt7b)){  //Vacuum test button
      test_num = 1;
      setPage(20);
      return;
  }
//   if(draw_button(5, 145, 470, 85, COMP_THEME, "", &Nunito_Black20pt7b)){  //Refrigeration test button
//       test_num = 2;
//       setPage(20);
//       return;
//   }
//   if(draw_button(5, 235, 470, 82, COMP_THEME, "", &Nunito_Black20pt7b)){  //Heating test button
//       test_num = 3;
//       setPage(20);
//       return;
//   }

  if(!done){  //button images
    tft.drawBitmap(5, 60, pump, 80, 80, TFT_WHITE);
    // tft.drawBitmap(5, 150, fridge_icon, 80, 80, TFT_WHITE);
    // tft.drawBitmap(5, 237, heater, 80, 80, TFT_WHITE);   
    draw_line(90, 80, "Test the functionality of the Vacuum Pump.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
    draw_line(90, 100, "Test Length: 45 minutes", TFT_WHITE, &Rajdhani_SemiBold9pt7b);    
    // draw_line(90, 170, "Test the capability of the Refrigeration System.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
    // draw_line(90, 190, "Test Length: 1 hour", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
    // draw_line(90, 260, "Test the functionality of the Heating Shelves.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
    // draw_line(90, 280, "Test Length: 5 minutes", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
  }  
  
  done = true;  
  
}




void test_pages() {  // Page 20
    
    if (!done) {
        CycleTime = 0; // always reset this, even if coming from an error page (aka abs(prev_page) == -1)
        Tests.off_gas = false;
        set_pixel_blue();
        tft.fillScreen(BACKGROUND);
        draw_line(240, 305, ":", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
        draw_line(315, 305, ":", TFT_WHITE, &Rajdhani_SemiBold20pt7b);


        switch (test_num) {  //information on the different tests


        //vacuum test info
        case 1:

            draw_title("VACUUM TEST", &Nunito_Black14pt7b);
            draw_line(50, "This test will attempt to bring the chamber", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);

            switch (pressure_unit) {
            case 0:
                draw_line(75, "pressure to below 67 Pa.", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
                break;

            case 1:
                draw_line(75, "pressure to below 500 mTorr.", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
                break;

            case 2:
                draw_line(75, "pressure to below 1.5 mBar.", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
                break;

            case 3:
                draw_line(75, "pressure to below 0.009 PSI.", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
                break;
            }

            draw_line(5, 80 + 30, "-Ensure that the vacuum pump is plugged in.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(5, 100 + 30, "-Check the pump/hose fittings for leaks.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(5, 120 + 30, "-Ensure that the drain valve is closed.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(5, 140 + 30, "-Make sure that the chamber door is closed and", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(5, 160 + 30, " creates a tight seal with the rubber gasket.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(145, 250, "Chamber Pressure:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(145, 270, "Time Remaining:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            test_time = Time(45, 0);
            break;


        //refrigeration test info
        case 2:

            // draw_title("COOLING TEST", &Nunito_Black14pt7b);
            // draw_line(50, "This test will attempt to bring the chamber", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
            // if (temp_unit == true) {
            //     draw_line(75, "temp. to below -25*F.", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
            // }
            // else {
            //     draw_line(75, "temp. to below -31*C.", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
            // }

            // draw_line(5, 80 + 30, "-Ensure that the chamber door is closed.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            // draw_line(5, 100 + 30, "-Ensure the trays are emptied prior to running the test.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            // draw_line(5, 120 + 30, "-Ensure that the drain valve is closed.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            // draw_line(5, 140 + 30, "-Ensure that the chamber fan is connected.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            // draw_line(5, 160 + 30, "-If tests continue to fail, contact Blue Alpine Support.", TFT_CYAN, &Rajdhani_SemiBold9pt7b);
            // draw_line(145, 250, "Chamber Temperature:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            // draw_line(145, 270, "Time Remaining:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            // test_time = Time(1, 0, 0);
            break;

        //heating shelves test info
        case 3:

            // draw_title("HEATING TEST", &Nunito_Black14pt7b);
            // draw_line(50, "This test will attempt to raise the temp. ", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);

            // if (temp_unit == true) {
            //     draw_line(75, "of heating shelves to 120*F.", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
            // }
            // else {
            //     draw_line(75, "of heating shelves to 48*C.", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
            // }

            // draw_line(5, 80 + 30, "-Ensure the trays are emptied prior to running the test.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            // draw_line(5, 100 + 30, "-Ensure that the heating shelves are connected.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            // draw_line(5, 120 + 30, "-If tests continue to fail, contact Blue Alpine Support.", TFT_CYAN, &Rajdhani_SemiBold9pt7b);
            // draw_line(145, 250, "Shelf Temperature:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            // draw_line(145, 270, "Time Remaining:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            // test_time = Time(5, 0);
            break;
        }
    }

    if (draw_button(5, 235, 135, 80, TFT_RED, "CANCEL", &Nunito_Black14pt7b))
    {
        setPage(19);
        Tests.off_gas = true;
        return;
    }

    switch (test_num)
    {
    // pressure
    case 1:
        /// TODO: We might want to be more strict than the standard checks (for example, with chamber sensor), but doing so may require diffrent recovery conditions on the error page
        // if (standardSensorCheck_pressure()) {
        //     setPage(23);
        //     return;
        // }
        if (remaining < 30_min)
            hardware_control(0, 1, 1);
        else
            hardware_control(1, 1, 0);
        break;

    // refrigeration
    case 2:

        // // NOTE: error page only thrown if this sensor is disconnected, but all sensors must be connected to exit
        // if (ChmbrTemp.Fault) {
        //     setPage(23);
        //     return;
        // }
        // hardware_control(1, 1, 0);
        break;

    // shelf heaters
    case 3:

        // if (ShelfTemp.Fault) {
        //     setPage(23);
        //     return;
        // }
        // hardware_control(0, 0, 0);
        // ForceHeaterON = true;
        break;
    }


    test_remaining();  //draw remaining time  
    draw_test_live_data();

    if (remaining <= 0) {
        Tests.off_gas = true;
        ForceHeaterON = false;
        setPage(21);
        return;
    }

    done = true;

}


/// Page 38
void qc_test_pages()
{
    static int off_gas_min_mTorr = 1200;
    if (!done)
    {
        set_pixel_blue();
        tft.fillScreen(BACKGROUND);
        CycleTime = 0;
        draw_line(240, 305, ":", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
        draw_line(315, 305, ":", TFT_WHITE, &Rajdhani_SemiBold20pt7b);


        /// TODO: reset qc_test_num in p35
        switch (Tests.qc_test_num)
        {
        case 0:
        case 1:
        case 3:
            test_num = 1;
            break;

        case 2:
            test_num = 2;
            break;

        case 4:
            test_num = 3;
            break;

        default: // just in case
            test_num = 0;
            return;
        }


        /// static items ///

        if (Tests.leak_only == true)
        {
            draw_title("VACUUM TEST", &Nunito_Black14pt7b);
            draw_line(50, "This test will attempt to bring the chamber", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
            draw_line(75, "pressure to below 1125 mTorr.", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);

            draw_line(5,  80 + 30, "-Ensure that the vacuum pump is plugged in.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(5, 100 + 30, "-Check the pump/hose fittings for leaks.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(5, 120 + 30, "-Ensure that the drain valve is closed.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(5, 140 + 30, "-Make sure that the chamber door is closed and", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(5, 160 + 30, " creates a tight seal with the rubber gasket.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(5, 180 + 30, "-If tests continue to fail, contact Blue Alpine Support.", TFT_CYAN, &Rajdhani_SemiBold9pt7b);
            draw_line(145, 250, "Chamber Pressure:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(145, 270, "Time Remaining:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);

            test_time = Time(10, 0);
        }
        else
        {
            //information on the different tests
            switch (test_num)
            {

            //vacuum test info
            case 1:
                if(Tests.off_gas)
                {
                  draw_title("OFF GAS HEAT RACK", &Nunito_Black14pt7b);
                  draw_line(50, "If this is not the first time using this heat rack,", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
                  draw_line(75, "hit skip. Otherwise, the heat and pump will be on", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
                  static String phrase = "until pressure is below " + String(off_gas_min_mTorr) + " mTorr";
                  static const char* placeHolder = phrase.c_str();
                  draw_line(100, placeHolder, TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
                  static int inc_down_y = 45;
                  draw_line(5, 80 + inc_down_y, "-Ensure that the vacuum pump is plugged in.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                  //draw_line(5, 100 + inc_down_y, "-Check the pump/hose fittings for leaks.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                  draw_line(5, 100 + inc_down_y, "-Ensure that the drain valve is closed.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                  draw_line(5, 120 + inc_down_y, "-Make sure that the chamber door is closed and", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                  draw_line(5, 140 + inc_down_y, " creates a tight seal with the rubber gasket.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                  draw_line(5, 160 + inc_down_y, "-If tests continue to fail, contact Blue Alpine Support.", TFT_CYAN, &Rajdhani_SemiBold9pt7b);
                  draw_line(145, 230, "Shelf Temperature:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                  draw_line(145, 250, "Chamber Pressure:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                  draw_line(145, 270, "Time Remaining:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                }
                else
                {
                  draw_title("VACUUM TEST", &Nunito_Black14pt7b);
                
                  draw_line(50, "This test will attempt to bring the chamber", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);

                  if (Tests.qc_test_num == 3)
                  {
                      draw_line(75, "pressure to below 500 mTorr.", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
                  }
                  else
                  {
                      draw_line(75, "pressure to below 1125 mTorr.", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
                  }

                  draw_line(5, 80 + 30, "-Ensure that the vacuum pump is plugged in.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                  draw_line(5, 100 + 30, "-Check the pump/hose fittings for leaks.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                  draw_line(5, 120 + 30, "-Ensure that the drain valve is closed.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                  draw_line(5, 140 + 30, "-Make sure that the chamber door is closed and", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                  draw_line(5, 160 + 30, " creates a tight seal with the rubber gasket.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                  draw_line(5, 180 + 30, "-If tests continue to fail, contact Blue Alpine Support.", TFT_CYAN, &Rajdhani_SemiBold9pt7b);
                  draw_line(145, 250, "Chamber Pressure:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                  draw_line(145, 270, "Time Remaining:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                }

                if (Tests.off_gas)
                {
                    test_time = Time(45, 0);
                }
                else if(Tests.qc_test_num == 3)
                {
                    test_time = Time(20, 0);//
                }
                else
                {
                    test_time = Time(10, 0);
                }
                break;



            //refrigeration test info
            case 2:
                draw_title("COOLING TEST", &Nunito_Black14pt7b);
                draw_line(50, "This test will attempt to bring the chamber", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);

                if (temp_unit == true)
                {
                    draw_line(75, "temp. to below -25*F.", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
                }
                else
                {
                    draw_line(75, "temp. to below -31*C.", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
                }

                draw_line(5, 80 + 30, "-Ensure that the chamber door is closed.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                draw_line(5, 100 + 30, "-Ensure the trays are emptied prior to running the test.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                draw_line(5, 120 + 30, "-Ensure that the drain valve is closed.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                draw_line(5, 140 + 30, "-Ensure that the chamber fan is connected.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                draw_line(5, 160 + 30, "-If tests continue to fail, contact Blue Alpine Support.", TFT_CYAN, &Rajdhani_SemiBold9pt7b);
                draw_line(145, 250, "Chamber Temperature:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                draw_line(145, 270, "Time Remaining:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                
                test_time = Time(40, 0);
                break;

            //heating shelves test info
            case 3:
                draw_title("HEATING TEST", &Nunito_Black14pt7b);
                draw_line(50, "This test will attempt to raise the temp. ", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);

                if (temp_unit == true)
                {
                    draw_line(75, "of heating shelves to 120*F.", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
                }
                else
                {
                    draw_line(75, "of heating shelves to 48*C.", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
                }

                draw_line(5,  80 + 30, "-Ensure the trays are emptied prior to running the test.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                draw_line(5, 100 + 30, "-Ensure that the heating shelves are connected.", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                draw_line(5, 120 + 30, "-If tests continue to fail, contact Blue Alpine Support.", TFT_CYAN, &Rajdhani_SemiBold9pt7b);
                draw_line(145, 250, "Shelf Temperature:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
                draw_line(145, 270, "Time Remaining:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);

                // test is 10 minutes
                if(!Tests.heat_const)
                {
                  test_time = Time(10, 0);
                }
                else
                {
                  test_time = Time(2, 0);
                }
                break;
            }
        }
    }

    
    /// turn on appropriate feeze-drying peripherals //

    ForceHeaterON = false;
    switch (test_num)
    {
    case 1:
        if (Tests.qc_test_num == 3)
            hardware_control(0, 1, 1);  //turn on vacuum if it's on the 3rd vacuum test turn on refrigeration.
            if(chamber_pressure == 200)
            {
              remaining = 0;
              Tests.cold_pressure_1 = chamber_pressure;
            }

        else if (Tests.qc_test_num == 0)
        {
            hardware_control(0, 0, 1);
            if(Tests.off_gas == true)
            {
              ForceHeaterON = true;
              Tests.heating_test_f = ShelfTemp.Fahrenheit;
            }
            else{
              ForceHeaterON = false;
            }
        }
        else if(Tests.qc_test_num == 1){
          hardware_control(0, 0, 1);
        }
            

        break;

    case 2:
        hardware_control(1, 1, 0);
        break;

    case 3:
        hardware_control(0, 0, 0);
        ForceHeaterON = true;
        break;

    }

    test_remaining();  //draw remaining time  
    draw_test_live_data();

    /* WE DO NOT WANT THIS
    // Auto skip test 1
    if (Tests.qc_test_num == 1 && chamber_pressure < 1000 && chamber_pressure > 150)
    {
        Tests.ambient_pressure_1 = chamber_pressure;
        Tests.qc_test_num++;
        setPage(40);
        return;
    } */

    if(Tests.off_gas){
      if(chamber_pressure < off_gas_min_mTorr && chamber_pressure > 150)
      {
        Tests.off_gas = false;
        done = false;
        return;
      }
    }

    if(test_num == 3)
    {
        if(!Tests.heat_const)
        {
            if(ShelfTemp.Fahrenheit >= 120)
            {
                Tests.heat_const = true;
                done = false;
                return;
            }
        }
    }

    if (draw_button(5, 235, 135, 80, TFT_RED, "CANCEL", &Nunito_Black14pt7b))
    {
        //setPage(17);
        Tests.qc_test_num = -1;
        Tests.off_gas = false;
        return;
    }

    if (draw_button(390, 265, 80, 50, TFT_RED, "SKIP", &Nunito_Black14pt7b) || remaining <= 0 || Tests.qc_test_num == -1)
    {
        if (Tests.leak_only == true)
        {
            setPage(40);
            return;
        }

        switch (Tests.qc_test_num)
        {
        case 0:
            if(Tests.off_gas)
            {
              Tests.off_gas = false;
              done = false;
              return;
            }
            else
            {
              Tests.ambient_pressure_1 = chamber_pressure;
              Tests.qc_test_num++;
              Tests.off_gas = false;
              setPage(39);
              return;
            }

        case 1:
            Tests.ambient_pressure_2 = chamber_pressure;
            Tests.qc_test_num++;
            setPage(40);
            return;

        case 2:
            Tests.chamber_test_f = ChmbrTemp.Fahrenheit;
            Tests.qc_test_num++;
            setPage(38);
            return;

        case 3:
            Tests.cold_pressure_1 = chamber_pressure;
            Tests.qc_test_num++;
            if(Tests.heating_test_f >= 119 && Tests.heating_test_f <= 121)
            {
              Tests.qc_test_num++;
            }
            else
            {
              setPage(38);
            }
            return;

        case 4:
            Tests.heating_test_f = ShelfTemp.Fahrenheit;
            Tests.qc_test_num++;

        default:
            Tests.qc_test_num = 0;
            Tests.off_gas = true;
            setPage(36);
            return;
        }
    }

    done = true;
}

// Page 21
void results()
{

    if (!done) {  //draw test box over current screen
        sound = false;

        pixels.clear();
        pixels.show();

        tft.fillRoundRect(50, 50, 380, 220, 10, MAIN_THEME);
        tft.drawRoundRect(50, 50, 380, 220, 10, TFT_WHITE);
    }
    
    // timing for blinking LEDS
    unsigned now = millis() % 2500;

    set_pixel_red();

    if (now > 1000 && TimeSinceLastValidPresure > PRES_INV_TIMEOUT_STRICT_ms)
        set_pixel_white();

    if (now > 1500 && ChmbrTemp.Fault)
        set_pixel_yellow();

    if (now > 2000 && ShelfTemp.Fault)
        set_pixel_purple();

    pixels.show();


    //touch anywhere on the screen to change pages
    if (invisible_touch_test() /*|| !sensorFault*/)
    {
        pixels.clear();
        pixels.show();
        press_button();
        if(prev_page == 20)
        {
          setPage(19);
        }
        else
        {
          setPage(prev_page);
        }
        return;
    }

    if (!done)  //check results and print
    {
        switch (test_num) {
            //pressure results
        case 1:
            
            // /// Override for if we got here from the diagnostics page
            // //Check if it can get to at least 150 pascals
            // if (pressure_ < 67)
            // {
            //     set_pixel_green();
            //     draw_line(150, "VACUUM TEST PASSED", TFT_GREEN, &Nunito_Black14pt7b);
            //     draw_line(395, 80, "X", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            // }
            // else
            // {
            //     set_pixel_red();
            //     draw_line(150, "VACUUM TEST FAILED", TFT_RED, &Nunito_Black14pt7b);
            //     draw_line(395, 80, "X", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            // }
            draw_line(150, "VACUUM TEST", TFT_WHITE, &Nunito_Black14pt7b);
            draw_line(395, 80, "X", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            draw_line(180, "Final Chamber Pressure:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);

            // Check which unit of pressure you will be using.
            switch (pressure_unit)
            {
            case 0:
                draw_line(165, 200, pressure_, 1, " Pa", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
                break;

            case 1:
                draw_line(165, 200, pressure_ * 7.5006167382113, 2, " mTorr", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
                break;

            case 2:
                draw_line(165, 200, pressure_ * 0.01, 1, " mBar", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
                break;

            case 3:
                draw_line(165, 200, pressure_ * 0.00014503773772954, 3, " PSI", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
                break;
            }

            break;


            //cooling results
        case 2:
            if (ChmbrTemp.Fahrenheit < -25)
            {
                set_pixel_green();
                draw_line(150, "COOLING TEST PASSED", TFT_GREEN, &Nunito_Black14pt7b);
                draw_line(395, 80, "X", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            }
            else
            {
                set_pixel_red();
                draw_line(150, "COOLING TEST FAILED", TFT_RED, &Nunito_Black14pt7b);
                draw_line(395, 80, "X", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            }
            draw_line(180, "Final Chamber Temperature:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);

            // Check for temp unit to display
            if (temp_unit == true)
            {
                draw_line(200, 200, ChmbrTemp.Fahrenheit, 1, " *F", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
            }
            else
            {
                draw_line(200, 200, ChmbrTemp.Celsius, 1, " *C", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
            }
            break;


            //heating results
        case 3:
            if (ShelfTemp.Fahrenheit > 110 && ShelfTemp.Fahrenheit < 140)
            {
                set_pixel_green();
                draw_line(150, "HEATING TEST PASSED", TFT_GREEN, &Nunito_Black14pt7b);
                draw_line(395, 80, "X", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            }
            else
            {
                set_pixel_red();
                draw_line(150, "HEATING TEST FAILED", TFT_RED, &Nunito_Black14pt7b);
                draw_line(395, 80, "X", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            }
            draw_line(180, "Final Shelf Temperature:", TFT_WHITE, &Rajdhani_SemiBold9pt7b);

            // Check for temp unit to display
            if (temp_unit == true)
            {
                draw_line(200, 200, ShelfTemp.Fahrenheit, 1, " *F", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
            }
            else
            {
                draw_line(200, 200, ShelfTemp.Celsius, 1, " *C", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
            }
            break;
        case 99:
          set_pixel_red();
          draw_line(90, "VACUUM PUMP ERROR", TFT_RED, &Nunito_Black14pt7b);
          draw_line(55, 120, "Ensure the pressure valve is shut", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
          draw_line(55, 150, "Ensure vacuum pump switch is active", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
          draw_line(55, 180, "Ensure vacuum pump is plugged in", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
          draw_line(55, 210, "Ensure hose is properly connected", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
          draw_line(260, "Click anywhere to continue", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
          //draw_line(395, 80, "X", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        }
    }

    done = true;

}
