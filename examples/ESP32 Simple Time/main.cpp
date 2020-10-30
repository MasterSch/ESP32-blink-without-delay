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

uint32_t sntp_update_delay_MS_rfc_not_less_than_15000 ()
{
   //info_sntp_update_delay_MS_rfc_not_less_than_15000_has_been_called = true;
    return 60000; // 15s
}


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




void setup()
{
  Serial.begin(115200);
  syncTime();
  printLocalTime();

}

void loop()
{
  currentMillis = millis();
  if (currentMillis == intervallMillis) syncTime();
  if ((currentMillis - previousMillis) >= interval) 
  {
    previousMillis += interval;
    printLocalTime();
  }
}
