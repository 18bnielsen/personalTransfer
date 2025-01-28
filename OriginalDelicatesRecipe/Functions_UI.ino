/*
This file contains all of the functions that draw items to the screen to be used by the different pages of the User Interface.
*/



void draw_pressure(int x, int y, bool ignoreError = false)
{
    static uint32_t lastPrintedPressure;
    static int      lastPrintedUnit;
    static bool     lastConnectionStatus;

    // don't draw if nothing's changed
    if (lastPrintedPressure == chamber_pressure
        && lastPrintedUnit == pressure_unit
        && lastConnectionStatus == (TimeSinceLastValidPresure > PRES_INV_TIMEOUT_ms)
        && done)
    {
        return;
    }

    lastPrintedPressure = chamber_pressure;
    lastPrintedUnit = pressure_unit;
    lastConnectionStatus = (TimeSinceLastValidPresure > PRES_INV_TIMEOUT_ms);


    tft.fillRect(x - 3, y - 15, 130, 20, BACKGROUND);

    if (TimeSinceLastValidPresure > PRES_INV_TIMEOUT_STRICT_ms && !ignoreError)
    {
        draw_line(x, y, "Disconnected", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
    }
    else
    {
        switch (pressure_unit)
        {
        case 0:
            draw_line(x, y, chamber_pressure * 0.13332237, 1, " Pa", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            break;
        case 1:
            draw_line(x, y, chamber_pressure, 0, " mTorr", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            break;
        case 2:
            draw_line(x, y, chamber_pressure * 0.0013332237, 3, " mBar", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            break;
        case 3:
            draw_line(x, y, chamber_pressure * 0.0000193368, 3, " psi", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
            break;
        }
    }
}



void button_label(int x, int y, String label, const GFXfont * font = &Nunito_Black20pt7b, bool shadow = true, int color = TFT_BLACK) {
  int16_t x1, y1;
  static uint16_t size[2] = {0, 0};
  //calculate center of the button
  tft.setFont(font);
  tft.getTextBounds(label, 0, 0, &x1, &y1, &size[0], &size[1]);  //calc width of new string
  
  if (shadow){
  //print label shadow
    tft.setCursor(x - size[0] / 2 - 3, y + size[1] / 2 + 2 - 3);
    tft.setTextColor(color);
    tft.print(label);
  }
 
  tft.setCursor(x - size[0] / 2, y + size[1] / 2 - 3);
  tft.setTextColor(TFT_WHITE);
  //print label
  tft.print(label);
}



bool draw_button(int x, int y, int w, int h, int color, String label, const GFXfont *font) {
  //Serial.println("Checking for touch");
  //check if already drawn // only draw once
  if (!done) {
  //draw button
    drawRoundedRectYGradient(x, y, w, h, 10, color, BUTTON);
  //draw label
    button_label(x + w / 2, y + h / 2, label, font);

  }
  //check for touches
  if (ts.touched()) {
    //Serial.println("I've been touched!? uwu");
    TS_Point p = ts.getPoint();
    ts_x = p.y;
    ts_y = tft.height() - p.x;
   


    if (point_check(x, x + w, y, y + h)) {
  //draw touch feedback
      tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, TFT_WHITE);
      tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, TFT_WHITE);
      press_button();
      return true;
    }
    tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, BACKGROUND);
    tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, BACKGROUND);
    return false;
  }
  //check the last touch point
  else {
    tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, BACKGROUND);
    tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, BACKGROUND);
  } 
  
  return false;
}



bool draw_button(int x, int y, int w, int h, int color, String label_1, String label_2, const GFXfont * font) {

  //check if already drawn // only draw once
  if (!done) {
  //draw button
    drawRoundedRectYGradient(x, y, w, h, 10, color, BUTTON);
  //draw label
    button_label(x + w / 2, y + h / 3, label_1, font);
    button_label(x + w / 2, y + 2 * h / 3, label_2, font);

  }
  //check for touches
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    ts_x = p.y;
    ts_y = tft.height() - p.x;


    if (point_check(x, x + w, y, y + h)) {
  //draw touch feedback
      press_button();
      tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, TFT_WHITE);
      tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, TFT_WHITE);
      
      return true;
    }
    tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, BACKGROUND);
    tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, BACKGROUND);
    return false;
  }
  
  else{
    tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, BACKGROUND);
    tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, BACKGROUND);
  }
  
  return false;
}

/// TODO: Delete if unused
//   bool draw_invisible_button(int x, int y, int w, int h, int color, String label_1, String label_2, const GFXfont * font) {

//   //check if already drawn // only draw once
//   if (!done) {
//   //draw button
//     drawRoundedRectYGradient(x, y, w, h, 10, color, BUTTON);
//   //draw label
//     button_label(x + w / 2, y + h / 3, label_1, font);
//     button_label(x + w / 2, y + 2 * h / 3, label_2, font);

//   }
//   //check for touches
//   if (ts.touched()) {
//     TS_Point p = ts.getPoint();
//     ts_x = p.y;
//     ts_y = tft.height() - p.x;


//     if (point_check(x, x + w, y, y + h)) {
//   //draw touch feedback
//       press_button();
//       tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, TFT_WHITE);
//       tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, TFT_WHITE);
      
//       return true;
//     }
//     tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, BACKGROUND);
//     tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, BACKGROUND);
//     return false;
//   }
  
//   else{
//     tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, BACKGROUND);
//     tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, BACKGROUND);
//   }
  
//   return false;
//   }





bool draw_button_change(int x, int y, int w, int h, int color, String label, const GFXfont * font, bool * refresh, bool *pressed) {

  //this button needs to be refreshed after loading
  if (*refresh || !done) {
  //draw button
    drawRoundedRectYGradient(x, y, w, h, 10, color, BUTTON);
  //draw label
    button_label(x + w / 2, y + h / 2, label, font);
    if (label == " C/   F") {
  // Shadows
      draw_line(x+1+w/2-30, y+21, "o", TFT_BLACK, &Rajdhani_SemiBold9pt7b);
      draw_line(x+30+w/2-30, y+21, "o", TFT_BLACK, &Rajdhani_SemiBold9pt7b);
  // Degree Symbols
      draw_line(x+4+w/2-30, y+18, "o", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
      draw_line(x+33+w/2-30, y+18, "o", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
    }
    *refresh = false;
  }
  //check for touches
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    ts_x = p.y;
    ts_y = tft.height() - p.x;


    if (point_check(x, x + w, y, y + h)) {
  //draw touch feedback
      tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, TFT_WHITE);
      tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, TFT_WHITE);      
      
      if(!*pressed){
        press_button();
        *refresh = true;
        *pressed = true;
        return true;
      }
      else{
        *refresh = false;
        return false;
      }
    }
    tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, BACKGROUND);
    tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, BACKGROUND);
    *pressed = false;
    *refresh = false;
    return false;
  }  
  
  //check the last touch point
  else{
    tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, BACKGROUND);
    tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, BACKGROUND);
    *pressed = false;
    *refresh = false;
  }
  *pressed = false;
  return false;

}



bool draw_button_change_img(int x, int y, int w, int h, int color, const uint8_t * img, bool * refresh , bool *pressed) {

  //this button needs to be refreshed after loading
  if (*refresh || !done) {
  //draw button
    drawRoundedRectYGradient(x, y, w, h, 10, color, BUTTON);
  //draw label
    tft.drawBitmap(x + 13, y + 13, img, w - 25, h - 25, TFT_WHITE);
    *refresh = false;
  }
  
  //check for touches
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    ts_x = p.y;
    ts_y = tft.height() - p.x;


    if (point_check(x, x + w, y, y + h)) {
  //draw touch feedback
      tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, TFT_WHITE);
      tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, TFT_WHITE);
            
      if(!*pressed){
        press_button();
        *refresh = true;
        *pressed = true;
        return true;
      }
      else{
        *refresh = false;
        return false;
      }
    }
    
    tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, BACKGROUND);
    tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, BACKGROUND);
    *pressed = false;
    *refresh = false;
    return false;
  }  
  
  //check the last touch point
  else{
    tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, BACKGROUND);
    tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, BACKGROUND);
    *pressed = false;
    *refresh = false;
  }

  *pressed = false;
  return false;

}



bool draw_arrow_button(int x, int y, bool left) {
  //defaults for arrow buttons
  int w = 60;
  int h = 60;
  
  if (!done) {
  //draw button

    if (left){
  //draw left button
      drawRoundedRectYGradient(x, y, 60, 60, 10, TFT_WHITE, COMP_THEME);
      tft.fillTriangle(x + 13, y + 15, x + 28, y + 48, x - 13 + 60, y + 15, TFT_WHITE);

  //tft.drawPixel(x + 28, y + 48, TFT_RED);
  //tft.drawPixel(x + 13, y + 15, TFT_RED);
  //tft.drawPixel(x - 13 + 60, y + 15, TFT_RED);

    }
    else{
  //draw right button
      drawRoundedRectYGradient(x, y, 60, 60, 10, TFT_WHITE, COMP_THEME);
      tft.fillTriangle(x + 13, y + 45, x + 30, y + 10, x - 13 + 60, y + 45, TFT_WHITE);

  //tft.drawPixel(x + 28, y + 10, TFT_RED);
  //tft.drawPixel(x + 13, y + 43, TFT_RED);
  //tft.drawPixel(x - 13 + 60, y + 43, TFT_RED);
    }

  }
  //check for touches
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    ts_x = p.y;
    ts_y = tft.height() - p.x;


    if (point_check(x, x + w, y, y + h)) {
  //draw touch feedback
      press_button();
      tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, TFT_WHITE);
      tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, TFT_WHITE);
      return true;
    }
    tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, BACKGROUND);
    tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, BACKGROUND);
    return false;
  }
  
  //check the last touch point
  else{
    tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, BACKGROUND);
    tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, BACKGROUND);
  }
  
  return false;
  
}



bool draw_toggle(int x, int y, int w, int h, bool * refresh, bool on , bool * pressed){

  //this button needs to be refreshed after loading
  if (*refresh || !done) {
  //draw button
    if(on){
  //button backdrop and border
      tft.fillRoundRect(x, y, w, h, 10, 0x5E6C);
      tft.fillRoundRect(x , y + 24, w, 36 ,10, 0x76CF);
      tft.drawRoundRect(x, y, w, h, 10, 0x4448);
      tft.drawRoundRect(x + 1, y + 1, w - 2, h - 2, 8, 0x4448);
      tft.fillRoundRect(x + w/2, y, w/2, h, 10, 0x4448);
      tft.fillRoundRect(x + 2 + w/2, y + 2, w/2 - 4, h - 4, 10, TFT_WHITE); 
  //grey slider         
      drawRoundedRectYGradient(x + 2 + w/2, y + 6, w/2 - 4, h - 8, 10, 0xF77D, 0xB5B6);
  //buttone lines
      tft.drawRoundRect(x + 2 + w/2 + 33, y + 20, 5 , 20, 3, 0xAD55);
      tft.drawRoundRect(x + 2 + w/2 + 33 + 12, y + 20, 5 , 20, 3, 0xAD55);
      tft.drawRoundRect(x + 2 + w/2 + 33 + 24, y + 20, 5 , 20, 3, 0xAD55);
  //draw label
      button_label(x + w / 4, y + h / 2, "ON", &Nunito_Black14pt7b, true, 0x4448);
    }
    else{
  //button backdrop and border
      tft.fillRoundRect(x, y, w, h, 10,  0xFA85);
      tft.fillRoundRect(x, y + 24, w, 36, 10, 0xFB46);
      tft.drawRoundRect(x, y, w, h, 10, 0x69C5);
      tft.drawRoundRect(x + 1, y + 1, w - 2, h - 2, 8, 0x69C5);
      tft.fillRoundRect(x, y, w/2, h, 10, 0x69C5); 
      tft.fillRoundRect(x + 2, y + 2, w/2 - 4, h - 4, 10, TFT_WHITE);
  //grey slider          
      drawRoundedRectYGradient(x + 2, y + 2 + 4, w/2 - 4, h - 4 - 4, 10, 0xF77D, 0xB5B6);
  //button lines
      tft.drawRoundRect(x + 2 + 33, y + 20, 5 , 20, 3, 0xAD55);
      tft.drawRoundRect(x + 2 + 33 + 12, y + 20, 5 , 20, 3, 0xAD55);
      tft.drawRoundRect(x + 2 + 33 + 24, y + 20, 5 , 20, 3, 0xAD55);
  //draw label
      button_label(x + w - w/4, y + h / 2, "OFF", &Nunito_Black14pt7b, true, 0x69C5);
    }
    *refresh = false;
  }
  //check for touches
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    ts_x = p.y;
    ts_y = tft.height() - p.x;


    if (point_check(x, x + w, y, y + h)) {
      if(!*pressed){
        press_button();
        *refresh = true;
        *pressed = true;
        return true;
      }
      else{
        *refresh = false;
        return false;
      }
    }
    *pressed = false;
    *refresh = false;
    return false;
  }
    
  else{
    *pressed = false;
    *refresh = false;
  }

  *pressed = false;
  return false;

}


 
void draw_title(String title) {

  int16_t x1, y1;
  static uint16_t size[2] = {0, 0};
  //calculate center of the button
  tft.setFont(&Nunito_Black20pt7b);
  tft.getTextBounds(title, 0, 0, &x1, &y1, &size[0], &size[1]);  //calc width of new string
  tft.setCursor(240 - size[0] / 2, size[1] + 3);
  tft.setTextColor(TFT_WHITE);
  tft.print(title);

}



void draw_title(String title, int color) {

  int16_t x1, y1;
  static uint16_t size[2] = {0, 0};
  //calculate center of the button
  tft.setFont(&Nunito_Black20pt7b);
  tft.getTextBounds(title, 0, 0, &x1, &y1, &size[0], &size[1]);  //calc width of new string
  tft.setCursor(240 - size[0] / 2, size[1] + 3);
  tft.setTextColor(color);
  tft.print(title);

}



void draw_title(String title, const GFXfont * font) {

  int16_t x1, y1;
  static uint16_t size[2] = {0, 0};
  //calculate center of the button
  tft.setFont(font);
  tft.getTextBounds(title, 0, 0, &x1, &y1, &size[0], &size[1]);  //calc width of new string
  tft.setCursor(240 - size[0] / 2, size[1] + 3);
  tft.setTextColor(TFT_WHITE);
  tft.print(title);

}



void draw_title_two(String title_1, String title_2, const GFXfont * font){
  int16_t x1 = 0;
  int16_t y1 = 0;
  static uint16_t size1[2] = {0, 0};
  static uint16_t size2[2] = {0, 0};
  uint16_t bottom;
  //calculate center of the button
  tft.setFont(font);
  tft.getTextBounds(title_1, 0, 0, &x1, &y1, &size1[0], &size1[1]);
  bottom = size1[1] + 3;
  tft.getTextBounds(title_2, 0, 0, &x1, &y1, &size2[0], &size2[1]);  //calc width of new string
  tft.setCursor(240 - size1[0] / 2, size1[1] + 3);
  tft.setTextColor(TFT_WHITE);
  tft.print(title_1);
  tft.setCursor(240 - size2[0] / 2, size2[1] + 3 + bottom);
  tft.setTextColor(TFT_WHITE);
  tft.print(title_2);
}


/*
 * Draw line functions draw strings to a specified line. If an x value line isnt specified it
 * centers the string to the screen
 */
void draw_line(int y, String line, int color) {
  int16_t x1, y1;
  static uint16_t size[2] = {0, 0};
  //calculate center of the button
  tft.setFont(&Rajdhani_SemiBold20pt7b);
  tft.getTextBounds(line, 0, 0, &x1, &y1, &size[0], &size[1]);  //calc width of new string
  tft.setCursor(240 - size[0] / 2, y);
  tft.setTextColor(color);
  tft.print(line);

}



void draw_line(int y, String line, int color, const GFXfont * font) {
  int16_t x1, y1;
  static uint16_t size[2] = {0, 0};
  //calculate center
  tft.setFont(font);
  tft.getTextBounds(line, 0, 0, &x1, &y1, &size[0], &size[1]);  //calc width of new string
  tft.setCursor(240 - size[0] / 2, y);
  tft.setTextColor(color);
  tft.print(line);

}



void draw_line(int x, int y, String line, int color, const GFXfont * font) {

  tft.setFont(font);
  tft.setCursor(x, y);
  tft.setTextColor(color);
  degree_symbol(line, font);
  tft.print(line);

}

/*
 * Draws a floating point value to a specified precision and its unit to the screen
 */
void draw_line(int x, int y, float value, int prec, String line, int color, const GFXfont * font) {

  tft.setFont(font);
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.print(value, prec);
  degree_symbol(line, font);
  tft.print(line);

}

/// TODO: Only used two times, can we get rid of this?
void degree_symbol(String line, const GFXfont* font)
{
  // To draw a degree symbol instead of an asterisk
  if (line == "F" || line == "C") {
    int height = 0;
    if (font == &Rajdhani_SemiBold20pt7b) height = 20;
    if (font == &Rajdhani_SemiBold12pt7b) height = 12;
    if (font == &Rajdhani_SemiBold9pt7b)  height =  7;
    tft.setCursor(tft.getCursorX() + 5, tft.getCursorY() - height);
    tft.setFont(&Rajdhani_SemiBold9pt7b);
    tft.print("o");
    tft.setCursor(tft.getCursorX(), tft.getCursorY() + height);
    tft.setFont(font);
  }
}

/*
 * Draws an int to the screen
 */
void draw_num(int x, int y, int number, int color, const GFXfont * font) {

  tft.setFont(font);
  tft.setCursor(x, y);
  tft.setTextColor(color);
  if (number < 10) {
    tft.print(0);
    tft.print(number);
  }
  else
    tft.print(number);
}


/*
   Helper function.
   Used by every button to check if the touch was within
   specified bounds
*/
bool point_check(int px, int px2, int py, int py2) {
  return ((ts_x > px) && (ts_x < px2) && (ts_y > py) && (ts_y < py2));
}

/// TODO: The following three functions are almost exactly the same, differing only in offses. Combine them.
/*
   Countdown function.
   Keeps track of the time left for the cycle.
   Can be adjusted by the user. Does not affect the default time.
*/
void time_countdown(Time* defaultTime) {


    static int prev_selection;
    Time prevRemaining = remaining;
    remaining = *defaultTime - CycleTime;

    if (!done) {
        draw_line(260, 105, ":", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
        draw_line(335, 105, ":", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    }

    
    //update each section of time individually
    if (prevRemaining.Seconds != remaining.Seconds || !done) {
        tft.fillRect(345, 60, 60, 60, BACKGROUND);
        draw_num(280 + 70, 105, remaining.Seconds, TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    }

    if (prevRemaining.Minutes != remaining.Minutes || !done || prev_selection != selection) {
        if (selection == 2) tft.fillRoundRect(270, 60, 60, 60, 10, MAIN_THEME);
        else tft.fillRect(270, 60, 60, 60, BACKGROUND);
        draw_num(205 + 70, 105, remaining.Minutes, TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    }

    if (prevRemaining.Hours != remaining.Hours || !done || prev_selection != selection) {
        if (selection == 1) tft.fillRoundRect(195, 60, 60, 60, 10, MAIN_THEME);
        else tft.fillRect(195, 60, 60, 60, BACKGROUND);
        draw_num(130 + 70, 105, remaining.totalHours(), TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    }
    prev_selection = selection;

    if (draw_arrow_button(115, 60, true)) {
        if (selection == 1) {
            *defaultTime = *defaultTime - Time(1, 0, 0);
            delay(100);
        }
        else if (selection == 2) {
            *defaultTime = *defaultTime - Time(0, 1, 0);
            delay(100);
        }
        else {
            *defaultTime = *defaultTime - Time(0, 15, 0);
            delay(100);
        }

    }
    if (draw_arrow_button(415, 60, false)) {
        if (selection == 1) {
            if (remaining.totalHours() < 99) {
                *defaultTime = *defaultTime + Time(1, 0, 0);
                delay(100);
            }
        }
        else if (selection == 2) {
            if (remaining.totalHours() < 99) {
                *defaultTime = *defaultTime + Time(0, 1, 0);
                delay(100);
            }
        }
        else {
            if (remaining.totalHours() < 99) {
                *defaultTime = *defaultTime + Time(0, 15, 0);
                delay(100);
            }
        }
    }

    if (invisible_button(195, 60, 60, 60)) {
        if (selection != 1) {
            prev_selection = selection;
            selection = 1;
        }
    }

    if (invisible_button(270, 60, 60, 60)) {
        if (selection != 2) {
            prev_selection = selection;
            selection = 2;
        }
    }

    if (invisible_touch(115, 415, 195, 270, 60)) {
        prev_selection = selection;
        selection = 0;
    }
}

/*
  Creates a countdown timer but can be modified to which ever location
*/
void time_countdown_manual(Time * defaultTime, int x, int y, int length) {  //lenght is the distance between arrows. -values bring it close, +values spread them apart

  static int prev_selection;
  Time prevRemaining = remaining;
  remaining = *defaultTime - CycleTime;

//creates the : inbetween the time values
  if(!done) {
    draw_line(x + 260, y + 105, ":", TFT_WHITE, &Rajdhani_SemiBold20pt7b);    
    draw_line(x + 335, y + 105, ":", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
  }

  //checks to see if the time has run out. Prevent negative numbers from displaying. 
  //Display zeros
  if(remaining.Seconds < 0 || remaining.Minutes < 0 || remaining.Hours < 0){
    tft.fillRect(x + 345, y + 60, 60, 60,BACKGROUND);
    tft.fillRect(x + 270, y + 60, 60, 60,BACKGROUND);
    tft.fillRect(x + 195, y + 60, 60, 60,BACKGROUND);
    draw_line(x + 200, y + 105, "00", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    draw_line(x + 275, y + 105, "00", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    draw_line(x + 350, y + 105, "00", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    }
  else{
  //update each section of time individually
    if(prevRemaining.Seconds != remaining.Seconds || !done){
      tft.fillRect(x + 345, y + 60, 60, 60,BACKGROUND);
      draw_num(x + 350, y + 105, remaining.Seconds, TFT_WHITE, &Rajdhani_SemiBold20pt7b); 
    }

    if(prevRemaining.Minutes != remaining.Minutes || !done || prev_selection != selection){
      if(selection == 2) tft.fillRoundRect(x + 270, y + 60, 60, 60, 10, MAIN_THEME);
      else tft.fillRect(x + 270, y + 60, 60, 60,BACKGROUND);
      draw_num(x + 275, y + 105, remaining.Minutes, TFT_WHITE, &Rajdhani_SemiBold20pt7b); 
    }

    if(prevRemaining.Hours != remaining.Hours || !done || prev_selection != selection){
      if(selection == 1) tft.fillRoundRect(x + 195, y + 60, 60, 60, 10, MAIN_THEME);
      else tft.fillRect(x + 195, y + 60, 60, 60,BACKGROUND);
      draw_num(x + 200, y + 105, remaining.totalHours(), TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    }
  }
  prev_selection = selection;  
//creates the arrows and incements depending on which on was pressed
  if (draw_arrow_button(x + 115 - length, y + 60, true)) {
    if (selection == 1) {
      *defaultTime = *defaultTime - Time(1, 0, 0);
      delay(100);
    }
    else if (selection == 2) {
      *defaultTime = *defaultTime - Time(0, 1, 0);
      delay(100);
    }
    else {
      *defaultTime = *defaultTime - Time(0, 15, 0);
      delay(100);
    }

  }
  if (draw_arrow_button(x + 415 + length, y + 60, false)) {
    if (selection == 1) {
      if(remaining.totalHours() < 99){ *defaultTime = *defaultTime + Time(1, 0, 0);
      delay(100);
      }
    }
    else if (selection == 2) {
      if(remaining.totalHours() < 99){ *defaultTime = *defaultTime + Time(0, 1, 0);
      delay(100);
      }
    }
    else {
      if(remaining.totalHours() < 99){ *defaultTime = *defaultTime + Time(0, 15, 0);
      delay(100);
      }
    }
  }

//creates a button behind the number so that specific number can be changed
  //this one is for the hour option
  if (invisible_button(x + 195, y + 60, 60, 60)) {
    if(selection != 1){
      prev_selection = selection;
      selection = 1;
    }    
  }
  //this is the minute select
  if (invisible_button(x + 270, y + 60, 60, 60)) {
    if(selection != 2){
      prev_selection = selection;
      selection = 2;
    }
  }
  //if anywhere else is pressed, default to no selection
  if (invisible_touch(x + 115, 415, 195, 270, y+60)) {
    prev_selection = selection;
    selection = 0;    
  }
}
// end of time_countdown_manual()


/*
   Countdown function.
   Keeps track of the time left for the cycle.
   Can be adjusted by the user. Does not affect the default time.
*/
void time_countdown_candy(Time * defaultTime) {

  static int prev_selection;
  Time prevRemaining = remaining;
  remaining = *defaultTime - CycleTime;


  if(!done) {
    draw_line(240, 185, ":", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    draw_line(315, 185, ":", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
  }

  //checks to see if the time has run out. Prevent negative numbers from displaying. 
  //Display zeros
  if(remaining.Seconds < 0 || remaining.Minutes < 0 || remaining.Hours < 0){
    tft.fillRect(325, 140, 60, 60,BACKGROUND);
    tft.fillRect(250, 140, 60, 60,BACKGROUND);
    tft.fillRect(175, 140, 60, 60,BACKGROUND);
    draw_line(110 + 70, 185, "00", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    draw_line(185 + 70, 185, "00", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    draw_line(260 + 70, 185, "00", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    }
  else{
  //update each section of time individually
    if(prevRemaining.Seconds != remaining.Seconds || !done){
      tft.fillRect(325, 140, 60, 60, BACKGROUND);
      draw_num(260 + 70, 185, remaining.Seconds, TFT_WHITE, &Rajdhani_SemiBold20pt7b); 
    }
  
    if(prevRemaining.Minutes != remaining.Minutes || !done || prev_selection != selection){
      if(selection == 2) tft.fillRoundRect(250, 140, 60, 60, 10, MAIN_THEME);
      else tft.fillRect(250, 140, 60, 60, BACKGROUND);
      draw_num(185 + 70, 185, remaining.Minutes, TFT_WHITE, &Rajdhani_SemiBold20pt7b); 
    }
  
    if(prevRemaining.Hours != remaining.Hours || !done || prev_selection != selection){
      if(selection == 1) tft.fillRoundRect(175, 140, 60, 60, 10, MAIN_THEME);
      else tft.fillRect(175, 140, 60, 60, BACKGROUND);
      draw_num(110 + 70, 185, remaining.totalHours(), TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    }
  }
  prev_selection = selection;  

  if (draw_arrow_button(95, 140, true)) {
    if (selection == 1) {
      *defaultTime = *defaultTime - Time(1, 0, 0);
      delay(100);
    }
    else if (selection == 2) {
      *defaultTime = *defaultTime - Time(0, 1, 0);
      delay(100);
    }
    else {
      *defaultTime = *defaultTime - Time(0, 15, 0);
      delay(100);
    }

  }
  if (draw_arrow_button(395, 140, false)) {
    if (selection == 1) {
      if(remaining.totalHours() < 99){ *defaultTime = *defaultTime + Time(1, 0, 0);
      delay(100);
      }
    }
    else if (selection == 2) {
      if(remaining.totalHours() < 99){ *defaultTime = *defaultTime + Time(0, 1, 0);
      delay(100);
      }
    }
    else {
      if(remaining.totalHours() < 99){ *defaultTime = *defaultTime + Time(0, 15, 0);
      delay(100);
      }
    }
  }

  if (invisible_button(175, 140, 60, 60)) {
    if(selection != 1){
      prev_selection = selection;
      selection = 1;
    }    
  }

  if (invisible_button(250, 140, 60, 60)) {
    if(selection != 2){
      prev_selection = selection;
      selection = 2;
    }
  }
  
  if (invisible_touch(95, 415, 195, 270, 140)) {
    prev_selection = selection;
    selection = 0;    
  }
}




void default_recipe_changer(int y, int *value, String label, bool *refresh) {
  String Recipe;
  switch(recipe_sel){
    case 1: Recipe = "Candy"; break;
    case 2: Recipe = "Greens"; break;
    case 3: Recipe = "Meat"; break;
    case 4: Recipe = "Fruit"; break;
    case 5: Recipe = "Liquid"; break;
    case 6: Recipe = "Pri Delicates"; break;
    case 7: Recipe = "Sec Delicates"; break;
  }
  if(!done) {
    draw_line(10, y + 15, Recipe, TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    draw_line(10, y + 40, label, TFT_WHITE, &Rajdhani_SemiBold12pt7b);
  }
  if (label == "Freeze Time:" || label == "Dry Time:") {
  // Converts from seconds to hours and minutes
    int hours = *value / 3600;
    int minutes = (*value / 60) % 60; 
    
    if (*refresh == true || !done) {
  // Draw the measurement
      tft.fillRect(225, y-15, 155, 50, BACKGROUND);  // Refresh measurements
      draw_num(250, y+30, hours, TFT_WHITE, &Rajdhani_SemiBold20pt7b);
      draw_line(300, y + 30, ":", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
      draw_num(313, y+30, minutes, TFT_WHITE, &Rajdhani_SemiBold20pt7b);
      *refresh = false;  // will stop time from refreshing constantly
    }
  // Draws the arrow buttons that change the recipe settings
    if(draw_arrow_button(150, y-10, true)){
      if(hours > 0){*value -= 15*60; *refresh = true; delay(125); return;}  // what is the 15*60 for?
      else if(minutes > 20) {*value -= 5*60; *refresh = true; delay(125); return;}
      else if(minutes > 0) {*value -= 1*60; *refresh = true; delay(125); return;}
      else delay(125);
    }
    if(draw_arrow_button(400, y-10, false)){
      if(hours < 48){
        if(hours > 0){*value += 15*60; *refresh = true; delay(125); return;}
        else if(minutes > 19) {*value += 5*60; *refresh = true; delay(125); return;}
        else {*value += 1*60; *refresh = true; delay(125); return;}
      }
      else delay(125);
    }
  }

int convert_unit_test;

  if (label == "Pressure:") {  // Other than candy, turn pressure red at over 300 Pa

  // Draws the arrow buttons that change the recipe pressure
    if(draw_arrow_button(150, y-10, true)) {
        if (*value > 600) *value -= 25; 
        else if (*value > 40 + OFFSET) *value -= 5;
        *refresh = true; delay(125);

    }
    
    if(draw_arrow_button(400, y-10, false)) {
      if (*value < 600) *value += 5; 
      else if (*value < 1000) *value += 25;
      *refresh = true; delay(125);
    }

    if (*refresh == true || !done) {
  // Other than candy, turn pressure measurement red at over 300 Pa
      if (Recipe != "Candy" || *value < 110) {
        if (*value > 200 || *value < 80) {
          if (*value > 995){
  // Code to change the pressure units
            switch (pressure_unit){
              case 0:  
                tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
                draw_line(250, y+30, String(*value) + "Pa", TFT_YELLOW, &Rajdhani_SemiBold20pt7b);      
                break;

              case 1:
                tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
                convert_unit_test = (*value * 7.5006167382113); 
                draw_line(255, y+30, String(convert_unit_test) + "mTorr", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
                break;

              case 2:
                tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
                convert_unit_test = (*value * 0.01);
                draw_line(255, y+30, String(convert_unit_test) + "mBar", TFT_YELLOW, &Rajdhani_SemiBold12pt7b); 
                break;

              case 3:
                tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
                convert_unit_test = (*value * 0.00014503773772954);
                draw_line(265, y+30, String(convert_unit_test) + "PSI", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
                break;
            } 
          }
          else {
  // Code to change the pressure units
            switch (pressure_unit){
              case 0:  
                tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
                draw_line(265, y+30, String(*value) + "Pa", TFT_YELLOW, &Rajdhani_SemiBold20pt7b);     
                break;

              case 1:
                tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
                convert_unit_test = (*value * 7.5006167382113); 
                draw_line(255, y+30, String(convert_unit_test) + "mTorr", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
                break;

              case 2:
                tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
                convert_unit_test = (*value * 0.01);
                draw_line(255, y+30, String(convert_unit_test) + "mBar", TFT_YELLOW, &Rajdhani_SemiBold12pt7b); 
                break;

              case 3:
                tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
                convert_unit_test = (*value * 0.00014503773772954);
                draw_line(265, y+30, String(convert_unit_test) + "PSI", TFT_YELLOW, &Rajdhani_SemiBold12pt7b);
                break;
            } 
          }
        }
        else {
  // Draw measurement white for under 600 Pa
          if (*value > 995){
  // Code to change the pressure units
            switch (pressure_unit){
              case 0:  
                tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
                draw_line(250, y+30, String(*value) + "Pa", TFT_WHITE, &Rajdhani_SemiBold20pt7b);     
                break;

              case 1:
                tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
                convert_unit_test = (*value * 7.5006167382113); 
                draw_line(255, y+30, String(convert_unit_test) + "mTorr", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
                break;

              case 2:
                tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
                convert_unit_test = (*value * 0.01);
                draw_line(255, y+30, String(convert_unit_test) + "mBar", TFT_WHITE, &Rajdhani_SemiBold12pt7b); 
                break;

              case 3:
                tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
                convert_unit_test = (*value * 0.00014503773772954);
                draw_line(265, y+30, String(convert_unit_test) + "PSI", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
                break;
            } 
          }

          else {
  // Code to change the pressure units
            switch (pressure_unit){
              case 0:  
                tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
                draw_line(265, y+30, String(*value) + "Pa", TFT_WHITE, &Rajdhani_SemiBold20pt7b);     
                break;

              case 1:
                tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
                convert_unit_test = (*value * 7.5006167382113); 
                draw_line(255, y+30, String(convert_unit_test) + "mTorr", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
                break;

              case 2:
                tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
                convert_unit_test = (*value * 0.01);
                draw_line(255, y+30, String(convert_unit_test) + "mBar", TFT_WHITE, &Rajdhani_SemiBold12pt7b); 
                break;

              case 3:
                tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
                convert_unit_test = (*value * 0.00014503773772954);
                draw_line(265, y+30, String(convert_unit_test) + "PSI", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
                break;
            } 
          }
        }
      }
      
      else if (Recipe == "Candy") {
  // Draw measurement white for candy
        if (*value > 995){
          switch (pressure_unit){
            case 0:  
              tft.fillRect(250, y-15, 150, 100, BACKGROUND);  // Refresh measurements
              draw_line(250, y+30, String(*value) + "Pa", TFT_WHITE, &Rajdhani_SemiBold20pt7b);     
              break;

            case 1:
              tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
              convert_unit_test = (*value * 7.5006167382113); 
              draw_line(255, y+30, String(convert_unit_test) + "mTorr", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
              break;

            case 2:
              tft.fillRect(220, y-15, 150, 100, BACKGROUND);  // Refresh measurements
              convert_unit_test = (*value * 0.01);
              draw_line(255, y+30, String(convert_unit_test) + "mBar", TFT_WHITE, &Rajdhani_SemiBold12pt7b); 
              break;

            case 3:
              tft.fillRect(250, y-15, 150, 100, BACKGROUND);  // Refresh measurements
              convert_unit_test = (*value * 0.00014503773772954);
              draw_line(265, y+30, String(convert_unit_test) + "PSI", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
              break;
          } 
        }
        else {
          switch (pressure_unit){
            case 0:  
              tft.fillRect(250, y-15, 150, 100, BACKGROUND);  // Refresh measurements
              draw_line(265, y+30, String(*value) + "Pa", TFT_WHITE, &Rajdhani_SemiBold20pt7b);     
              break;

            case 1:
              tft.fillRect(220, y-15, 175, 100, BACKGROUND);  // Refresh measurements
              convert_unit_test = (*value * 7.5006167382113); 
              draw_line(255, y+30, String(convert_unit_test) + "mTorr", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
              break;

            case 2:
              tft.fillRect(220, y-15, 150, 100, BACKGROUND);  // Refresh measurements
              convert_unit_test = (*value * 0.01);
              draw_line(255, y+30, String(convert_unit_test) + "mBar", TFT_WHITE, &Rajdhani_SemiBold12pt7b); 
              break;

            case 3:
              tft.fillRect(250, y-15, 150, 100, BACKGROUND);  // Refresh measurements
              convert_unit_test = (*value * 0.00014503773772954);
              draw_line(265, y+30, String(convert_unit_test) + "PSI", TFT_WHITE, &Rajdhani_SemiBold12pt7b);
              break;
          } 
        }
      }   
      *refresh = false;
    }
    
    
  }
}

void default_recipe_changer(int y, double *value, String label, bool *refresh) {
  String Recipe;
  switch(recipe_sel){
    case 1: Recipe = "Candy"; break;
    case 2: Recipe = "Greens"; break;
    case 3: Recipe = "Meat"; break;
    case 4: Recipe = "Fruit"; break;
    case 5: Recipe = "Liquid"; break;
    case 6: Recipe = "Delicates"; break;
  }
  if(!done) {
    draw_line(10, y + 15, Recipe, TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    draw_line(10, y + 40, label, TFT_WHITE, &Rajdhani_SemiBold12pt7b);
  }
  if (*refresh == true || !done){
  // Draw the measurement
    tft.fillRect(225, y-15, 155, 50, BACKGROUND);  // Refresh measurements
    if (!temp_unit) draw_line(265, y+30, String(int(round(*value))) + "C", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    else draw_line(265, y+30, String(int(round(*value*9/5+32))) + "F", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    *refresh = false;
  }
  // Draws the arrow buttons that change the recipe drying temperature
  // These values are Read as units of fahrenheit
  if(draw_arrow_button(150, y-10, true)) {
    if (*value > -17) {  // 0 F
      if (temp_unit) *value -= .55; 
      else if (!temp_unit) *value -= 1;
        *refresh = true; delay(125);
    }
    else delay(125);
  }
  // These values are Read as units of fahrenheit
  if(draw_arrow_button(400, y-10, false)) {
    if (*value < 74) {  // 165 F
      if (temp_unit){
        *value += .55;
      }  
      else if (!temp_unit) *value += 1;
        *refresh = true; delay(125);
    }
    else delay(125);
  }
}

void recipe_reset(){
  switch(recipe_sel){
    case 1: Candy = Candy_FactoryDefault; current_recipe = Candy; break;
    case 2: Greens = Greens_FactoryDefault; current_recipe = Greens; break;
    case 3: Meat = Meat_FactoryDefault; current_recipe = Meat; break;
    case 4: Fruit = Fruit_FactoryDefault; current_recipe = Fruit; break;
    case 5: Liquid = Liquid_FactoryDefault; current_recipe = Liquid; break;
    case 6: Delicates = Delicates_FactoryDefault; current_recipe = Delicates; break;
  }
}
/*
 * This functions does a similar job as time countdown but does
 * not countdown and display in realtime. Used for settings 
 * page only! And page 42
 */
void default_changer(int x, int y, Time* default_time, Time* prev_time, int* select, int* prev_select)
{
    //refresh time when changed or when selection has changed
    if (*default_time != *prev_time || *select != *prev_select || !done)
    {
        tft.fillRect(250, y, 155, 60, BACKGROUND);

        switch (*select) {
        case 1:
            tft.fillRoundRect(260, y, 60, 60, 10, MAIN_THEME);
            break;

        case 2:
            tft.fillRoundRect(333, y, 60, 60, 10, MAIN_THEME);
            break;
        }

        draw_line(325, y + 45, ":", TFT_WHITE, &Rajdhani_SemiBold20pt7b);

        //draw the hours up to 99 hours
        draw_num(265, y + 45, default_time->Hours + (default_time->Days) * 24, TFT_WHITE, &Rajdhani_SemiBold20pt7b);
        draw_num(338, y + 45, default_time->Minutes, TFT_WHITE, &Rajdhani_SemiBold20pt7b);

        //keep track of the previous values to check for changes
        *prev_time = *default_time;
        *prev_select = *select;
    }

    //draw left arrow button that decreases the time remaining depending on the selection
    if (draw_arrow_button(185, y, true))
    {
        switch (*select) {
        case 1:
            *default_time -= 1_hr;
            delay(100);
            break;

        case 2:
            *default_time -= 1_min;
            delay(100);
            break;

        default:
            *default_time -= 15_min;
            delay(100);
        }
    }

    //draw right arrow button that increases the time remaining depending on the selection
    if (draw_arrow_button(410, y, false) && default_time->totalHours() < 99)
    {
        switch (*select) {
        case 1:
            *default_time += 1_hr;
            delay(100);
            break;

        case 2:
            *default_time += 1_min;
            delay(100);
            break;

        default:
            *default_time += 15_min;
            delay(100);
        }
    }

    //create an invisible button for the hours selection 
    if (invisible_button(260, y, 60, 60) && *select != 1)
    {
        *prev_select = *select;
        *select = 1;
    }

    //create an invisible button for the minutes selection
    if (invisible_button(333, y, 60, 60) && *select != 2)
    {
        *prev_select = *select;
        *select = 2;
    }

    //deselect the selection if the arrows or another selection box is not pressed
    if (invisible_touch(260, 333, 185, 410, y))
    {
        *prev_select = *select;
        *select = 0;
    }

}

/*
 * This functions does a similar job as time countdown but does
 * not countdown and display in realtime. Used for settings 
 * page only! And page 42
 */
void prewarm_changer(int x, int y, Time *default_time, Time *prev_time, int * select, int * prev_select){
  //refresh time when changed or when selection has changed
  if (*default_time != *prev_time || *select !=  *prev_select || !done) {
    tft.fillRect(250, y, 155, 60, BACKGROUND);
    if (*select == 1) {
      tft.fillRoundRect(260, y, 60, 60, 10, MAIN_THEME);
    }
    else if (*select == 2)
      tft.fillRoundRect(333, y, 60, 60, 10, MAIN_THEME);
    draw_line(325, y + 45, ":", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
  //draw the hours up to 99 hours
    draw_num(265, y + 45, default_time->Hours + (default_time->Days)* 24, TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    draw_num(338, y + 45, default_time->Minutes, TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    
  //keep track of the previous values to check for changes
    *prev_time = *default_time;
    *prev_select = *select;
  }
  //draw left arrow button that decreases the time remaining depending on the selection
  if(draw_arrow_button(185, y, true)){
     if (*select == 1) {
      if(*default_time - Time(1, 0, 0) > 0)*default_time = *default_time - Time(1, 0, 0);
      delay(100);
    }
    else if (*select == 2) {
      if(*default_time - Time(0, 1, 0) > 0)*default_time = *default_time - Time(0, 1, 0);
      delay(100);
    }
    else {
      if(*default_time - Time(0, 15, 0) > 0)*default_time = *default_time - Time(0, 15, 0);
      delay(100);
    }
  }
  //draw right arrow button that increases the time remaining depending on the selection
  if(draw_arrow_button(410, y, false)){
    if (*select == 1) {
      if(default_time->Hours + (default_time->Days) * 24 < 10)*default_time = *default_time + Time(1, 0, 0);
      delay(100);
    }
    else if (*select == 2) {
      if(default_time->Hours + (default_time->Days) * 24 < 10) *default_time = *default_time + Time(0, 1, 0);
      delay(100);
    }
    else {
      if(default_time->Hours + (default_time->Days) * 24 < 10)*default_time = *default_time + Time(0, 15, 0);
      delay(100);
    }
  }
  //create an invisible button for the hours selection 
  if (invisible_button(260, y, 60, 60) && *select != 1) {
    *prev_select = *select;
    *select = 1;
  }
  //create an invisible button for the minutes selection
  if (invisible_button(333, y, 60, 60) && *select != 2) {
    *prev_select = *select;
    *select = 2;
  }
  //deselect the selection if the arrows or another selection box is not pressed
  if(invisible_touch(260, 333, 185, 410, y)){
    *prev_select = *select;
    *select = 0;
  }
  
}


/*
 * Draws the remaining time for the test.
 * Fundamentally different than the changer functions because 
 * there is no adjustability. So far only used for the test pages.
 * Could be used for other uses.
 */
void test_remaining(){
  Time prevRemaining = remaining;
  remaining = test_time - CycleTime;

  //do not display any negative numbers as time
  if(remaining.Seconds < 0 || remaining.Minutes < 0 || remaining.Hours < 0){
    tft.fillRect(325, 275, 60, 40,BACKGROUND);
    tft.fillRect(250, 275, 60, 40,BACKGROUND);
    tft.fillRect(175, 275, 60, 40,BACKGROUND);
    draw_line(135 + 50, 310, "00", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    draw_line(210 + 50, 310, "00", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    draw_line(285 + 50, 310, "00", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
  }
  else{
  //update each section of time individually
    if(prevRemaining.Seconds != remaining.Seconds || !done){
      tft.fillRect(325, 275, 60, 40,BACKGROUND);
      draw_num(285 + 50, 310, remaining.Seconds, TFT_WHITE, &Rajdhani_SemiBold20pt7b); 
    }
  
    if(prevRemaining.Minutes != remaining.Minutes || !done ){
      tft.fillRect(250, 275, 60, 40,BACKGROUND);
      draw_num(210 + 50, 310, remaining.Minutes, TFT_WHITE, &Rajdhani_SemiBold20pt7b); 
    }
  
    if(prevRemaining.Hours != remaining.Hours || !done ){
      tft.fillRect(175, 275, 60, 40,BACKGROUND);
      draw_num(135 + 50, 310, remaining.totalHours(), TFT_WHITE, &Rajdhani_SemiBold20pt7b);
    }
  }
}


/*
 * Draws the data for the tests page
 * Only one sensor data needs to be displayed per test
 */
void draw_test_live_data()
{
    switch (test_num)
    {
    case 1:
        if(Tests.off_gas && !Tests.leak_only)
        {
          ShelfTemp.Draw(340, 230);
        }
        draw_pressure(340, 250);
        break;

    case 2:
        ChmbrTemp.Draw(340, 250);
        break;

    case 3:
        ShelfTemp.Draw(340, 250);
        break;
    }
}


/*
   This function creates a button that does not have a fill
   color. Only outlines the area when touched
*/
bool invisible_button(int x, int y, int w, int h) {
  //check for touches
  if (ts.touched()) {
  //Serial.println("Some Touch");
    TS_Point p = ts.getPoint();
    ts_x = p.y;
    ts_y = tft.height() - p.x;


    if (point_check(x, x + w, y, y + h)) {
  //draw touch feedback      
      tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, TFT_WHITE);
      tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, TFT_WHITE);
      return true;
    }
    
    tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, BACKGROUND);
    tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, BACKGROUND);
    return false;
  }

  tft.drawRoundRect(x - 1, y - 1, w + 2, h + 2, 11, BACKGROUND);
  tft.drawRoundRect(x - 2, y - 2, w + 4, h + 4, 12, BACKGROUND);
  return false;
}


/*
 * Helper function for the value changers, takes 4 button x values as input
 * and returns if those buttons were not pressed. Used by the defaults page 
 * and for the dry and freeze pages as well
*/
bool invisible_touch(int x1, int x2, int x3, int x4, int y) {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    ts_x = p.y;
    ts_y = tft.height() - p.x;

    
    if (!(point_check(x1, x1 + 60, y, y + 60) || point_check(x2, x2 + 60, y, y + 60)
          || point_check(x3, x3 + 60, y, y + 60) || point_check(x4, x4 + 60, y, y + 60))) {
      return true;
    }
  }
  return false;
}


/*
 * This funtion is used to detect if the touch screen was pressed in any location.
 * Used for the hard ware test results page.
 */
bool invisible_touch_test() {
  if (ts.touched()) {
      return true;    
  }
  return false;
}


/*
   Displays elapsed time.
   Small format
   Small enough that there is barely any flicker
*/
void elapsed_time_small(int x, int y)
{
    static Time lastDraw = 0;

    if (!done)
    {
        tft.fillRect(x - 2, y - 16, 120, 18, BACKGROUND);
        draw_line(x + 33, y, ":", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
        draw_line(x + 73, y, ":", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
    }


    if (CycleTime.totalHours() != lastDraw.totalHours() || !done)
    {
        tft.fillRect(x, y+3, 20, -14, BACKGROUND);
        draw_num(x, y, CycleTime.totalHours(), TFT_WHITE, &Rajdhani_SemiBold9pt7b);
    }

    if (CycleTime().Minutes != lastDraw.Minutes || !done)
    {
        tft.fillRect(x+45, y+3, 20, -14, BACKGROUND);
        draw_num(x+45, y, CycleTime().Minutes, TFT_WHITE, &Rajdhani_SemiBold9pt7b);
    }

    if (CycleTime().Seconds != lastDraw.Seconds || !done)
    {
        tft.fillRect(x+85, y+3, 20, -14, BACKGROUND);
        draw_num(x+85, y, CycleTime().Seconds, TFT_WHITE, &Rajdhani_SemiBold9pt7b);
    }
    
    if (CycleTime != lastDraw)
        lastDraw = CycleTime;
}

/*
   Displays elapsed time
   Medium format. 
   No flicker
*/
void elapsed_time_medium(int x, int y)
{
    static Time lastDraw = 0;

    if (CycleTime.totalHours() != lastDraw.totalHours() || !done)
    {
        tft.fillRect(180, 170, 80, 25, BACKGROUND);
        draw_num(x, y, CycleTime.totalHours(), TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    }

    if (CycleTime().Minutes != lastDraw.Minutes || !done)
    {
        tft.fillRect(270, 170, 45, 25, BACKGROUND);
        draw_num(x + 60, y, CycleTime().Minutes, TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    }

    if (CycleTime().Seconds != lastDraw.Seconds || !done)
    {
        tft.fillRect(325, 170, 45, 25, BACKGROUND);
        draw_num(x + 120, y, CycleTime().Seconds, TFT_WHITE, &Rajdhani_SemiBold12pt7b);
    }


    if (CycleTime != lastDraw)
        lastDraw = CycleTime;

}


/*
   Controls the button for pressure unit change
   Pa  = 0, mTorr = 1, mBar = 2, psi = 3 
   Button shows the next unit that pressing will change to  
*/
void pressure_unit_changer(int x, int y, int size_) {

  static bool pressure_pressed;
  
  if (pressure_unit == 0) {
  //draw butons
    if (draw_button_change(x, y, size_, 45, MAIN_THEME, "mTorr", &Rajdhani_SemiBold9pt7b, &pressure_refresh, &pressure_pressed)) {
  //change the pressure unit
      pressure_unit = 1;
  //redraw pressure after unit change
      pressure_refresh = true;
      delay(150);
    }
  }
  else if (pressure_unit == 1) {
    if (draw_button_change(x, y, size_, 45, MAIN_THEME, "mBar", &Rajdhani_SemiBold9pt7b, &pressure_refresh, &pressure_pressed)) {
  //change the pressure unit
      pressure_unit = 2;
  //redraw pressure after unit change
      pressure_refresh = true;
      delay(150);
    }
  }
  else if (pressure_unit == 2) {
    if (draw_button_change(x, y, size_, 45, MAIN_THEME, "psi", &Rajdhani_SemiBold9pt7b, &pressure_refresh, &pressure_pressed)) {
  //change the pressure unit
      pressure_unit = 3;
  //redraw pressure after unit change
      pressure_refresh = true;
      delay(150);
    }
  }
  else if (pressure_unit == 3) {
    if (draw_button_change(x, y, size_, 45, MAIN_THEME, "Pa", &Rajdhani_SemiBold9pt7b, &pressure_refresh, &pressure_pressed)) {
  //change the pressure unit
      pressure_unit = 0;
  //redraw pressure after unit change
      pressure_refresh = true;
      delay(150);
    }
  }

}

/*
   Controls the button for temperature unit change
*/
void temp_unit_changer(int x, int y, int size_) {

  static bool temp_pressed;
  //draw unit changer button
  if (draw_button_change(x, y, size_, 45, MAIN_THEME, " C/   F", &Rajdhani_SemiBold9pt7b, &temp_refresh, &temp_pressed)) 
  {
    if (temp_unit) temp_unit = false;
    else temp_unit = true;
    temp_refresh = true;
    delay(150);
  }
  
}


/*
 * DIAGNOSTIC Buttons: Only used for the manual tests page
 * Moved here to simplify code
 */
 void diagnostic_buttons(bool * vacuum, bool * fridge, bool * shelves, bool * vacuum_override){

  static bool pressed_1 = false;
  static bool pressed_2 = false;
  static bool pressed_3 = false;

  //Serial.println(pressed_1);
  
  //draw diagnostic buttons
  //Vacuum Button
  if (*vacuum_override) {
      *vacuum = false;
      *vacuum_override = false;
      diagnostic_refresh_1 = true;
      draw_button_change_img(40, 70, 105, 105, TFT_GRAY, pump, &diagnostic_refresh_1, &pressed_1);
      delay(100);
  }
  else if (!(*vacuum)) {
    if(draw_button_change_img(40, 70, 105, 105, TFT_GRAY, pump, &diagnostic_refresh_1, &pressed_1)){
      *vacuum = true;
      diagnostic_refresh_1 = true;
      delay(100);
    }
  }
  else if (*vacuum) {
    if(draw_button_change_img(40, 70, 105, 105, TFT_GREEN2, pump, &diagnostic_refresh_1, &pressed_1)){
      *vacuum = false;
      diagnostic_refresh_1 = true;
      delay(100);
    }    
  }
  //Refrigeration button
  if(!(*fridge)){
    if(draw_button_change_img(185, 70, 105, 105, TFT_GRAY, fridge_icon, &diagnostic_refresh_2, &pressed_2)){
      *fridge = true;
      diagnostic_refresh_2 = true;
      delay(100);
    }
  }  
  else if(*fridge){
    if(draw_button_change_img(185, 70, 105, 105, TFT_GREEN2, fridge_icon, &diagnostic_refresh_2, &pressed_2)){
      *fridge = false; 
      diagnostic_refresh_2 = true;
      delay(100);
    } 
  }    
  //Heating shelves
  if(!(*shelves)){
    if(draw_button_change_img(330, 70, 105, 105, TFT_GRAY, heater, &diagnostic_refresh_3, &pressed_3)){
      *shelves = true;
      diagnostic_refresh_3 = true;
      delay(100);
    }    
  }
  else if(*shelves){
    if(draw_button_change_img(330, 70, 105, 105, TFT_GREEN2, heater, &diagnostic_refresh_3, &pressed_3)){
      *shelves = false;
      diagnostic_refresh_3 = true;
      delay(100);
    }    
  }
}



/*
 * Generates the buttons and output needed to draw changeable shelf temp
 * user interface. 
 * USED by dry cycle page and defaults page
 */
 void shelf_temp_changer(int x1, int x2, int x3, int y)
 {
     //draw buttons for changing shelf temperature
     if (draw_arrow_button(x2, y, false))
     {
         //prevent temperature from exceeding 65C or 150F
         if (ShelfTargetTemperature < 74)
         {
             ShelfTargetTemperature += temp_unit ? 5.0 / 9.0 : 1;
         }
         delay(100);

     }
     if (draw_arrow_button(x1, y, true))
     {
         //prevent temp from reaching below 0F
         if (ShelfTargetTemperature > -17.1)
         {
             ShelfTargetTemperature -= temp_unit ? 5.0 / 9.0 : 1;
         }
         delay(100);
     }



     // don't redraw if we don't have to

     static int lastDrawnTemperature;
     static bool   lastDrawnUnit;
     static int    lastDrawn_x3;
     static int    lastDrawn_y;

     int TargetT2Draw = (temp_unit ? (ShelfTargetTemperature * 9.0 / 5.0 + 32) : ShelfTargetTemperature) + 0.5;

     if (lastDrawnTemperature == TargetT2Draw
         && lastDrawnUnit == temp_unit
         && lastDrawn_x3 == x3
         && lastDrawn_y == y
         && done)
         return;

     lastDrawnTemperature = TargetT2Draw;
     lastDrawnUnit = temp_unit;
     lastDrawn_x3 = x3;
     lastDrawn_y = y;


     //draw change in temperature to screen
     tft.fillRect(x1 + 65, y, x2 - (x1 + 70), 60, BACKGROUND);
     draw_line(x3, y + 45, TargetT2Draw, 0, temp_unit ? "F" : "C", TFT_WHITE, &Rajdhani_SemiBold20pt7b);
 }

 void set_pressure_changer(int x1, int x2, int x3, int y) {

     static uint32_t last_drawn_pressure;
     //draw buttons for changing pressure
     if (draw_arrow_button(x2, y, false)) {
         last_drawn_pressure = set_pressure;
         if (set_pressure < 990 + OFFSET) {
             set_pressure += 10;
         }
         delay(100);
     }
     if (draw_arrow_button(x1, y, true)) {
         last_drawn_pressure = set_pressure;
         if (set_pressure > 40 + OFFSET) {
             set_pressure -= 10;
         }
         delay(100);
     }

     //draw change in pressure
     if (last_drawn_pressure != set_pressure || !done)
     {
         tft.fillRect(x1 + 65, y, x2 - (x1 + 70), 60, BACKGROUND);

         int color = (200 < set_pressure || set_pressure < 80) ? TFT_RED : TFT_WHITE;

         switch (pressure_unit) {
         case 0:
             draw_line(x3, y + 45, set_pressure, 0, " Pa", color, &Rajdhani_SemiBold20pt7b);
             break;

         case 1:
             draw_line(x3 - 5, y + 38, set_pressure * 7.5006167382113, 0, " mTorr", color, &Rajdhani_SemiBold12pt7b);
             break;

         case 2:
             draw_line(x3 - 17, y + 45, set_pressure * 0.01, 1, " mBar", color, &Rajdhani_SemiBold20pt7b);
             break;

         case 3:
             draw_line(x3 - 20, y + 45, set_pressure * 0.00014503773772954, 3, " PSI", color, &Rajdhani_SemiBold20pt7b);
             break;
         }

         last_drawn_pressure = set_pressure;
     }

 }

/*
   The Adafruit GFX library lacks the functions needed to make buttons look decent
   This is a function that generates rounded buttons with a color transition.   
*/
void drawRoundedRectYGradient(int x, int y, int w, int h, int rad, int color_1, int color_2) {
  //main variables for calculation
  float delta = -255.0 / h;
  float alpha = 255.0;
  uint32_t color = color_1;
  int i;  
  int r2 = rad*rad;
  int y_orig = y;
  int x2;
  int xCen = x + rad;
  int yCen = y + rad; 
  
  //top curve
  for (i = 9; i >= 0; i--) {
    x2 = (int) (sqrt(r2 - i*i) + 0.5);  
    tft.drawFastHLine(xCen - x2, yCen - i - 1, w - 2*(rad - x2), color);    
    y++;    
    alpha += delta;
    color = alphaBlend((uint8_t)alpha, color_1, color_2, 2);
  }

  //middle rectangle
  for (i = h - 2 * rad; i >= 0; i--) {
    tft.drawFastHLine(x, y++, w, color);
    alpha += delta;
    color = alphaBlend((uint8_t)alpha, color_1, color_2, 2);
  }
  
  //bottom curve
  xCen = x + rad;
  yCen = y_orig + h - rad; 
  for (i = 0; i < 10; i++) {
    x2 = (int) (sqrt(r2 - i*i) + 0.5);  
    tft.drawFastHLine(xCen - x2, yCen + i, w - 2*(rad - x2), color);    
    y++;    
    alpha += delta;
    color = alphaBlend((uint8_t)alpha, color_1, color_2, 2);
  }
}


/*
 * Taken from TFT_eSPI
 * Used for creating the gradient for buttons
 */
uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc)
{
  // For speed use fixed point maths and rounding to permit a power of 2 division
  uint16_t fgR = ((fgc >> 10) & 0x3E) + 1;
  uint16_t fgG = ((fgc >>  4) & 0x7E) + 1;
  uint16_t fgB = ((fgc <<  1) & 0x3E) + 1;

  uint16_t bgR = ((bgc >> 10) & 0x3E) + 1;
  uint16_t bgG = ((bgc >>  4) & 0x7E) + 1;
  uint16_t bgB = ((bgc <<  1) & 0x3E) + 1;

  // Shift right 1 to drop rounding bit and shift right 8 to divide by 256
  uint16_t r = (((fgR * alpha) + (bgR * (255 - alpha))) >> 9);
  uint16_t g = (((fgG * alpha) + (bgG * (255 - alpha))) >> 9);
  uint16_t b = (((fgB * alpha) + (bgB * (255 - alpha))) >> 9);

  // Combine RGB565 colours into 16 bits
  
  return (r << 11) | (g << 5) | (b << 0);
  
}

/*
 * Same as above except uses dithering to make the gradient 
 * more smooth.
 */
uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc, uint8_t dither)
{
  if (dither) {
    int16_t alphaDither = (int16_t)alpha - dither + random(2*dither+1);  // +/-4 randomised
    alpha = (uint8_t)alphaDither;
    if (alphaDither <  0) alpha = 0;
    if (alphaDither > 255) alpha = 255;
  }

  return alphaBlend(alpha, fgc, bgc);
}

void error_tone(bool *started, int repeat){  // Pass in a variable to tell if the sound has been started,
  static uint32_t note_duration = 0;  // Also pass in how many seconds before the sound repeats itself.
  if(button_sound || page == 47){
  // Reset note duration if false was passed in or repeat seconds was triggered 
    if (!*started || millis() > note_duration + (repeat * 1000)) {note_duration = millis(); *started = true;}
  // Play notes based on how long it has been since started was passed in as false.
    if (millis() < note_duration + 150)  tone(BUZZER, 440, 300);  // pin, note, duration
    else if (millis() < note_duration + 500)  tone(BUZZER, 880, 300);  // (8th note)
    else if (millis() > note_duration + 750) {noTone(BUZZER); return;}
  }
}


// Makes a short sound when a button is pressed
void press_button(){
  if(button_sound || page == 47){
    tone(11, 480);
    delay(30);
    noTone(11);
  }
}

double temp_changer_manual(int x, int y, double temp_changing, int max_temp, int min_temp, bool initialize){

  bool refresh_value = false;  //showing that nothing has been pressed yet
  if(!done){refresh_value = true;}  //allows us to draw initial value
  String temporary_string = "";
  if(draw_arrow_button(x, y, true)){
    if(temp_changing >= min_temp){
      refresh_value = true;
      if(temp_unit){temp_changing -= .55;delay(125);}
      else{temp_changing -= 1; delay(125);}
      }
    else{delay(125);}
    }
  if(draw_arrow_button(225 + x, y, false)){
    if(temp_changing <= max_temp){
      refresh_value = true;
      if(temp_unit){temp_changing += .55; delay(125);}
      else{temp_changing += 1; delay(125);}
      }
    else{delay(125);}
    }
    if(refresh_value == true){
      tft.fillRect(90+x, y + 5, 120, 50, BACKGROUND);
      if(temp_unit == false){
          temporary_string = String(temp_changing, 0) + "C";
        }
      else{
          double f_temp_pre_warm = round((temp_changing * 9 / 5) + 32);
          if(f_temp_pre_warm == 166){
            f_temp_pre_warm -= 1;
          }
          temporary_string = String(f_temp_pre_warm, 0) + "F";
        }
      draw_line(105+x, y + 40, temporary_string, TFT_WHITE,  &Rajdhani_SemiBold20pt7b);

    }
    return temp_changing;
}
