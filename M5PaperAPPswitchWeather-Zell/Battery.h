/*
   Copyright (C) 2021 SFini

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
  * @file Battery.h
  * 
  * Helperfunctions for reading the battery value.
  * 
  * To do:
  * solve the bug for SOC=1% when usb dc in,
  * WeatherDisplay::ShowM5PaperInfo
DateTime: 22.06.2022 22:06:54
Latitude: 30.29
Longitude: 120.16
WifiRSSI: -80
BatteryVolt: 4.25
BatteryCapacity: 1

  */
#pragma once

#include "Data.h"

/**
  * Read the battery voltage
  */
bool GetBatteryValues(MyData &myData)
{  
   //M5.BatteryADCBegin();//need this to init ADC before calling ADC sampling function 
   uint32_t vol = M5.getBatteryVoltage(); // bugs!
   //assert failed: esp_adc_cal_raw_to_voltage esp_adc_cal.c:334 (chars != NULL)
    
   myData.batteryVolt_raw = vol / 1000.0f;
   if (vol < 3300) {
      //vol = 3300; //bugs!
      vol = vol;
   } else if (vol > 4350) {
      vol = 4350;
   }
   float Akku_SOC_raw = (myData.batteryVolt_raw - 3.3) / (float)(4.350 - 3.300);
   float battery = (float)(vol - 3300) / (float)(4350 - 3300);
   if (Akku_SOC_raw > 1) {
      Akku_SOC_raw = 1;
   }
   myData.batteryVolt = vol / 1000.0f;
   
   Serial.println("batteryVolt: " + String(myData.batteryVolt));
   
//   if (battery <= 0.01) {
//      battery = 0.01;
//   }
   if (battery < 0) {
      battery = 0;
   }
   if (battery > 1) {
      battery = 1;
   }
   myData.Akku_SOC= Akku_SOC_raw*100;
   myData.batteryCapacity = (int) (battery * 100);
   Serial.println("batteryCapacity: " + String(myData.batteryCapacity));
   
   return true;
}

//M5.BatteryADCBegin();
//BatteryVolt: 0.28
//BatteryCapacity: 1
//BatterySOC: -287.24
