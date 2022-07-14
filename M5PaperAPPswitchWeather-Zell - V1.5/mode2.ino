 void mode2()
{
//

//  
  canvas.deleteCanvas();
  canvas.createCanvas(540, 960);
  canvas.setTextSize(5);
  canvas.drawString("mode 2! RTC Time APP", 70, 400); 
  canvas.pushCanvas(0,0,UPDATE_MODE_GC16);

 if (new_mode == true)
 {
      set_time();
 }
  MinRefresh ();
  Shutdown();//wake up  every 60 second?
  
//  delay(10000);
//  Turn_off();
//  esp_deep_sleep_start();
  
}
