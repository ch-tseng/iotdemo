#include <WirelessIoT.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
WirelessIoT iot(1);

String lcdLine0 = "SunplusIT IoT";

byte pinButton1 = 12;  //Temperature
byte pinButton2 = 11;  //PIR
byte pinButton3 = 10;  //Door
byte pinButton4 = 9;  //Remote Key
byte pinButton5 = 8;  //Smart Plug
byte pinButton6 = 7;  //Empty devices
byte pinButton7 = 5;  //LED color
byte pinButton8 = 6;  //LED lightness
byte pinButton9 = 13;  //Clear all devices
byte pinLED_red = 3;  //LED
byte pinLED_blue = 4;  //LED
byte pinLED_green = 2;  //LED

int lightnessLED = 0;
int colorLED = 0;

void displayLCD(int yPosition, String txtMSG) {
  int xPos;
   if(txtMSG.length()>16) {
     xPos = 0;
     if(xPos<0) xPos=0;    
   }else{
      xPos = (16-txtMSG.length())/2;
   }
   lcd.setCursor(xPos,yPosition);
   lcd.print(txtMSG);
}

String padString(String stringTXT, char padTXT, int limitWidth,String alignType) {
  int numEmpty = limitWidth - stringTXT.length();
  for (int numI = 0; numI < numEmpty; numI++) {
    if(alignType=="left") {
      stringTXT = stringTXT + padTXT;
    }else if(alignType=="right") {
      stringTXT = padTXT + stringTXT;
    }else{
      (numI % 2 == 0) ? (stringTXT = padTXT + stringTXT) : (stringTXT = stringTXT + padTXT);
    }
  }
  return stringTXT;
}

void sendBlink(int deviceType, String typeDesc, unsigned int deviceID) {
  digitalWrite(pinLED_green, HIGH);
  iot.blinkDevice(deviceType, 0);
  String line2 = "";
  line2.concat(padString("Blink to " + typeDesc + ' ' + String(deviceID), ' ', 20, "center"));
  displayLCD(1, "                    "); 
  displayLCD(2, line2); 
  String line3 = "";
  line3.concat(padString(typeDesc + " will light", ' ', 20, "center"));
  displayLCD(3, line3); 
  Serial.println("          ---> Blink to " + typeDesc);
  delay(400);
  digitalWrite(pinLED_green, LOW);
  delay(200);
  digitalWrite(pinLED_green, HIGH);
  delay(800);
  digitalWrite(pinLED_green, LOW);
}

// Device Type: 0-->All, 1-->PIR, 2-->SmartPlug, 3-->Door, 4-->T&H, 5--> RemoteKey, 6--> LED
void clearDeviceList(int deviceType, unsigned int waitSeconds) {
  int id;

  digitalWrite(pinLED_blue, LOW);
  digitalWrite(pinLED_green, HIGH);
  
  switch (deviceType) {
    case 0:
      //Clear all devices
      displayLCD(3, padString("Clear ALL units.", ' ', 20, "center"));
      Serial.println("          --->Empty ALL devices");
      iot.clrAllDevice();
      break;
    case 1:
      //PIR
      displayLCD(3, padString("Clear PIR units.", ' ', 20, "center"));
      Serial.println("          --->Empty PIR devices");
      id = iot.clrAsynTypeDevice(T_PIR);
      break;
    case 2:
      //Smart Plug
      displayLCD(3, padString("Clear PLUG units.", ' ', 20, "center"));
      Serial.println("          --->Empty PLUG devices");
      id = iot.clrAsynTypeDevice(T_SMARTPLUG);
      break;
    case 3:
      //Door
      displayLCD(3, padString("Clear DOOR units.", ' ', 20, "center"));
      Serial.println("          --->Empty DOOR devices");
      id = iot.clrAsynTypeDevice(T_DOOR);
      break;
    case 4:
      //Temperature & Humidity
      displayLCD(3, padString("Clear T/H units.", ' ', 20, "center"));
      Serial.println("          --->Empty T/H devices");
      id = iot.clrAsynTypeDevice(T_TH);
      break;
    case 5:
      //Remote Key
      displayLCD(3, padString("Clear KEY units.", ' ', 20, "center"));
      Serial.println("          --->Empty KEY devices");
      id = iot.clrAsynTypeDevice(T_REMOTEKEY);
      break;
    case 6:
      //LED
      displayLCD(3, padString("Clear LED units.", ' ', 20, "center"));
      Serial.println("          --->Empty LED devices");
      id = iot.clrAsynTypeDevice(T_LED_CTRL);
      break;
    break;
  }
  
  unsigned long timer1 = millis();
  do{
    digitalWrite(pinLED_green, LOW);
    delay(250);
    digitalWrite(pinLED_green, HIGH);
  
    int res = iot.stAsynCommand(id);
    unsigned int timerLength = (millis() - timer1)/1000;
    Serial.print('[' + String(timerLength) + ']'); Serial.print("Clearing devices for " + String(deviceType) + " now --->"); Serial.println(res);
    
    if (res == ST_OK) {
      Serial.print("Clean device for "+String(deviceType)+" success!");
      break;
    }else{
      if(timerLength>waitSeconds) {
        Serial.println("Has waited for " + String(waitSeconds) + " s, give up to clear device " + String(deviceType) + ". ");
        break;
      }
    }
    //delay(500);
  } while (true);
  digitalWrite(pinLED_green, LOW);
  
}


void setup() {
  
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  
  iot.begin();

  // Button 
  pinMode(pinButton1, INPUT);
  pinMode(pinButton2, INPUT);
  pinMode(pinButton3, INPUT);
  pinMode(pinButton4, INPUT);
  pinMode(pinButton5, INPUT);
  pinMode(pinButton6, INPUT);
  pinMode(pinButton7, INPUT);
  pinMode(pinButton8, INPUT);
  pinMode(pinButton9, INPUT);
  pinMode(pinLED_red, OUTPUT);
  pinMode(pinLED_blue, OUTPUT);
  pinMode(pinLED_green, OUTPUT);

  Serial.println(">> Wait startIOT....");
  delay(1000);
  iot.startIoT();
  //deviceInfo();
  Serial.println(">> Finish startIOT....");
    
  displayLCD(0, padString(lcdLine0, ' ', 20, "center"));
  
}

boolean initStart = 1;
int tot;

void loop() {
  //deviceInfo();
 
  String lcdLine1 = "";
  String lcdLine2 = "";
  String lcdLine3 = "";
  String lcdLine_1;
  String lcdLine_2; 
  String lcdLine_3; 

  if(initStart==1) {
    Serial.println(">> Wait startIOT....");
    //iot.startIoT();
    delay(30000);
    Serial.print("Total Devices: "); Serial.println(iot.totalDevices());
    Serial.println(">> Finish startIOT....");
    initStart = 0;
  }

  //溫溼度模組----------------------------------------------------
  int res_TempHum;
  int deviceID_TempHum = 0;
  byte vTemp, vHum;
  
  res_TempHum = iot.rdTemperature(deviceID_TempHum, &vTemp);
  if (res_TempHum == ST_OK) {
    iot.rdHumidity(deviceID_TempHum, &vHum);
    //Serial.print("Temp="); Serial.print(temp); Serial.print(" / "); Serial.print("Humi="); Serial.println(hum);      
    lcdLine_2 = " T:"+String(vTemp) + "C H:" + String(vHum)+'%';
  }else{
    lcdLine_2 = "           ";   
  }

    //門匙模組---------------------------------------------------
   int res_Door;
   int deviceID_Door = 0;
   byte vDoor;
   
   res_Door = iot.rdDoor(deviceID_Door, &vDoor);
   if (res_Door == ST_OK) {
      lcdLine_1 = "Door:"+String(vDoor);
      //sendBlink(T_DOOR, "DOOR", 0);  //讓Door指示燈閃爍, 可告知使用者收到訊息.
   }else{
      lcdLine_1 = "         ";  
   }
   lcdLine1.concat(padString(lcdLine_1 + lcdLine_2 , ' ', 20, "center"));
   //Serial.println(lcdLine1);
   displayLCD(2, lcdLine1);

   //LED模組---------------------------------------------------- 
   int res_LED;
   int deviceID_LED = 0;
   byte vBright, vColor;

   res_LED = iot.rdBrightness(deviceID_LED, &vBright);
   
   if (res_LED == ST_OK) {
      iot.rdColorTemp(deviceID_LED, &vColor);
      //lightnessLED = vBright;
      //colorLED = vColor;
      lcdLine_1 = "Light:"+String(vBright) + " Color:" + String(vColor);
   }else{
      lcdLine_1 = "                    ";  
   }
   lcdLine2.concat(padString(lcdLine_1 , ' ', 20, "center"));
   displayLCD(1, lcdLine2);
   //PIR模組---------------------------------------------------- 
   int res_PIR;
   int deviceID_PIR = 0;
   byte vPir;

   res_PIR = iot.rdPIR(deviceID_PIR, &vPir);
    if (res_PIR == ST_OK) {
      lcdLine_3 = " PIR:"+String(vPir);
    }else{
      lcdLine_3 = "     ";
    }
    
  //SmartPlug模組-------------------------------------------------
  int res_SmartPlug;
  int deviceID_SmartPlug = 0;
  byte vPlug;
  
  res_SmartPlug = iot.rdSmartPlug(deviceID_SmartPlug, &vPlug);
  if (res_SmartPlug == ST_OK) {
    lcdLine_2 = " Plug:"+String(vPlug);
  }else{
    lcdLine_2 = "      ";
  }
 
  //RemoteKey模組-------------------------------------------------
  int res_RemoteKey;
  int deviceID_RemoteKey = 0;
  byte vKey;
   
  res_RemoteKey = iot.rdRemoteKeyAlm(deviceID_RemoteKey, &vKey);
  //Serial.print("status: remoteKey = "); Serial.println(vKey);
  if (res_RemoteKey == ST_OK) {
    lcdLine_1 = "Key:"+String(vPlug);

    if(vKey==1) {
      if (vPlug == 0) {
        vPlug = iot.ctrlSmartPlug(deviceID_SmartPlug, true);
        //sendBlink(T_REMOTEKEY, "KEY", 0);  //讓Remote Key閃爍, 可告知使用者收到訊息.
      }else{
        vPlug = iot.ctrlSmartPlug(deviceID_SmartPlug, false);
        //sendBlink(T_REMOTEKEY, "KEY", 0);  //讓Remote Key閃爍, 可告知使用者收到訊息.
      }
    }
        
  }else{
    lcdLine_1 = "     ";  
  }
  lcdLine3.concat(padString(lcdLine_1 + lcdLine_2 + lcdLine_3, ' ', 20, "center"));
  //Serial.println(lcdLine3);  
  displayLCD(3, lcdLine3); 
  
  //---Control LED module------------------------------------------------
  if(digitalRead(pinButton7)>0) {
    
    lightnessLED += 1;
    if(lightnessLED>12) { lightnessLED=0; }
    //if(lightnessLED<0) { lightnessLED=11; }
    
    digitalWrite(pinLED_green, HIGH);
    if(lightnessLED==0) {
      iot.lightOnOff(0, TURN_ON);
    }else if(lightnessLED==12) {
      iot.lightOnOff(0, TURN_OFF);
    }else{
      iot.lightSet(0, lightnessLED, colorLED);
    }
    
    Serial.println("LED lightness --> " + String(lightnessLED));
    delay(600);
    digitalWrite(pinLED_green, LOW);
  }

  if(digitalRead(pinButton8)>0) {
    colorLED += 1;
    if(colorLED>11) { colorLED=1; }
    if(colorLED<1) { colorLED=11; }
    
    digitalWrite(pinLED_green, HIGH);
    iot.lightSet(0, lightnessLED, colorLED);
    Serial.println("LED color --> " + String(colorLED));
    delay(600);
    digitalWrite(pinLED_green, LOW);
  }
   
  //---Blink--------------------------------------------------------------
  if(digitalRead(pinButton1)>0) {    
    sendBlink(T_TH, "T/H", 0);
  }
  
  if(digitalRead(pinButton2)>0) {
    sendBlink(T_PIR, "PIR", 0);
  }

  if(digitalRead(pinButton3)>0) {
    sendBlink(T_DOOR, "DOOR", 0);
  }

  if(digitalRead(pinButton4)>0) {
    sendBlink(T_REMOTEKEY, "KEY", 0);
  }

  if(digitalRead(pinButton5)>0) {
    sendBlink(T_SMARTPLUG, "PLUG", 0);
  }
  

  if(digitalRead(pinButton9)>0) {
    Serial.println("          ---> Blink to all devices");
    sendBlink(T_TH, "T/H", 0);
    delay(300);
    sendBlink(T_PIR, "PIR", 0);
    delay(300);
    sendBlink(T_DOOR, "DOOR", 0);
    delay(300);
    sendBlink(T_REMOTEKEY, "KEY", 0);
    delay(300);
    sendBlink(T_SMARTPLUG, "PLUG", 0);
    delay(300);  
  }


  //---------------------------- Clear actions-------------------------------------------//
  
  if(digitalRead(pinButton6)>0) {
    digitalWrite(pinLED_blue, HIGH);
    Serial.println("          --->Empty device mode is active!");
    displayLCD(1, " [Clear Device Mode]"); 
    displayLCD(2, "                    "); 
    displayLCD(3, "                    "); 
    
    // Device Type: 0-->All, 1-->PIR, 2-->SmartPlug, 3-->Door, 4-->T&H, 5--> RemoteKey, 6--> LED
    if(digitalRead(pinButton1)>0) {
      clearDeviceList(4, 10);
      delay(500);
    }
    
    if(digitalRead(pinButton2)>0) {
      clearDeviceList(1, 10);
      delay(500);
    }
    
    if(digitalRead(pinButton3)>0) {
      clearDeviceList(3, 10);
      delay(500);
    }
    
    if(digitalRead(pinButton4)>0) {
      clearDeviceList(5, 10);
      delay(500);
    }

    if(digitalRead(pinButton5)>0) {
      clearDeviceList(2, 10);
      delay(500);
    }

    if(digitalRead(pinButton7)>0 or digitalRead(pinButton8)>0) {
      clearDeviceList(6, 10);
      delay(500);
    }

    if(digitalRead(pinButton9)>0) {
      clearDeviceList(0, 10);
      delay(500);
    }
    
  }else{
    digitalWrite(pinLED_blue, LOW);
  }
    //deviceInfo();
    delay(100);
}
