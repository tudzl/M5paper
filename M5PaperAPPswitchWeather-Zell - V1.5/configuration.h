#pragma once

// for the mode selector
const int Number_of_modes = 6; //between 1 and 7 modes are possible

// here you can name every mode -> the name that is displayed on the display
// just leave the string empty if you have nothing to put there
// max Lenght of the String is 20 Symbols
const String Mode_0 = "WeatherSTA";
const String Mode_1 = "RTC_NTP time";//mode2
const String Mode_2 = "QR_Alipay";
const String Mode_3 = "QR_Wechat";
const String Mode_4 = "mode5";
const String Mode_5 = "mode6";
const String Mode_6 = "mode7";

// for the clock example to work properly
const int UTC_OFFSET = 60; // the amount of minutes + or - from GMT
const int UTC_OFFSET_HZ = 60*8;
// very useful
#define uS_TO_S_FACTOR 1000000
#define uS_TO_MS_FACTOR 1000
