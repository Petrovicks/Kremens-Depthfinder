Simple Arduino-based NMEA datalogger.

NMEA sentence documentation: http://aprs.gids.nl/nmea/

NOTE: Uses custom SD card reader library: https://github.com/adafruit/SD

Hardware Notes:
- Arduino Mega 2560
- seedstudios SD card reader shield
- Both peripherals continually transmit at 1Hz. 
- Garmin GPS
	- https://static.garmincdn.com/pumac/GPS16x_TechnicalSpecifications.pdf
	- WIRING
		- RED: Vin (8-40 Vdc, 12V ideal)
			- ~780mW typical consumption
		- BLK: Ground
		- BLU: RS232 input
		- WHT: RS232 output
	- Utilizes traditional RS232, requires hardware inversion and level shifter to utilize the ATMega2560 UART.
	- Configured to automatically transmit all NMEA GPS minimum messages, GPRMC used for this project. 
- Northstar D210 Explorer (depthfinder)
	- http://www.northstarnav.com/upload/Northstar/Documents/D210.pdf
	WIRING
		- RED: Vin (12Vdc, sensitive to noise)
		- BLK: Ground
		- ORAN: 12V, 100mA buzzer
		- BWN: 5V RS232 signal
