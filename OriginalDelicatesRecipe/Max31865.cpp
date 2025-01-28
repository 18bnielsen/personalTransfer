#include "Max31865.h"

void Max31865::AsyncRead()
{
	// time SINCE last read (or read-step)
	unsigned dt;

	/// Read sensor ///
	/*
	readRTD includes 75 ms of delays, which is unacceptable for fast asynchronous polling, so I am rewriting it here
	Corollarily, POLL_PERIOD should be larger than 75
	Functions called here are inherited from Adafruit_MAX31865 - I had to edit that class to make them protected instead of private
	*/
	switch (_readStep)
	{
	case 0:
	{
		dt = millis() - _timeofLastRead[0]; // use index 2 for POLL_DELAY instead of POLL_PERIOD
		if (dt < POLL_PERIOD) return;

		_thisFault = readFault();
		clearFault();
		enableBias(true);

		_timeofLastRead[0] = millis();
		_readStep++;
		return;
	}

	case 1:
	{
		dt = millis() - _timeofLastRead[0];
		if (dt < BIAS_DELAY) return;

		uint8_t t = readRegister8(MAX31865_CONFIG_REG);
		t |= MAX31865_CONFIG_1SHOT;
		writeRegister8(MAX31865_CONFIG_REG, t);

		_timeofLastRead[1] = millis();
		_readStep++;
		return;
	}
	case 2:
	{
		dt = millis() - _timeofLastRead[1];
		if (dt < CONF_DELAY) return;

		uint16_t rtd = readRegister16(MAX31865_RTDMSB_REG);
		enableBias(false); // Disable bias current again to reduce selfheating.
		rtd >>= 1; // remove fault bit

		_thisRead = calculateTemperature(rtd, _rNom, _rRef);

		// now dt is the time between calculations
		dt = millis() - _timeofLastRead[2];
		
		_timeofLastRead[2] = millis();
		_readStep = 0;
		break;
	}

	// just in case...
	default:
		_readStep = 0;
		return;
	}

	// apply a custom fault code if reading is out of absolute range
	if (_thisRead > TEMPERATURE_RANGE_MAX || _thisRead < TEMPERATURE_RANGE_MIN)
		_thisFault |= FaultCodes.RANGE;


	/// calculate success rate, temerature, and  ///
	_thisFault &= FAULTS_CHECKED;

	// compensate gain for actual time between reads (higher gain when dt is large)
	float SRateGain = dt / (TIME_CONSTANT * 1000.0);
	if (SRateGain > 1) SRateGain = 1; // keep _successRate between 0 and 1


	if (_thisFault)
		_successRate -= _successRate * SRateGain;  // big change when rate is high; small change when small
	else
	{
		_successRate += (1 - _successRate) * SRateGain;  // big change when rate small; small change when large

		// d(elta-)T(emperature), not to be confused with d(elta-)t(ime)
		float dT = _thisRead - _temp_c;

		// w = LP gain adjusted for real frequency - higher frequencies (more samples) = lower gain
		// Now our filter behaves the same regardless of POLL_PERIOD
		float w = LP_CONSTANT / (float)dt;

		// temperature is always recorded when available, even if we're also reporting a fault
		_temp_c += dT * w / (abs(dT) + w);  // low-pass filter
		_temp_f = (_temp_c * 9.0 / 5.0) + 32;
    //_temp_c = 0;
    //_temp_f = 0;
	}


	/// To report a FAULT or not to, that is the question ///

	// the threshold for considering us in or out of the FAULT state depends on the current state to create hysieresis
	float successThreshold = (_filteredFault ? NOMIMAL_SUCCESS_RATE : MIN_SUCCESS_RATE) / 100.0;

	// new faults persist until all faults (including others) are cleared
	_filteredFault = (_successRate < successThreshold) ? (_filteredFault | _thisFault) : 0;


	/// Calculate uptime ///
	// _uptime uses a seperate counter to _filteredFault since we usualy only care about it when it's fairly large (and _filteredFault will already be set properly by that point anyway)

	// online
	if (!_thisFault && (UpTime >= 0))
		_uptime += dt;

	// offline
	else if (_thisFault && (UpTime <= 0))
		_uptime -= dt;

	// changing states
	else _uptime = 0;
}


/// Draw() is spesific to our enviroment and as such has special needs ///
#include "MCUFRIEND_kbv.h"
#include "Fonts/Rajdhani_SemiBold9pt7b.h"
#include "Fonts/KronaOne_Regular4pt7b.h"
#include "Fonts/KronaOne_Regular5pt7b.h"
#include "Fonts/KronaOne_Regular6pt7b.h"

extern MCUFRIEND_kbv tft;
extern void draw_line(int x, int y, String line, int color, const GFXfont* font);
extern void draw_line(int x, int y, float value, int prec, String line, int color, const GFXfont* font);
extern const uint16_t bg_default;
extern bool done;
extern bool temp_unit;
extern int  page;

void Max31865::Draw(int x, int y, bool ignoreFault)
{
	double val = temp_unit ? Fahrenheit : Celsius;
	double valRound = round(val * 10);

	enum {
		x1 = 40, // width of number part
		x2 = 98, // width of Disconnected
	};

	// show if sensor disconnected
	if (!_lastDrawnFault != !Fault || !done)
	{
		_lastDrawnFault = (Fault > 0);

		tft.fillRect(x, y + 2, x2, -14, bg_default);

		if (Fault && !ignoreFault)
		{
			draw_line(x, y, "Disconnected", TFT_WHITE, &Rajdhani_SemiBold9pt7b);
			return;
		}
		else
		{
			// sensor reconnected
			// make this not-equal to force redrawing value and unit
			_lastDrawnUnit = !temp_unit;
			_lastDrawnTemp = -val;
		}
	}
	else if(Fault) return;


	// draw units
	if (temp_unit != _lastDrawnUnit || !done)
	{
		_lastDrawnUnit = temp_unit;

		tft.fillRect(x+x1, y+2, 20, -17, bg_default);
		tft.setTextColor(TFT_WHITE);

		tft.setCursor(x+x1, y-6);
		tft.setFont(&KronaOne_Regular4pt7b);   tft.print("0");

		tft.setCursor(x+x1+8, y);
		tft.setFont(&Rajdhani_SemiBold9pt7b);  tft.print(temp_unit ? "F" : "C");
	}


	// draw value
	if (_lastDrawnPage != page
	 || _lastDrawnX != x
	 || _lastDrawnY != y
	 || _lastDrawnTemp != valRound
	 || !done)
	{
		_lastDrawnPage = page;
		_lastDrawnX = x;
		_lastDrawnY = y;
		_lastDrawnTemp = valRound;


		tft.fillRect(x, y + 2, x1, -14, bg_default);

		tft.setFont(&Rajdhani_SemiBold9pt7b);
		tft.setCursor(x, y);
		tft.setTextColor(TFT_WHITE);
		tft.print(val, 1);
	}
}