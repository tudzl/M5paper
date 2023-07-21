
#include "FS.h"
#include "SPIFFS.h"
//#include "binaryttf.h"
#define img_path "/dng/Alipay2022.dng"

void mode3()
{
  M5.enableEPDPower(); 
  StopWiFi();  
  canvas.deleteCanvas();
  //canvas.createCanvas(540, 960);
  canvas.createCanvas(960, 540);
  canvas.setTextSize(5);
  Serial.println("#>mode 3! display QR_Alipay");
  canvas.drawString("mode 3! QR_Alipay", 70, 400); 
  canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
  delay(2000);
 
  canvas.deleteCanvas();
  canvas.createCanvas(540, 960);
  bool file_exist = true;
  Serial.println("#>load /dng/Alipay2022.dng");
  File img_file = SD.open(img_path);
    if (!img_file) {
        Serial.println("Failed to open file for reading");
        file_exist=false;
        //return 0;
    }
  //fclose (img_file)

   File img_file2 = SD.open("/dng/Alipay2022.dng");
    if (!img_file2) {
        Serial.println("Failed to open SD dng img for reading");
        file_exist=false;
        //return 0;
    }
   //fclose (img_file2)
  canvas.drawPngFile(SD,img_path);
  //canvas.drawPngFile(SD, "/dng/Alipay2022.dng");
  //canvas.drawJpgUrl("https://m5stack.oss-cn-shenzhen.aliyuncs.com/image/example_pic/flower.jpg");
  if(file_exist)
    canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
  else  Serial.println("#>rendering EPD display failed! check file path, SD card!");
  Serial.println("#>finished EPD display");
  delay(6000);
  M5.disableEPDPower();//to save power
  Shutdown();//wake up  every 60 second?
  //Turn_off();
  
}

/*
 * it supports single-plane, 24 bit, uncompressed BMP files
 *  //image from SD card
  //canvas.drawJpgFile(SD, "/bmp/flower2.jpg"); //put image.jpg on the sdcard before uploading sketch
  //canvas.drawBmpFile(SD, "/bmp/weather_icons_20px.bmp", 0, 0);
  canvas.drawBmpFile(SD, "/bmp/shortname.bmp", 0, 0);
 */
