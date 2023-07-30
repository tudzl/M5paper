//weather app
//wifi issue:  GetWeather failed, error: connection refusedDateTime: 28.07.2023 19:23:34.  
//GetWeather at uri: api.openweathermap.orgl#�?deserializeJson() failed: IncompleteInput 
//V2.4 added saving T max T min tp SPIFFS function
//V2.3C improved GUI
//V2.2D improve GUI, wifi
//V2.2 improve gui
//V2.1 add low battery shutdown
//touch top right area to enter akku mode with Akku_mode_selector() function

#include <M5EPD.h>
#include "FS.h"
#include "SPIFFS.h"
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
#include <SD.h>
#define PIN_SPI_CS_TF 4

// Refresh the M5Paper info more often.
//#define REFRESH_PARTLY 1
#define Refresh_min_interval 5 // sleep-wakeup/update cycle
#define Refresh_Seconds_interval_USB 30
bool USB_Plug_in = false;
int cycle_cnt = 0;
MyData         myData;            // The collection of the global data
WeatherDisplay myDisplay(myData); // The global display helper class
float T_max,T_min;
int H_max,H_min;
void SensorValue_record(float T_cur, int H_cur );
void SensorValue_history_read(void );
void SensorValue_history_check(void);

void mode1()
{ // here you put your mode1 program
Serial.println("#>: Entering Mode 1 APP...");
SensorValue_history_check();
#ifndef REFRESH_PARTLY
  InitEPD(true);
  M5.enableEPDPower();//to save power
  //pinMode(PIN_SPI_CS_TF , OUTPUT); //no effect?
  pinMode(PIN_SPI_CS_TF , INPUT_PULLUP);
  //digitalWrite(PIN_SPI_CS_TF,HIGH); //disable SD card
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
  canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
  delay(600);
  //M5.disableEPDPower();//to save power
  if (StartWiFi(myData.wifiRSSI)) { //wifiRSSI !< The wifi signal strength
    canvas.setTextColor(WHITE, BLACK);
    canvas.drawString("#>:Wifi connected!", 70, 200);
    canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
    Serial.println("#>:Measuring Akku Voltage ...");
    M5.BatteryADCBegin();//need this to init ADC before calling ADC sampling function
    GetBatteryValues(myData);

    if (myData.batteryCapacity = 1) { //need improve
      //when usb plug-in, vbat=3.3, soc=1%? why
      USB_Plug_in = true;
    }

    Serial.println("#>:Collecting sensor data ...");
    GetSHT30Values(myData);
    GetMoonValues(myData);
    Serial.println("#>:Collecting cloud weather data ...");
    if (myData.weather.Get()) {
      Serial.println("#>:cloud weather data collected!");
      SetRTCDateTime(myData);
    }
    M5.enableEPDPower();
    //delay(300);
    myData.Dump();
    delay(50);
    Serial.println("#>main Display.Show now!");
    myDisplay.Show();
    delay(250);
    StopWiFi();
  }
  else {
    canvas.setTextColor(WHITE, BLACK);
    canvas.drawString("#>:Wifi N.A. ,please check!", 70, 200);
    canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);

  }
  //delay(500);
  if (USB_Plug_in) {
    delay(500);
    M5.disableEPDPower();//to save power
    delay(1000 * Refresh_Seconds_interval_USB);
    Serial.println("#>USB_Plug_in mode - update SHT30 data");
    M5.enableEPDPower();
    GetSHT30Values(myData);
    myDisplay.ShowM5Paper_SHT30_Info(); //Draw the M5Paper SHT30 environment and RTC information
    Cycle_show_M5PaperInfo(); // 30S interval to update values
    //update in area (697, 35, 245, 251)
    //Serial.println("#>:esp_deep_sleep_start");
    //esp_deep_sleep_start();
  }
  
  //Serial.println(Refresh_min_interval);
  gpio_hold_dis((gpio_num_t)2); // release the gpio pin to make a proper shutdown possible
  //ShutdownEPD(60 * Refresh_min_interval); // every  5/30 min   wakeup!

  if (myData.Akku_SOC < 40) {
    Serial.printf("#>:Entering low Akku SOC mode, will ShutdownEPD_BAT_low() Now...\r\n");
    ShutdownEPD_BAT_low();
  }
  else{
    Serial.printf("#>:Entering low power mode(akku mode), will wakeup every %d min.Shutdown Now...\r\n", Refresh_min_interval);
    ShutdownEPD_BAT(60 * Refresh_min_interval); // every  5/30 min   wakeup!
  }
    
  delay(200);
  //memset(bar_payload, 0, sizeof(bar_payload));
  //sprintf(bar_payload, "LPM Failed!"); //
  //Serial.println("#>:LPM failed,Update draw Head info");
  //M5.enableEPDPower();
  //myDisplay.UpdateHead(); //bugs here, stuck in push!
  Serial.println("#>:LPM failed,Entering esp_deep_sleep now... ");

  delay(100);
  M5.disableEXTPower();
  delay(100);
  M5.disableEPDPower();
  delay(100);
  esp_sleep_enable_timer_wakeup(60 * Refresh_min_interval * 1000000);
  delay(100);
  ShutdownEPD(60 * Refresh_min_interval);
  //M5.disableMainPower();
  //esp_deep_sleep_start();
  //Cycle_show_M5PaperInfo(); // 30S interval to update values

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
  SensorValue_record(myData.sht30Temp,myData.sht30Humidity);
  myData.nvsCounter++;
  myData.SaveNVS();
  ShutdownEPD(600); //RTC  10 minute  wakeup!

  /*
      Shutdown the M5Paper
      NOTE: the M5Paper could not shutdown while on usb connection.
            In this case use the esp_deep_sleep_start() function.
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
  while (cycle_EN_USB_DC_mode) {
    M5.enableMainPower();
    M5.enableEPDPower();
    M5.BatteryADCBegin();//need this to init ADC before calling ADC sampling function
    GetBatteryValues(myData);//added 21.July.2023
    GetSHT30Values(myData);//added 21.July.2023
    myDisplay.ShowM5Paper_SHT30_Info();
    myData.Dump();
    Serial.println("Update Min and Max Temp.&Hum values!");
    SensorValue_record(myData.sht30Temp,myData.sht30Humidity);
    Serial.printf("#>:--T_max&min: %5.2F C, %5.2F C\r\n",T_max,T_min);
    Serial.printf("#>:--H_max&min: %3d %%, %3d %%\r\n",H_max,H_min);
    myDisplay.Show_SHT_Min_Max_Info();
    myDisplay.UpdateHead();//added v2.3B
    delay(100);
    cycle_cnt++;
    if (100 ==myData.Akku_SOC) {

      if (4.35 == myData.batteryVolt) //if(myData.batteryVolt==3.3)
        cycle_EN_USB_DC_mode = true;
      Serial.println("Fast cycle mode running when USB powered!");
    }
    if (myData.Akku_SOC < 75) {
      cycle_EN_USB_DC_mode = false;
      Serial.println("#>: Low Akku (<75%), fast cycle mode exit now...");
    }

    Serial.println("#>:Checking Akku mode touch input...");
    //delay(1000*30); // improve
    M5.disableEPDPower();
    for (int i = 0; i < Refresh_Seconds_interval_USB/10; i++) {
      if (Akku_mode_selector()) {
        M5.enableEPDPower();
        myDisplay.DrawAkkuMode_Info();
        cycle_EN_USB_DC_mode = false;
        i = Refresh_Seconds_interval_USB;
        Serial.println("#>:Activate Akku mode, fast cycle mode exit now...");
      }
      delay(100);

    }
    M5.disableEPDPower();
    //Serial.printf("#>:Battery Vol: %3.2f V\r\n",myData.batteryVolt);
    Serial.printf("#>:Battery Raw: %3.2f V\r\n", myData.batteryVolt_raw);
    Serial.printf("APP running cycle cnt:%d\r\n", cycle_cnt);

  }

}

void SensorValue_record(float T_cur, int H_cur ){
     //update min max values
    if (1==cycle_cnt){
      H_min = H_cur;
      if (0 == T_min)
          T_min = T_cur;
      //T_min = T_cur;
    }
    if (0==cycle_cnt){
      SensorValue_history_read ();
      if (0 == T_min){
        T_min = T_cur;
        Serial.printf("#>:0 cycle--updated SPIFFS--T_max&min: %5.2F C, %5.2F C\r\n",T_max,T_min);
      }
    }

    if (T_cur > T_max)
      T_max = T_cur;
    if (T_cur < T_min)
      T_min = T_cur;

    if (H_cur > H_max)
      H_max = H_cur;
    if (H_cur < H_min)
      H_min = H_cur;
    myData.sht30T_max=  T_max;
    myData.sht30T_min=  T_min;
    myData.sht30H_max=  H_max;
    myData.sht30H_min=  H_min;

    if( (0==cycle_cnt%6)&&(cycle_cnt>5)){
    //save to SPIFFS
        if(!SPIFFS.begin())
        {
          Serial.println("SPIFFS Mount Failed -> skip T max min saving");
          //SPIFFS.format();
          //SPIFFS.begin();  
          return;
        }  
      
       File file_Max = SPIFFS.open("/TMax.txt","w");
      //Modus = file3.parseInt(); //get the value from Storage as an integer
    //   File file3 = SPIFFS.open("/Modus.txt","w");
    //
          if (!file_Max) 
        {
          Serial.println("#>:Error opening file TMax.txt for writing!");
          return;
        }
          if (file_Max.print(myData.sht30T_max))
        {
          Serial.println("- file written TMax");
        } else {
          Serial.println("- TMax write failed");
        }
        file_Max.close(); 
        if(0!=T_min){
           File file_Min = SPIFFS.open("/TMin.txt","w");
      //Modus = file3.parseInt(); //get the value from Storage as an integer
    //   File file3 = SPIFFS.open("/Modus.txt","w");
    //parseFloat
           if (!file_Min) 
           {
          Serial.println("#>:Error opening file TMin.txt for writing!");
          return;
           }
          if (file_Min.print(myData.sht30T_min))
            {
          Serial.println("- file written TMin");
            } else {
          Serial.println("- TMin write failed");
           }
          file_Min.close();   
        //finished saving
        //readback, debug only
         }
        SensorValue_history_check();
    }



}


void SensorValue_history_read(void){
    float T_max_tmp,T_min_tmp;
    //read from SPIFFS
    if(!SPIFFS.begin())
    {
      Serial.println("SPIFFS Mount Failed -> skip T max min saving");
      //SPIFFS.format();
      //SPIFFS.begin();  
      return;
    }  
  Serial.println("#>:reading recorded T max min values...");
  File file_Max = SPIFFS.open("/TMax.txt","r");
  //Modus = file3.parseInt(); //get the value from Storage as an integer
 //   File file3 = SPIFFS.open("/Modus.txt","w");
 //parseFloat
      if (!file_Max) 
    {
      Serial.println("#>:Error opening file TMax.txt for reading!");
      return;
    }
    T_max_tmp= file_Max.parseFloat(); 

    file_Max.close(); 
  File file_Min = SPIFFS.open("/TMin.txt","w");
  //Modus = file3.parseInt(); //get the value from Storage as an integer
 //   File file3 = SPIFFS.open("/Modus.txt","w");
 //parseFloat
      if (!file_Min) 
    {
      Serial.println("#>:Error opening file TMin.txt for reading!");
      return;
    }
    T_min_tmp= file_Min.parseFloat(); 
    file_Min.close();   
    //myData.sht30T_max=  T_max;
    //myData.sht30T_min=  T_min;
    Serial.printf("#>:Read SPIFFS--T_max&min: %5.2F C, %5.2F C\r\n",T_max_tmp,T_min);

    T_min=T_min_tmp;
    T_max=T_max_tmp;

  
}


void SensorValue_history_check(void){
    float T_max_tmp,T_min_tmp;
    //read from SPIFFS
    if(!SPIFFS.begin())
    {
      Serial.println("SPIFFS Mount Failed -> skip T max min saving");
      //SPIFFS.format();
      //SPIFFS.begin();  
      return;
    }  
  Serial.println("#>:check reading recorded T max min values...");
  File file_Max = SPIFFS.open("/TMax.txt","r");
  //Modus = file3.parseInt(); //get the value from Storage as an integer
 //   File file3 = SPIFFS.open("/Modus.txt","w");
 //parseFloat
      if (!file_Max) 
    {
      Serial.println("#>:Error opening file TMax.txt for reading!");
      return;
    }
    T_max_tmp= file_Max.parseFloat(); 

    file_Max.close(); 
  File file_Min = SPIFFS.open("/TMin.txt","w");
  //Modus = file3.parseInt(); //get the value from Storage as an integer
 //   File file3 = SPIFFS.open("/Modus.txt","w");
 //parseFloat
      if (!file_Min) 
    {
      Serial.println("#>:Error opening file TMin.txt for reading!");
      return;
    }
    T_min_tmp= file_Min.parseFloat(); 
    file_Min.close();   
    //myData.sht30T_max=  T_max;
    //myData.sht30T_min=  T_min;
    Serial.printf("#>:Check SPIFFS--T_max&min: %5.2F C, %5.2F C\r\n",T_max_tmp,T_min);

  
}
