/*
//Sketch uses 1098313 bytes (16%) of program storage space. Maximum is 6553600 bytes.
Sketch uses 1108257 bytes (16%) of program storage space. Maximum is 6553600 bytes.

Global variables use 42096 bytes (0%) of dynamic memory, leaving 4479888 bytes for local variables. Maximum is 4521984 bytes.

*/
#include <M5EPD.h> 
#include "SPIFFS.h"
#define FORMAT_SPIFFS_IF_FAILED true
#include <esp_wifi.h>
#include <esp_bt.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#include "configuration.h" 

#include "credentials.h"

M5EPD_Canvas canvas(&M5.EPD); // setup for the Display


// to choose which mode to use
int Modus;
bool new_mode= false;
int a=0;

// for the Touchscreen
int point[2][2];
bool app_launch_sellect = false;


// For the Clock
rtc_time_t RTCtime;
rtc_date_t RTCDate;
char timeStrbuff[64];

// for NTP to RTC time
time_t t;
struct tm *tm;
char *version_info="V2.0, 06.Juli.2022, Author: Zell";

void setup() {
  M5.begin(true,false,true,false,true); //Touchscreen,SDreader, Serial,BatteryADC,I2C
  Serial.println("M5paper App Launch program Angeschaltet");
  Serial.printf("FW version:%s\r\n",version_info);
  M5.EPD.SetRotation(90); // for the Screen to be upright
  //M5.EPD.Clear(true);  //if you want a Full refresh after every start
  M5.update();//added by Zell

  if( M5.BtnL.wasPressed()){
    app_launch_sellect = true;
    Serial.println("BtnL > launch APP sellect");
  }
    if( M5.BtnR.wasPressed()){
    app_launch_sellect = true;
    Serial.println("BtnR > launch APP sellect");
  }
  if( M5.BtnP.isPressed()){
    app_launch_sellect = true;
    Serial.println("BtnP launch APP sellect");
  }
  wifiMulti.addAP(ssid, password);
  //wifiMulti.addAP(ssid1, password1);
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
    {
      Serial.println("SPIFFS Mount Failed -> Formatting SPIFFS");
      SPIFFS.format();
      SPIFFS.begin();  
      return;
    }  
  
  File file3 = SPIFFS.open("/Modus.txt","r");
  Modus = file3.parseInt(); //get the value from Storage as an integer
  file3.close();

}

void loop() {
  
if( M5.BtnP.isPressed())// if button P (Power) was pressed to boot the device manualy, the modus choosing mode is activated
    {//seems not entered!  
      M5.update();
      selector();
    }  
if( app_launch_sellect){
    M5.update();
    Serial.println("#>:Entering APP sellection function");
    selector();
    }
if (new_mode == true)
    {
    FullCleanDisplay();
    File file3 = SPIFFS.open("/Modus.txt","w");
      if (!file3) 
    {
      Serial.println("Error opening file Modus for writing");
      return;
    }
      if (file3.print(Modus))
    {
      Serial.println("- file written Modus");
    } else {
      Serial.println("- write failed");
    }
    file3.close(); 
}


if (Modus == 0)
  {
    Serial.println("switching to Selector");
    selector();   
  }
    else if(Modus == 1 && Modus <= Number_of_modes)
    {
      Serial.println("switching to Mode 1");
      mode1();
    }
    else if(Modus == 2 && Modus <= Number_of_modes)
    {
      Serial.println("switching to Mode 2");
      mode2();
    }
    else if(Modus == 3 && Modus <= Number_of_modes)
    {
      Serial.println("switching to Mode 3");
      mode3();
    }
    else if(Modus == 4 && Modus <= Number_of_modes)
    {
      Serial.println("switching to Mode 4");
      mode4();
    }
    else if(Modus == 5 && Modus <= Number_of_modes)
    {
      Serial.println("switching to Mode 5");
      mode5();
    }
    else if(Modus == 6 && Modus <= Number_of_modes)
    {
      Serial.println("switching to Mode 6");
      mode6();
    }
    else if(Modus == 7 && Modus <= Number_of_modes)
    {
      Serial.println("switching to Mode 7");
      mode7();
    }
        else
        {
          Serial.println("mode not known going to selector");
          selector();
        } 


}
