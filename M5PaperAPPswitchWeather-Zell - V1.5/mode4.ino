
#include <SD.h>
#include "FS.h"
#include "SPIFFS.h"
#include <SD.h>
#define PIN_SPI_CS 4
#define img_path3 "/dng/wechat.dng"

void mode4()
{
   canvas.deleteCanvas();
  canvas.createCanvas(540, 960);//vertical
  //canvas.createCanvas(960, 540);
  canvas.setTextSize(5);
  canvas.drawString("mode 4!", 70, 400); 
  canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
  StartWiFi(myData.wifiRSSI);
  delay(3000);
  bool file_exist = true;
  if (!SD.begin(PIN_SPI_CS)) {
    Serial.println("SD CARD FAILED, OR NOT PRESENT!");
    //core2 enteres this , why? with TF card present
    //while (1); // don't do anything more:
  }
  else  Serial.println("SD CARD detected!");
  File img_file2 = SD.open("/dng/wechat.dng");
    if (!img_file2) {
        Serial.println("Failed to open SD dng img for reading");
        file_exist=false;
        //return 0;
    }
  Serial.println("#>load:/dng/wechat.dng");
  canvas.drawPngFile(SD,img_path3);
  canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
   Serial.println("#>finished EPD  dng display");
  //canvas.drawPngFile(SD, "/dng/wechat.dng");
   Serial.println("#>test jpg load: https://m5stack.oss-cn-shenzhen.aliyuncs.com/image/example_pic/flower.jpg");
  canvas.drawJpgUrl("https://m5stack.oss-cn-shenzhen.aliyuncs.com/image/example_pic/flower.jpg");  //tested OK, need 10s to display
  canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
   Serial.println("#>finished EPD display");
  delay(10000);
  M5.disableEPDPower();//to save power
  Shutdown();//wake up  every 60 second?
  //Turn_off();
  
}
