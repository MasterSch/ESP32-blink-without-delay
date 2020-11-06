// OPTIONAL: change SNTP startup delay
// a weak function is already defined and returns 0 (RFC violation)
// it can be redefined:
//uint32_t sntp_startup_delay_MS_rfc_not_less_than_60000 ()
//{
//    //info_sntp_startup_delay_MS_rfc_not_less_than_60000_has_been_called = true;
//    return 60000; // 60s (or lwIP's original default: (random() % 5000))
//}

// OPTIONAL: change SNTP update delay
// a weak function is already defined and returns 1 hour
// it can be redefined:
//uint32_t sntp_update_delay_MS_rfc_not_less_than_15000 ()
//{
//    //info_sntp_update_delay_MS_rfc_not_less_than_15000_has_been_called = true;
//    return 15000; // 15s
//}


#include <arduino.h>
#include <WiFi.h>
#include "time.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

#define TFT_GREY 0xBDF7

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg=0, mdeg=0, hdeg=0;
uint16_t osx=64, osy=64, omx=64, omy=64, ohx=64, ohy=64;  // Saved H, M, S x & y coords
uint16_t x0=0, x1=0, yy0=0, yy1=0, xx=67, yy=90;
uint32_t targetTime = 0;                    // for next 1 second timeout


const char* ssid       = "zuhause";
const char* password   = "schmitt.obw@web.de";

const char* ntpServer1 = "fritz.box";
//const char* ntpServer2 = "1.de.pool.ntp.org";
//const char* ntpServer3 = "2.de.pool.ntp.org";

const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const char *Monat[12] = {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};
const char *Wochentag[7] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};

unsigned long previousMillis=0, currentMillis=0, intervallMillis = 100 * 60 *60;
int interval = 1000;

struct tm timeinfo;

uint8_t hh, mm, ss;

boolean initial = 1;


void printLocalTime()
{
  //struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  //Serial.println(&timeinfo, "%A, %d. %B %Y %H:%M:%S Uhr  ");
  Serial.print(Wochentag[timeinfo.tm_wday]);
  Serial.print(", der ");
  Serial.print(&timeinfo, "%d. ");
  Serial.print(Monat[timeinfo.tm_mon]);
  Serial.println(&timeinfo, " %Y  Zeit: %H:%M:%S Uhr  ");

}

void syncTime()
  {
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(50);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  configTzTime("CET-1CEST,M3.5.0/02,M10.5.0/03", ntpServer1);
  //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //printLocalTime();
  getLocalTime(&timeinfo);
  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  currentMillis = millis();
  while ((currentMillis - previousMillis) >= interval) 
    {
      previousMillis += interval;
    }
  }

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h


  void drawClock() 
  {
  // Draw clock face
  tft.fillCircle(xx, yy, 61, TFT_BLUE);
  //delay(2000);
  tft.fillCircle(xx, yy, 57, TFT_BLACK);
  //delay(2000);
 
  // Draw 12 lines   5 minuten striche
  for(int i = 0; i<360; i+= 30) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx*57+xx;
    yy0 = sy*57+yy;
    x1 = sx*50+xx;
    yy1 = sy*50+yy;
    tft.drawLine(x0, yy0, x1, yy1, TFT_BLUE);
  }
    //delay(2000);

  // Draw 60 dots   sekunden
  for(int i = 0; i<360; i+= 6) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx*53+xx;
    yy0 = sy*53+yy;
    tft.drawPixel(x0, yy0, TFT_BLUE);
    //  15 min punkte
    if(i==0 || i==180) {
     tft.fillCircle(x0, yy0, 1, TFT_CYAN);
     tft.fillCircle(x0+1, yy0, 1, TFT_CYAN);
     }
    if(i==90 || i==270) {
     tft.fillCircle(x0, yy0, 1, TFT_CYAN);
     tft.fillCircle(x0+1, yy0, 1, TFT_CYAN); 
     }
  }


  tft.fillCircle(xx+1, yy+1, 3, TFT_RED);  //Mittelpunkt

  // Draw text at position 64,125 using fonts 4
  // Only font numbers 2,4,6,7 are valid. Font 6 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : . a p m
  // Font 7 is a 7 segment font and only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : .
  tft.drawCentreString("Time flies",64,180,4);
  }




void setup()
{
  tft.init();
  tft.setRotation(0);
  //tft.fillScreen(TFT_GREY);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);  // Adding a black background colour erases previous text automatically  

  delay(5000);

  Serial.begin(115200);

  /*
  int i = 90;
  do {
  Serial.print(i);
  Serial.print("  ");
  Serial.print(sin(i*PI/180) * 33);
  Serial.print("  ");
  Serial.print(sin(i*PI/180) * 44);
  Serial.print("  ");
  Serial.println(sin(i*PI/180) * 47);
  i = i - 3;
  } while (i >= 0);
  */
    
  syncTime();
  printLocalTime();

}

void loop()
{
  currentMillis = millis();
  if (currentMillis >= intervallMillis) syncTime();
  if ((currentMillis - previousMillis) >= interval) 
  {
    previousMillis += interval;
    //printLocalTime();
  
    if (initial) drawClock();
 
    getLocalTime(&timeinfo);
 
    ss = timeinfo.tm_sec;
    mm = timeinfo.tm_min;
    hh = timeinfo.tm_hour;         

    /*
    Serial.print(hh);
    Serial.print("  ");
    Serial.print(mm);
    Serial.print("  ");
    Serial.print(ss);
    Serial.println("  ");
    */

    // Pre-compute hand degrees, x & y coords for a fast screen update
    sdeg = ss*6;                  // 0-59 -> 0-354
    mdeg = mm*6+sdeg*0.01666667;  // 0-59 -> 0-360 - includes seconds
    hdeg = hh*30+mdeg*0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds
    hx = cos((hdeg-90)*0.0174532925);    
    hy = sin((hdeg-90)*0.0174532925);
    mx = cos((mdeg-90)*0.0174532925);    
    my = sin((mdeg-90)*0.0174532925);
    sx = cos((sdeg-90)*0.0174532925);    
    sy = sin((sdeg-90)*0.0174532925);

    /*
    Serial.print("Sekunde:  ");
    Serial.print(ss);
      Serial.print("  sx: ");
    Serial.print(sx);
        Serial.print("  sy: ");
    Serial.print(sy);
    */

    if (ss==0 || initial) {
      initial = 0;
      // Erase hour and minute hand positions every minute
      tft.drawLine(ohx, ohy, xx, yy, TFT_BLACK);
      ohx = hx*33+xx;    
      ohy = hy*33+yy;
      tft.drawLine(omx, omy, xx, yy, TFT_BLACK);
      omx = mx*44+xx;    
      omy = my*44+yy;
    }

      // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
      tft.drawLine(osx, osy, xx, yy, TFT_BLACK);
      tft.drawLine(ohx, ohy, xx, yy, TFT_WHITE);
      tft.drawLine(omx, omy, xx, yy, TFT_WHITE);
      osx = sx*47+xx;    
      osy = sy*47+yy;
      tft.drawLine(osx, osy, xx, yy, TFT_RED);
   
      /*
      Serial.print("  osx: ");
      Serial.print(osx);
      Serial.print("  osy: ");
      Serial.println(osy);
      */

    tft.fillCircle(xx, yy, 3, TFT_RED);
  
  }
}
