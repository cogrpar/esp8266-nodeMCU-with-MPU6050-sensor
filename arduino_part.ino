
#include "Wire.h"
#include <ESP8266WiFi.h>
 
const char* ssid = "SSID";//type your ssid
const char* password = "PASSWORD";//type your password

 //setup connection to webpage
const char server[] = "https://espbots.000webhostapp.com"; 

WiFiClient client;

//void webprint(const char* input);
//void cleardoc();


const uint8_t MPU_addr=0x68; // I2C address of the MPU-6050
 
const float MPU_GYRO_250_SCALE = 131.0;
const float MPU_GYRO_500_SCALE = 65.5;
const float MPU_GYRO_1000_SCALE = 32.8;
const float MPU_GYRO_2000_SCALE = 16.4;
const float MPU_ACCL_2_SCALE = 16384.0;
const float MPU_ACCL_4_SCALE = 8192.0;
const float MPU_ACCL_8_SCALE = 4096.0;
const float MPU_ACCL_16_SCALE = 2048.0;
 
  
 
struct rawdata {
int16_t AcX;
int16_t AcY;
int16_t AcZ;
int16_t Tmp;
int16_t GyX;
int16_t GyY;
int16_t GyZ;
};
 
struct scaleddata{
float AcX;
float AcY;
float AcZ;
float Tmp;
float GyX;
float GyY;
float GyZ;
};
 
bool checkI2c(byte addr);
void mpu6050Begin(byte addr);
rawdata mpu6050Read(byte addr, bool Debug);
void setMPU6050scales(byte addr,uint8_t Gyro,uint8_t Accl);
void getMPU6050scales(byte addr,uint8_t &Gyro,uint8_t &Accl);
scaleddata convertRawToScaled(byte addr, rawdata data_in,bool Debug);
 
void setup() {
  
  Serial.begin(115200);
 
 

  delay(10);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
   
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  mpu6050Begin(MPU_addr);
   
  Wire.begin();
}
 
void loop() {
    cleardoc();
    //now read from the sensor and write to the webpage using these fuctions
    rawdata next_sample;
    setMPU6050scales(MPU_addr,0b00000000,0b00010000);
    next_sample = mpu6050Read(MPU_addr, true);
    convertRawToScaled(MPU_addr, next_sample,true);
   
    delay(1000); // Wait 1 second and scan again

}
  
 
void mpu6050Begin(byte addr){
  // This function initializes the MPU-6050 IMU Sensor
  // It verifys the address is correct and wakes up the
  // MPU.
  if (checkI2c(addr)){
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x6B); // PWR_MGMT_1 register
    Wire.write(0); // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);
 
    delay(30); // Ensure gyro has enough time to power up
  }
}
 
bool checkI2c(byte addr){
  // We are using the return value of
  // the Write.endTransmisstion to see if
  // a device did acknowledge to the address.
  Serial.println(" ");
  webprint(" ");
  Wire.beginTransmission(addr);
 
  if (Wire.endTransmission() == 0){
    Serial.print(" Device Found at 0x");
    Serial.println(addr,HEX);
    //webprint(" Device Found at 0x");
    //webprint(addr);
    //webprint(HEX);
    return true;
  }
  else{
    Serial.print(" No Device Found at 0x");
    Serial.println(addr,HEX);
    //webprint(" No Device Found at 0x");
    //webprint(addr);
    //webprint(HEX);
    return false;
  }
}
 
  
 
rawdata mpu6050Read(byte addr, bool Debug){
  // This function reads the raw 16-bit data values from
  // the MPU-6050
 
  rawdata values;
 
  Wire.beginTransmission(addr);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(addr,14,true); // request a total of 14 registers
  values.AcX=Wire.read()<<8|Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  values.AcY=Wire.read()<<8|Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  values.AcZ=Wire.read()<<8|Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  values.Tmp=Wire.read()<<8|Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  values.GyX=Wire.read()<<8|Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  values.GyY=Wire.read()<<8|Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  values.GyZ=Wire.read()<<8|Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
   
 
  if(Debug){
    
    Serial.print(" GyX = "); Serial.print(values.GyX);
    //webprint(" GyX = "); webprint(values.GyX);
    
    Serial.print(" | GyY = "); Serial.print(values.GyY);
    //webprint(" | GyY = "); webprint(values.GyY);
    
    Serial.print(" | GyZ = "); Serial.print(values.GyZ);
    //webprint(" | GyZ = "); webprint(values.GyZ);
    
    Serial.print(" | Tmp = "); Serial.print(values.Tmp);
    //webprint(" | Tmp = "); webprint(values.Tmp);
    
    Serial.print(" | AcX = "); Serial.print(values.AcX);
    //webprint(" | AcX = "); webprint(values.AcX);
    
    Serial.print(" | AcY = "); Serial.print(values.AcY);
    //webprint(" | AcY = "); webprint(values.AcY);
    
    Serial.print(" | AcZ = "); Serial.println(values.AcZ);
    //webprint(" | AcZ = "); webprint(values.AcZ);
  }
 
  return values;
}
 
void setMPU6050scales(byte addr,uint8_t Gyro,uint8_t Accl){
  Wire.beginTransmission(addr);
  Wire.write(0x1B); // write to register starting at 0x1B
  Wire.write(Gyro); // Self Tests Off and set Gyro FS to 250
  Wire.write(Accl); // Self Tests Off and set Accl FS to 8g
  Wire.endTransmission(true);
}
 
void getMPU6050scales(byte addr,uint8_t &Gyro,uint8_t &Accl){
  Wire.beginTransmission(addr);
  Wire.write(0x1B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(addr,2,true); // request a total of 14 registers
  Gyro = (Wire.read()&(bit(3)|bit(4)))>>3;
  Accl = (Wire.read()&(bit(3)|bit(4)))>>3;
}
 
  
 
scaleddata convertRawToScaled(byte addr, rawdata data_in, bool Debug){

  scaleddata values;
  float scale_value = 0.0;
  byte Gyro, Accl;
   
  getMPU6050scales(MPU_addr, Gyro, Accl);
   
  if(Debug){
    Serial.print("Gyro Full-Scale = ");
    //webprint("Gyro Full-Scale = ");
  }
 
  switch (Gyro){
    case 0:
    scale_value = MPU_GYRO_250_SCALE;
    if(Debug){
      Serial.println("±250 °/s");
      //webprint("±250 °/s");
    }
    break;
    case 1:
    scale_value = MPU_GYRO_500_SCALE;
    if(Debug){
      Serial.println("±500 °/s");
      //webprint("±500 °/s");
    }
    break;
    case 2:
    scale_value = MPU_GYRO_1000_SCALE;
    if(Debug){
      Serial.println("±1000 °/s");
      //webprint("±1000 °/s");
    }
    break;
    case 3:
    scale_value = MPU_GYRO_2000_SCALE;
    if(Debug){
      Serial.println("±2000 °/s");
      //webprint("±2000 °/s");
    }
    break;
    default:
    break;
  }
 
  values.GyX = (float) data_in.GyX / scale_value;
  values.GyY = (float) data_in.GyY / scale_value;
  values.GyZ = (float) data_in.GyZ / scale_value;
   
  scale_value = 0.0;
  if(Debug){
    Serial.print("Accl Full-Scale = ");
    //webprint("Accl Full-Scale = ");
  }
  switch (Accl){
    case 0:
    scale_value = MPU_ACCL_2_SCALE;
    if(Debug){
      Serial.println("±2 g");
      //webprint("±2 g");
    }
    break;
    case 1:
    scale_value = MPU_ACCL_4_SCALE;
    if(Debug){
      Serial.println("±4 g");
      //webprint("±4 g");
    }
    break;
    case 2:
    scale_value = MPU_ACCL_8_SCALE;
    if(Debug){
      Serial.println("±8 g");
      //webprint("±8 g");
    }
    break;
    case 3:
    scale_value = MPU_ACCL_16_SCALE;
    if(Debug){
      Serial.println("±16 g");
      //webprint("±16 g");
    }
    break;
    default:
    break;
  }
  values.AcX = (float) data_in.AcX / scale_value;
  values.AcY = (float) data_in.AcY / scale_value;
  values.AcZ = (float) data_in.AcZ / scale_value;
    
   
  values.Tmp = (float) data_in.Tmp / 340.0 + 36.53;

  int tai = data_in.Tmp / 340.0 + 36.53;

  const char* tal = (const char*) tai;
   
  if(Debug){
    
  Serial.print(" GyX = "); Serial.print(values.GyX);
  webprint(" GyX = "); webprint(values.GyX);
  
  Serial.print(" °/s| GyY = "); Serial.print(values.GyY);
  webprint(" °/s| GyY = "); webprint(values.GyY);
  
  Serial.print(" °/s| GyZ = "); Serial.print(values.GyZ);
  webprint(" °/s| GyZ = "); webprint(values.GyZ);
   
  Serial.print(" °/s| Tmp = "); Serial.print(values.Tmp);
  //webprint(" °/s| Tmp = "); webprint(values.Tmp);
  
  Serial.print(" °C| AcX = "); Serial.print(values.AcX);
  //webprint(" °C| AcX = "); webprint(values.AcX);
  
  Serial.print(" g| AcY = "); Serial.print(values.AcY);
  //webprint(" g| AcY = "); webprint(values.AcY);
  
  Serial.print(" g| AcZ = "); Serial.print(values.AcZ);Serial.println(" g");
  //webprint(" g| AcZ = "); webprint(values.AcZ);webprint(" g");
  }
   
  return values;
}

//webprint
void webprint(const char* input) {

  
  delay(5);


  //Serial.println("\nStarting connection to server..."); 
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    //Serial.println("connected to server");
    //WiFi.printDiag(Serial);

    String data = "var=" + (String)input;

     //change URL below to the Sub-Domain
     client.println("POST /Home/Code2.php HTTP/1.1"); 
     //change URL below ito match the Domain
     client.print("Host: espbots.000webhostapp.com\n");                 
     client.println("User-Agent: ESP8266/1.0");
     client.println("Connection: close"); 
     client.println("Content-Type: application/x-www-form-urlencoded");
     client.print("Content-Length: ");
     client.print(data.length());
     client.print("\n\n");
     client.print(data);
     client.stop(); 
     
     //Serial.println("\n");
    //Serial.println("this is what we sent: ");
     //Serial.println(data);
     //Serial.println("which was this many bytes: ");
     //Serial.println(data.length());       
     delay(5);
    } 

}


void webprint(char input) {

  
  delay(5);


  //Serial.println("\nStarting connection to server..."); 
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    //Serial.println("connected to server");
    //WiFi.printDiag(Serial);

    String data = "var=" + (String)input;

     //change URL below to the Sub-Domain
     client.println("POST /Home/Code2.php HTTP/1.1"); 
     //change URL below ito match the Domain
     client.print("Host: espbots.000webhostapp.com\n");                 
     client.println("User-Agent: ESP8266/1.0");
     client.println("Connection: close"); 
     client.println("Content-Type: application/x-www-form-urlencoded");
     client.print("Content-Length: ");
     client.print(data.length());
     client.print("\n\n");
     client.print(data);
     client.stop(); 
     
     //Serial.println("\n");
    //Serial.println("this is what we sent: ");
     //Serial.println(data);
     //Serial.println("which was this many bytes: ");
     //Serial.println(data.length());       
     delay(5);
    } 

}


void webprint(int input) {

  
  delay(5);


  //Serial.println("\nStarting connection to server..."); 
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    //Serial.println("connected to server");
    //WiFi.printDiag(Serial);

    String data = "var=" + (String)input;

     //change URL below to the Sub-Domain
     client.println("POST /Home/Code2.php HTTP/1.1"); 
     //change URL below ito match the Domain
     client.print("Host: espbots.000webhostapp.com\n");                 
     client.println("User-Agent: ESP8266/1.0");
     client.println("Connection: close"); 
     client.println("Content-Type: application/x-www-form-urlencoded");
     client.print("Content-Length: ");
     client.print(data.length());
     client.print("\n\n");
     client.print(data);
     client.stop(); 
     
     //Serial.println("\n");
    //Serial.println("this is what we sent: ");
     //Serial.println(data);
     //Serial.println("which was this many bytes: ");
     //Serial.println(data.length());       
     delay(5);
    } 

}


void webprint(float input) {

  
  delay(5);


  //Serial.println("\nStarting connection to server..."); 
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    //Serial.println("connected to server");
    //WiFi.printDiag(Serial);

    String data = "var=" + (String)input;

     //change URL below to the Sub-Domain
     client.println("POST /Home/Code2.php HTTP/1.1"); 
     //change URL below ito match the Domain
     client.print("Host: espbots.000webhostapp.com\n");                 
     client.println("User-Agent: ESP8266/1.0");
     client.println("Connection: close"); 
     client.println("Content-Type: application/x-www-form-urlencoded");
     client.print("Content-Length: ");
     client.print(data.length());
     client.print("\n\n");
     client.print(data);
     client.stop(); 
     
     //Serial.println("\n");
    //Serial.println("this is what we sent: ");
     //Serial.println(data);
     //Serial.println("which was this many bytes: ");
     //Serial.println(data.length());       
     delay(5);
    } 

}

void cleardoc (){
  
  delay(20);


  //Serial.println("\nStarting connection to server..."); 
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
   // Serial.println("connected to server");
    WiFi.printDiag(Serial);

    String endthis = "endthis=" + (String)"1";

     //change URL below to the Sub-Domain
     client.println("POST /Home/Code2.php HTTP/1.1"); 
     //change URL below ito match the Domain
     client.print("Host: espbots.000webhostapp.com\n");                 
     client.println("User-Agent: ESP8266/1.0");
     client.println("Connection: close"); 
     client.println("Content-Type: application/x-www-form-urlencoded");
     client.print("Content-Length: ");
     client.print(endthis.length());
     client.print("\n\n");
     client.print(endthis);
     client.stop(); 
     
     //Serial.println("\n");
     //Serial.println("this is what we sent: ");
     //Serial.println(endthis);
     //Serial.println("which was this many bytes: ");
     //erial.println(endthis.length());       
     delay(20);
    } 
  
}

