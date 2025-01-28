/*
Things not-yet implemented
	- Get individual fault bits as boolean bitfield
	- Check if temp is within a more spesific range
	- Get derivative of temperature
*/

#pragma once

#include <Adafruit_MAX31865.h>

class Max31865 : private Adafruit_MAX31865
{
	friend void Command_Terminal(); // allows us to nudge the uptime during testing

public:
	static struct {
	enum {
		HIGHTHRESH = MAX31865_FAULT_HIGHTHRESH,  // 0x80
		LOWTHRESH  = MAX31865_FAULT_LOWTHRESH,   // 0x40
		REFINLOW   = MAX31865_FAULT_REFINLOW,    // 0x20
		REFINHIGH  = MAX31865_FAULT_REFINHIGH,   // 0x10
		RTDINLOW   = MAX31865_FAULT_RTDINLOW,    // 0x08
		OVUV       = MAX31865_FAULT_OVUV,        // 0x04

		RANGE      = 0x02,  // 0x02
	};
	} FaultCodes;

	// use bitfield in the future, maybe
	 //union Faults {
	 //	uint8_t raw;
	 //	struct {
	 //		bool HIGHTHRESH : 1; // 0x80
	 //		bool LOWTHRESH : 1; // 0x40
	 //		bool REFINLOW : 1; // 0x20
	 //		bool REFINHIGH : 1; // 0x10
	 //		bool RTDINLOW : 1; // 0x08
	 //		bool OVUV : 1; // 0x04
	 //		bool RANGE : 1; // 0x02
	 //		bool : 1;
	 //	};
	 //};

private:
	 /// constants ///
	const unsigned _rRef;  // resistance of refrence resistor
	const unsigned _rNom;  // value of sensor at 0 deg C
	enum {
		POLL_PERIOD = 100, // time between polls in milliseconds
		BIAS_DELAY = 10,   // hardcoded in Adafriut driver
		CONF_DELAY = 65,   // hardcoded in Adafriut driver

		MIN_SUCCESS_RATE = 15,      // Success rate less than MIN_SUCCESS_RATE percent causes us to enter FAULT state
		NOMIMAL_SUCCESS_RATE = 20,  // A success rate of at least NOMIMAL_SUCCESS_RATE percent is required to exit FAULT state
		TIME_CONSTANT = 10,         // SuccessRate gain (takes roughly this many seconds for a 75% change)
		LP_CONSTANT = 500,          // roughly how many degrees of change we expect per second, times 1000 (aka deg/ms)

		TEMPERATURE_RANGE_MAX = 100,
		TEMPERATURE_RANGE_MIN = -60,

		FAULTS_CHECKED = 0
//		| MAX31865_FAULT_HIGHTHRESH // thrown when sensor not present
//		| MAX31865_FAULT_LOWTHRESH
//		| MAX31865_FAULT_REFINLOW   // thrown when sensor not present
//		| MAX31865_FAULT_REFINHIGH
//		| MAX31865_FAULT_RTDINLOW   // thrown when sensor not present
//		| MAX31865_FAULT_OVUV
		| FaultCodes.RANGE,         // thrown when sensor not present
	};

	/// semi-static variables for asynchronous reads ///
	unsigned _timeofLastRead[3] = { 0 };
	int _readStep = 0;
	uint8_t _thisFault;
	float _thisRead;

	/// semi-static variables for drawing ///
	double _lastDrawnTemp;
	bool _lastDrawnFault;
	bool _lastDrawnUnit;
	int _lastDrawnX;
	int _lastDrawnY;
	int _lastDrawnPage;

	/// Readonly properties ///
	float _successRate = 0.5; // DO NOT set this to 0 or 1; the _successRate will NEVER change in those cases
	uint8_t _filteredFault;
	int _uptime;
	double _temp_c;
	double _temp_f;


public:
	Max31865(int8_t cs, const unsigned rRef, const unsigned rNom) : Adafruit_MAX31865(cs), _rRef(rRef), _rNom(rNom)
	{
		_timeofLastRead[0] = random(POLL_PERIOD);  // offset the reads a bit
		_timeofLastRead[1] = _timeofLastRead[0] + BIAS_DELAY;
		_timeofLastRead[2] = _timeofLastRead[1] + CONF_DELAY;
	};

	bool Begin()  { return begin((max31865_numwires_t)0); }
	int16_t RTD() { return readRTD(); }

	 // Call this every loop to read sensor once per POLL_PERIOD
	 // Also calculates uptime, success rate, and filters fault codes
	void AsyncRead();

	// Draws the temperature at the spesified location.
	// Only draws if the value or page has changed
	void Draw(int x, int y, bool ignoreFault = false);

	 // Last-reported tempreture in degrees Celsius
	const double& Celsius = _temp_c;

	 // Last-reported tempreture in degrees Fahrenheit
	const double& Fahrenheit = _temp_f;
	
	 // The current, filtered fault status (bitfield, see FaultCodes)
	const uint8_t& Fault = _filteredFault;

	 // a rough estimate of the proportion of successfull reads over the last [TIME_CONSTANT] seconds
	const float& SuccessRate = _successRate;

	 // Time in ms that the sensor has been online. Negative values indicate the sensor is offline.
	 // Unlike Fault, UpTime is not filtered and is instead updated immediatley.
	const int& UpTime = _uptime;
};
