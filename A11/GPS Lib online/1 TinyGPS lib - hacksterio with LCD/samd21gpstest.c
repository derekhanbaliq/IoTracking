/*
	Develop by:		Vicente Arturo Zavala Ortiz
	Date:			March 21 2017 1:15:35 PM
	Description:	GPS Test
*/

#include "TFT.h"
#include "TinyGPS.h"

#define TFT_DISPLAY

#define GPSBaud								9600
#define SerialMonitor						SerialUSB
#define gpsPort								Serial

// The TinyGPS object
TinyGPS gps;

void printDateTime(TinyGPSDate &d, TinyGPSTime &t);
void printStr(const char *str, int len);
void printInt(unsigned long val, bool valid, int len);
void printFloat(float val, bool valid, int len, int prec);
bool readGPS(unsigned long ms);

void setup()
{
	// add setup code here
	#ifdef TFT_DISPLAY
		Tft.init();
	#else
		SerialMonitor.begin(GPSBaud);
	#endif

	delay(100);

	gpsPort.begin(GPSBaud);
}

void loop()
{
	// add main program code here 
	readGPS(5000);
	
	#ifdef TFT_DISPLAY
		Tft.set_font_size(3);
		printDateTime(gps.date, gps.time);
		Tft._putchar('\n');
		Tft._puts("LAT: ");
		printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
		Tft._putchar('\n');
		Tft._puts("LNG: ");
		printFloat(gps.location.lng(), gps.location.isValid(), 11, 6);
		Tft._putchar('\n');		
	#else
		printDateTime(gps.date, gps.time);
		SerialMonitor.println();
		SerialMonitor.print("LAT: ");
		printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
		SerialMonitor.print(" LNG: ");
		printFloat(gps.location.lng(), gps.location.isValid(), 11, 6);
		SerialMonitor.println();		
	#endif
 	
	delay(5000);
		
	Tft.clrscr();
	
	if(millis() > 5000 && gps.charsProcessed() < 10)
	{
		#ifdef TFT_DISPLAY
			Tft._puts("No GPS data received: check wiring\n");
		#else
			SerialMonitor.println("No GPS data received: check wiring");
		#endif		
	}
}

bool readGPS(unsigned long ms)
{
	for(unsigned long start = millis(); millis() - start < ms;)
	{
		if(gpsPort.available()) 
		{
			if(gps.encode(gpsPort.read())) 
			{
				if(gps.location.isValid()) {
					return true;
				}
			}
		}
	}
	
	return false;
}

static void printFloat(float val, bool valid, int len, int prec)
{
	if(valid)
	{
		#ifdef TFT_DISPLAY
			Tft.print_float(val);
		#else
			SerialMonitor.print(val, prec);
		#endif
		
		int vi		= abs((int) val);
		int flen	= prec + (val < 0.0 ? 2 : 1);
		
		flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
		
		for(int i=flen; i<len; ++i) {
			#ifdef TFT_DISPLAY
				Tft._putchar(' ');
			#else
				SerialMonitor.print(' ');
			#endif
		}
	}

	readGPS(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
	char sz[32] = "*****************";
	
	if(valid)
		sprintf(sz, "%ld", val);
	
	sz[len] = 0;
	
	for(int i=strlen(sz); i<len; ++i)
		sz[i] = ' ';
	
	if(len > 0)
		sz[len - 1] = ' ';
	
	#ifdef TFT_DISPLAY
		Tft._puts(sz);
	#else
		SerialMonitor.print(sz);
	#endif
	
	readGPS(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
	if(d.isValid()) 
	{
		char sz[32];

		sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
		
		#ifdef TFT_DISPLAY
			Tft._puts(sz);
			Tft._putchar('\n');
		#else
			SerialMonitor.print(sz);
		#endif
	}
	
	if(t.isValid()) 
	{
		char sz[32];
		
		sprintf(sz, "%02d:%02d:%02d ", (t.hour() + gps.timeZone()), t.minute(), t.second());
		
		#ifdef TFT_DISPLAY
			Tft._puts(sz);
			Tft._putchar('\n');
		#else
			SerialMonitor.print(sz);
		#endif
	}

	//printInt(d.age(), d.isValid(), 5);
	readGPS(0);
}

static void printStr(const char *str, int len)
{
	int slen = strlen(str);
	
	for(int i=0; i<len; ++i) 
	{
		#ifdef TFT_DISPLAY
			Tft._putchar(i < slen ? str[i] : ' ');
		#else
			SerialMonitor.print(i < slen ? str[i] : ' ');
		#endif
	}
	
	readGPS(0);
}