/*
	A bunch of small commands (mostly time-related) to aid with debugging
*/
// these can be used in debugging to change things without rebuilding
int __a__ = 55; // 65
int __b__ = 75; // 60
int __c__ = 30; // 25

extern unsigned long time_pressure_went_invalid;
extern int time_delta_pressure_went_invalid;

void Command_Terminal()
{
	char arg1 = Serial.read();
	Time DT;

	switch (toupper(arg1))
	{
	case 'A':
		__a__ = Serial.parseInt();
		return;

	case 'B':
		__b__ = Serial.parseInt();
		return;

	case 'C':
		__c__ = Serial.parseInt();
		return;

	case 'P':
	{
		int p = Serial.parseInt();

		if (p) setPage(p);
		else  done = false;

		return;
	}

	case 'X':
	{
		bool en = RTC->MODE2.CTRLA.reg & RTC_MODE2_CTRLA_ENABLE;
		if (en) CycleTime.RTCdisable();
		else    CycleTime.RTCenable();

		return;
	}

	case 'D':
		DebugBox.x0 = Serial.parseInt();
		DebugBox.y0 = Serial.parseInt();
		DebugBox.x1 = Serial.parseInt();
		DebugBox.y1 = Serial.parseInt();
		return;

	case 'W':
		REG_WDT_CTRLA ^= WDT_CTRLA_ENABLE;
		return;

	case 'T':
		Wire.setTimeout(Serial.parseInt());
		return;

	default:
	{
		int s = Serial.parseInt();
		int m = Serial.parseInt();
		int h = Serial.parseInt();

		DT = Time(h, m, s);
	}
	}

	Serial.printf("> %c ", arg1);
	Serial.println(DT.toString());

	switch (arg1)
	{
	case '+':
		CycleTime += DT;


		ShelfTemp._uptime  += DT.totalSeconds() * 1000 * sgn(ShelfTemp.UpTime);
		ChmbrTemp._uptime  += DT.totalSeconds() * 1000 * sgn(ChmbrTemp.UpTime);
		PressureTimeOffset += DT.totalSeconds() * 1000;

		//TimeOfLastBufferPacket = ((DT.totalSeconds() * 1000) > TimeOfLastBufferPacket) ?
		//	0 : (TimeOfLastBufferPacket - DT.totalSeconds() * 1000);

		//TimeOfLastPressureRead = ((DT.totalSeconds() * 1000) > TimeOfLastPressureRead) ?
		//	0 : (TimeOfLastPressureRead - DT.totalSeconds() * 1000);



		break;

	case '-':
		CycleTime -= DT;

		ShelfTemp._uptime   = 0;
		ChmbrTemp._uptime = 0;
		PressureTimeOffset  = 0;
		break;

	case '=':
		CycleTime = DT;

		break;

	case '>':
		Serial.println(CycleTime > DT);
		break;


	case '<':
		Serial.println(CycleTime < DT);
		break;

	case '!':
		//if(time_pressure_went_invalid)
	{
		unsigned dtint = DT.totalSeconds() * 1000;

		if ((int)time_pressure_went_invalid - dtint <= 0)
			time_pressure_went_invalid = 0;
		else
			time_pressure_went_invalid -= dtint;

		if (time_delta_pressure_went_invalid - dtint <= 0)
			time_delta_pressure_went_invalid = 0;
		else
			time_delta_pressure_went_invalid -= dtint;

		break;
	}

		
	}

	if (ShelfTemp.UpTime   < 0) ShelfTemp._successRate   = 0.01; // this stops error-page bouncing when faking an error
	if (ChmbrTemp.UpTime < 0) ChmbrTemp._successRate = 0.01;

}

void print_ms(unsigned long ms)
{
	Time tspan = Time(ms / 1000);

	Serial.print(tspan.Minutes);
	Serial.print(":");
	Serial.print(tspan.Seconds);
}


/// 5 min heat
// show disconnected always