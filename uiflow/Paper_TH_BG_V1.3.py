from m5stack import *
from m5ui import *
from uiflow import *
from libs.label_plus import *
import time
#V1.2  15.july.2023 improved GUI, partial show
#V1.2  15.july.2023 improved GUI, added icons

#V1.1  11.july.2023
#Author: Zell, tudzl@hotmail.de

@timerSch.event('timer3')
def callback_timer3():
  # global params
  global temperature,Hum,Akku_V
  power.epd_power_on()
  print(str('#>:TMR3 ISR') )
  wait_ms(20)
  lcd.partial_show(90, 800, 120, 60)
  lcd.partial_show(370, 800, 120, 60)
  lcd.partial_show(440, 930, 100, 30)
  #lcd.show()
  wait_ms(400)
  print(str('#>:show E-Ink contents2') )
  wait_ms(20)
  #power.epd_power_off()
  pass


setScreenColor(15)
power.ext_power_off()

Akku_V = None
temperature = None
Hum = None
#timerSch.stop('timer3')
timerSch.timer.init(period=5000, mode=timerSch.timer.PERIODIC, callback=callback_timer3)


power.epd_power_on()

image0 = M5Img(0, 0, "res/BG_girl_TH.jpg", True)
rectangle_Temp = M5Rect(90, 800, 120, 60, 15, 0)
rectangle_Hum = M5Rect(370, 800, 120, 60, 14, 0)
Temp = M5LabelPlus(93, 820, "lab-T", lcd.FONT_DejaVu40, 0, rotate=0, url='http://api.m5stack.com/v1', json=None, timer=True, interval=1000, error_msg='Error', error_color=0x272727)
label0 = M5TextBox(370, 820, "lab_H", lcd.FONT_DejaVu40, 0, rotate=0)
label_Akku = M5TextBox(440, 930, "label1", lcd.FONT_Comic, 1, rotate=0)

lcd.show()
wait_ms(500)
timerSch.run('timer3', 5000, 0x00)

while True:
  #power.epd_power_on()
  #power.epd_power_off()
  temperature = sht30.temperature
  Hum = sht30.humidity
  Akku_V = bat.voltage()
  Temp.setText(str("%.2f"%(temperature)))
  label0.setText(str("%.2f"%(Hum)))
  label_Akku.setText(str(Akku_V/1000)+"V")
  #lcd.show()
  print((str('Temp:') + (str(temperature)))+"C")
  print((str('Hum:') + (str(Hum)))+"%")
  print((str('Akku:') + (str(Akku_V/1000)))+"V")
  wait_ms(700)
  #power.epd_power_off()
  wait_ms(300)
