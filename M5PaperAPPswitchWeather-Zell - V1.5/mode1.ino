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
#define Refresh_Seconds_interval_USB 30
bool USB_Plug_in = false;
int cycle_cnt = 0;
MyData         myData;            // The collection of the global data
WeatherDisplay myDisplay(myData); // The global display helper class

void mode1()
{ // here you put your mode1 program

   #ifndef REFRESH_PARTLY
   InitEPD(true);
   M5.enableEPDPower();//to save power
    canvas.deleteCanvas();
    //canvas.createCanvas(540, 960);
    canvas.createCanvas(960, 540);
    canvas.setTextSize(5);
    canvas.setTextColor(0x09); //TFT_eSPI::setTextColor(fgcolor, bgcolor)
    canvas.drawString("mode 1!", 70, 400); 
    canvas.setTextColor(WHITE, BLACK);
    canvas.drawString("Weather station!", 70, 300); 
    canvas.setTextSize(4);
    canvas.setTextColor(0x06); 
    canvas.drawString("Conneting Wifi now...", 70, 100); 
    canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
    delay(600);
    M5.disableEPDPower();//to save power
   if (StartWiFi(myData.wifiRSSI)) { //wifiRSSI !< The wifi signal strength
      Serial.println("#>:Collecting sensor data...");   
      M5.BatteryADCBegin();//need this to init ADC before calling ADC sampling function 
      GetBatteryValues(myData);
      
      if(myData.batteryCapacity = 1){//need improve
         //when usb plug-in, vbat=3.3, soc=1%? why
         USB_Plug_in = true;
      }
     
     
      GetSHT30Values(myData);
      GetMoonValues(myData);
      if (myData.weather.Get()) {
         SetRTCDateTime(myData);
      }
      M5.enableEPDPower();   
      delay(300);
      myData.Dump();
      delay(150);
      Serial.println("#>main Display.Show!");
      myDisplay.Show();
      delay(150);
      StopWiFi();
   }
   //delay(500);
   if(USB_Plug_in){
    delay(500);
    M5.disableEPDPower();//to save power
    delay(1000*Refresh_Seconds_interval_USB);
    Serial.println("#>USB_Plug_in mode - update SHT30 data");
    M5.enableEPDPower();
    GetSHT30Values(myData);
    myDisplay.ShowM5Paper_SHT30_Info();
    Cycle_show_M5PaperInfo(); // 30S interval to update values
    //update in area (697, 35, 245, 251)
    //Serial.println("#>:esp_deep_sleep_start");
    //esp_deep_sleep_start();
   }
   Serial.printf("#>:Entering low power mode, will wakeup every %d min.Shutdown Now...\r\n",Refresh_min_interval);
   //Serial.println(Refresh_min_interval);
   gpio_hold_dis((gpio_num_t)2); // release the gpio pin to make a proper shutdown possible
   //ShutdownEPD(60 * Refresh_min_interval); // every  5/30 min   wakeup!
   ShutdownEPD_BAT(60 * Refresh_min_interval); // every  5/30 min   wakeup!
   delay(1000);
   //memset(bar_payload, 0, sizeof(bar_payload));
   //sprintf(bar_payload, "LPM Failed!"); //
   Serial.println("#>:LPM failed,Update draw Head info");
   //M5.enableEPDPower();
   //myDisplay.UpdateHead();
   Serial.println("#>:LPM failed,Entering esp_deep_sleep now... ");
   
   delay(100);
   M5.disableEXTPower();
   delay(200);
   M5.disableEPDPower();
   M5.disableMainPower();
   delay(100);
   esp_sleep_enable_timer_wakeup(60*Refresh_min_interval * 1000000);
   delay(100);
   esp_deep_sleep_start();
   Cycle_show_M5PaperInfo(); // 30S interval to update values
   
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


void  Cycle_show_M5PaperInfo()
{
  bool cycle_EN_USB_DC_mode = true;
  //M5.enableEPDPower();
  while(cycle_EN_USB_DC_mode){
    M5.enableEPDPower();
    myDisplay.ShowM5Paper_SHT30_Info();
    myData.Dump();
    cycle_cnt++;
    if(myData.batteryCapacity==1){

        if(myData.batteryVolt=3.3)
        cycle_EN_USB_DC_mode = true;
        Serial.println("Fast cycle mode running when USB powered!");
    }
    if(myData.Akku_SOC<75){
           cycle_EN_USB_DC_mode = false;
           Serial.println("#>: Low Akku (<75%), fast cycle mode exit now...");
    }

    Serial.println("#>:Checking Akku mode touch input...");
    //delay(1000*30); // improve
    //M5.disableEPDPower();
    for (int i=0;i<Refresh_Seconds_interval_USB;i++){
      if(Akku_mode_selector()){
        myDisplay.DrawAkkuMode_Info();
        cycle_EN_USB_DC_mode =false;
        i=Refresh_Seconds_interval_USB;
        Serial.println("#>:Activate Akku mode, fast cycle mode exit now...");
      }
      delay(100);
      
    }
    
    //Serial.printf("#>:Battery Vol: %3.2f V\r\n",myData.batteryVolt);
    Serial.printf("#>:Battery Raw: %3.2f V\r\n",myData.batteryVolt_raw);
    Serial.printf("APP running cycle cnt:%d\r\n",cycle_cnt);
    //Serial.println("#>:Checking Akku mode touch input...");
//    if(Akku_mode_selector()){
//      myDisplay.DrawAkkuMode_Info();
//      cycle_EN_USB_DC_mode =false;
     //Serial.println("#>:Activate Akku mode, fast cycle mode exit now...");
     //break;
//    }
  }
   
}
