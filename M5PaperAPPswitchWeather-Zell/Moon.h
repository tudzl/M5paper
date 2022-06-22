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
  * @file Moon.h
  * https://github.com/signetica/MoonRise
  * Library / C++ class for calculating moon rise/set events for Unix, Linux, Arduino.
  * Helper function to get the moon rise and set data.
  */
#pragma once
#include <MoonRise.h>

/* Calculate the moon rise and set with the MoonRise library */
bool GetMoonValues(MyData &myData)
{
   MoonRise mr;
   time_t   time = GetRTCTime();

   mr.calculate(LATITUDE, LONGITUDE, time);

   myData.moonRise = mr.riseTime;
   myData.moonSet =  mr.setTime;
   return true;
}
