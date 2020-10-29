#include <arduino.h>
#include <WiFi.h>
#include "time.h"

const char* ssid       = "zuhause";
const char* password   = "schmitt.obw@web.de";

const char* ntpServer1 = "0.de.pool.ntp.org";
const char* ntpServer2 = "1.de.pool.ntp.org";
const char* ntpServer3 = "2.de.pool.ntp.org";

const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

const char *Monat[12] = {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};
const char *Wochentag[7] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};



void printLocalTime()
{
  struct tm timeinfo;
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

void setup()
{
  Serial.begin(115200);
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  configTzTime("CET-1CEST,M3.5.0/02,M10.5.0/03", ntpServer1, ntpServer2, ntpServer3);
  //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop()
{
  delay(1000);
  printLocalTime();
}
