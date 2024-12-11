/* Function prototypes of routines in MultiFunctionShield.c */
/* Goes with Version 8 */


uint16_t Poll_POT_ADC_Value(void);
void Clear_LEDs(void);
void disp_adc_on_7seg(float inValue);
void Display_All(void);
void Display_the_FULL (void);
void Display_Waiting(void);
void MultiFunctionShield__ISRFunc(void);
void MultiFunctionShield_Clear(void);
void MultiFunctionShield_Display (int16_t value);
void MultiFunctionShield_Display_PWM(int16_t duty_cycle_percent);
void MultiFunctionShield_DisplayByteArray(unsigned char bytes[4]);
void MultiFunctionShield_DisplayByteAt(int digit, unsigned char value);
void MultiFunctionShield_DisplayBytes(uint32_t values);
void MultiFunctionShield_Single_Digit_Display (int digit, int8_t value);
void MultiFunctionShield_WriteNumberToSegment(uint8_t digit);
void OneSecond_Show_Potentiometer__ISRFunc(void);
void set_Decimal_Point (int position);
void UART_String(char* stringOut);
