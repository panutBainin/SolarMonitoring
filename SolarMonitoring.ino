#include <LiquidCrystal_I2C.h>
// set the LCD number of columns and rows
int lcdColumns = 20;
int lcdRows = 4;
// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

#include <HardwareSerial.h>
#include <PZEM004Tv30.h>
HardwareSerial PzemSerial2(2);     // Use hwserial UART2 at pins IO-16 (RX2) and IO-17 (TX2)
PZEM004Tv30 pzem(&Serial2);



#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#define BLYNK_PRINT Serial

char auth[] = "TE4YCebPcq2FRuwpx1EeKAWSBijIqgbc";//Enter the Auth code which was send by Blink
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "1161304210494";
char pass[] = "095325";



int offset =130;// set the correction offset value
double voltDC;
#define Vin A0


const int VIN = A3; // define the Arduino pin A0 as voltage input (V in)
const float VCC   = 3.279;// supply voltage
const int MODEL = 3;   // enter the model (see above list)
#include <Robojax_AllegroACS_Current_Sensor.h>
Robojax_AllegroACS_Current_Sensor robojax(MODEL,VIN);


unsigned long previousMillis = 0;
const long interval = 300000;           // ช่วงเวลากระพริบ

void setup() {
  
  Serial.begin(115200);

   lcd.begin();
  lcd.clear();                       
  lcd.backlight();
  lcd.setCursor(3, 0); lcd.print("Solar Monitoring");
 
  Serial.println("Solar Monitor");



// รอเชื่อมต่อจากไอดีและพาสเวิร์ดของไวไฟอาจารย์องอาจครับ เมื่อเปลี่ยนไอดีและพาสเวิร์ดให้ปลดคอมเมนต์ส่วนเชื่อมต่อไวไฟออกครับ
//     WiFi.begin(ssid, pass);
//  Serial.printf("WiFi connecting to %s\n",  ssid);
//  if(WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(400); }
//  Serial.printf("\nWiFi connected\nIP : ");   // จะหลุดออกจาก while ก็ต่อเมือ Connected เรียบร้อย
//  Serial.println(WiFi.localIP());     

//เริ่มการเชื่อมต่อ Blynk Server ปลดคอมเมนต์ออกครับ เมื่อพร้อมต่อใช้งานกับไวไฟ
//  Blynk.begin(auth, ssid, pass);
delay(1000);

}

void loop() {

  //เชื่อมต่อ Blynk Server ปลดคอมเมนต์ออกครับ เมื่อพร้อมต่อใช้งานกับไวไฟ
      // Blynk.run();



////////////////////////////////////////////////////////วัดแรงดัน DC ด้วย Volage Devider โดยใช้ R=50k , 5k วัดแรงดันได้สูงสุด 40V////////////////////////////////////////////////////
     
       unsigned int v = analogRead(Vin);  // อ่านอินพุต        
       voltDC = map(v,0,4095, 0, 4000) + offset;// map 0-1023 to 0-2500 และเช็คความถูกต้องด้วยoffset
       voltDC /=100;// หารโดย 100 เพื่อเป็นเลข 2 หลัก
       Serial.print("VoltageDC: ");
        Serial.print(voltDC);
        Serial.println(" V");
        Blynk.virtualWrite(V5, voltDC);
// delay(2000);     
////////////////////////////////////////////////////////วัดกระแส DC///////////////////////////////////////////////////////////////////////////////////////////////////////

//Robojax.com ACS758 Current Sensor 
  //Robojax.com Allegro ACS Current Sensor  
   double currentDC = robojax.getCurrent();
    currentDC -= 164.3;
    Serial.print("CurrentDC: ");
    Serial.print(currentDC,2); // print the current with 3 decimal places
    Serial.println(" A");
//    Serial.print(robojax.getCurrentAverage(300),3);//   
//    Serial.println("A");
   //robojax.debug();
   Blynk.virtualWrite(V6, currentDC);  
// delay(2000); 

///////////////////////////////////////////////////////////กำลังไฟฟ้าDC//////////////////////////////////////////////////////////////////////////////////////////////////////

    double powerDC = voltDC*currentDC;
    Serial.print("powerDC: ");
    Serial.print(powerDC,2); // print the current with 3 decimal places
    Serial.println(" W");
    Blynk.virtualWrite(V7, powerDC);  

//    delay(2000);
/////////////////////////////////////////////////////////วัดไฟฟ้ากระแสสลับ////////////////////////////////////////////////////////////////////////////////////////////////////

 float voltage = pzem.voltage();
  if(isnan(voltage)){
    Serial.println("Error reading voltage");
  } else {
    Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
    Blynk.virtualWrite(V0, voltage);    
  }

  float current = pzem.current();
  if(isnan(current)){
    Serial.println("Error reading current");
  } else {
    Serial.print("Current: "); Serial.print(current); Serial.println("A");
    Blynk.virtualWrite(V1, current);
  }

  float power = pzem.power();
  if(isnan(power)){
    Serial.println("Error reading power");
  } else {
    Serial.print("Power: "); Serial.print(power); Serial.println("W");
    Blynk.virtualWrite(V2, power);
  }

  float energy = pzem.energy();
  if(isnan(energy)){
    Serial.println("Error reading energy");
  } else {
    Serial.print("Energy: "); Serial.print(energy,3); Serial.println("kWh"); 
    Blynk.virtualWrite(V4, energy);
  }


//  float pf = pzem.pf();
//  if(isnan(pf)){
//    Serial.println("Error reading power factor");
//  } else {
//    Serial.print("PF: "); Serial.println(pf);
//    Blynk.virtualWrite(V5, pf);
//  }
//
//  float frequency = pzem.frequency();
//  if(isnan(frequency)){
//    Serial.println("Error reading frequency");
//  } else {
//    Serial.print("Frequency: "); Serial.print(frequency, 1); Serial.println("Hz");
//    Blynk.virtualWrite(V6, frequency);
//  }
    Serial.println();  
    delay(4000); 


       lcd.clear(); 
       lcd.setCursor(0, 0);lcd.print("Vdc:");lcd.print(voltDC,1); lcd.print("V");
//       lcd.setCursor(0, 1);lcd.print("I:");lcd.print(currentDC); lcd.print("A");                       
       lcd.setCursor(0, 1);lcd.print("Vac:");lcd.print(voltage); lcd.print("V");
       lcd.setCursor(0, 2);lcd.print("Iac:");lcd.print(current);lcd.print("A");
       lcd.setCursor(0, 3);lcd.print("Pac:");lcd.print(power);lcd.print("W");


// รอเชื่อมต่อจากไอดีและพาสเวิร์ดของไวไฟอาจารย์องอาจครับ เมื่อเปลี่ยนไอดีและพาสเวิร์ดให้ปลดคอมเมนต์ส่วนเชื่อมต่อไวไฟออกครับ       
// unsigned long currentMillis = millis();
//  if (currentMillis - previousMillis >= interval) {
//    
//    previousMillis = currentMillis;
//         WiFi.begin(ssid, pass);
//  Serial.printf("WiFi connecting to %s\n",  ssid);
//  while(WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(400); }
//  Serial.printf("\nWiFi connected\nIP : ");   // จะหลุดออกจาก while ก็ต่อเมือ Connected เรียบร้อย
//  Serial.println(WiFi.localIP()); 
//    
//    }

    
}
