//weather app
#include <M5EPD.h>
#include "Config.h"
//#include "ConfigOverride.h" // Remove this line
#include "Data.h"
#include "Display.h"
#include "Battery.h"
#include "EPD.h"
#include "EPDWifi.h"
#include "Moon.h"
#include "SHT30.h"
#include "Time.h"
#include "Utils.h"
#include "Weather.h"

// Refresh the M5Paper info more often.
//#define REFRESH_PARTLY 1
#define Refresh_min_interval 5 // sleep-wakeup/update cycle
bool USB_Plug_in = false;

MyData         myData;            // The collection of the global data
WeatherDisplay myDisplay(myData); // The global display helper class

void mode1()
{ // here you put your mode1 program

   #ifndef REFRESH_PARTLY
   InitEPD(true);
    canvas.deleteCanvas();
    //canvas.createCanvas(540, 960);
    canvas.createCanvas(960, 540);
    canvas.setTextSize(5);
    canvas.drawString("mode 1!", 70, 400); 
    canvas.drawString("Weather station!", 70, 300); 
    canvas.setTextSize(4);
    canvas.drawString("Conneting Wifi now...", 70, 100); 
    canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
   
   if (StartWiFi(myData.wifiRSSI)) { //wifiRSSI !< The wifi signal strength
      Serial.println("#>:Collecting sensor data...");   
      M5.BatteryADCBegin();//need this to init ADC before calling ADC sampling function 
      GetBatteryValues(myData);
      
      if(myData.batteryCapacity = 1){
         //when usb plug-in, vbat=3.3, soc=1%? why
         USB_Plug_in = true;
      }
     
         
      GetSHT30Values(myData);
      GetMoonValues(myData);
      if (myData.weather.Get()) {
         SetRTCDateTime(myData);
      }
      myData.Dump();
      myDisplay.Show();
      StopWiFi();
   }
   delay(1000);
   if(USB_Plug_in){
    delay(1000*Refresh_min_interval);
    Serial.println("#>- update SHT30 data");
    GetSHT30Values(myData);
    myDisplay.ShowM5Paper_SHT30_Info();
    //update in area (697, 35, 245, 251)
    Serial.println("#>:esp_deep_sleep_start");
    esp_deep_sleep_start();
   }
   Serial.print("#>: Shutdown Now... will wakeup every ? min:");
   Serial.println(Refresh_min_interval);
   ShutdownEPD(60 * Refresh_min_interval); // every  5 min   wakeup!
   //ShutdownEPD(60 * 60); // every 1 hour   wakeup!
#else 
   myData.LoadNVS();
   if (myData.nvsCounter == 1) {
      InitEPD(true);
      if (StartWiFi(myData.wifiRSSI)) {
         M5.BatteryADCBegin();//need this to init ADC before calling ADC sampling function 
         GetBatteryValues(myData);
         GetSHT30Values(myData);
         GetMoonValues(myData);
         if (myData.weather.Get()) {
            SetRTCDateTime(myData);
         }
         myData.Dump();
         myDisplay.Show();
         StopWiFi();
      }
   } else {
      InitEPD(false);
      GetSHT30Values(myData);
      myDisplay.ShowM5PaperInfo();
      if (myData.nvsCounter >= 60) {
         myData.nvsCounter = 0;
      }
   }
   myData.nvsCounter++;
   myData.SaveNVS();
   ShutdownEPD(600); //RTC  10 minute  wakeup!

/* 
 *  Shutdown the M5Paper 
 *  NOTE: the M5Paper could not shutdown while on usb connection.
 *        In this case use the esp_deep_sleep_start() function.
*/
#endif // REFRESH_PARTLY   

  
//  canvas.deleteCanvas();
//  canvas.createCanvas(540, 960);
//  canvas.setTextSize(5);
//  canvas.drawString("mode 1!", 70, 400); 
//  canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
//  
//  delay(10000);
//  Turn_off();

}
