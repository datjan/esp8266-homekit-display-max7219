
/*  WEMOS D1 Mini
                     ______________________________
                    |   L T L T L T L T L T L T    |
                    |                              |
                 RST|                             1|TX HSer
                  A0|                             3|RX HSer
                  D0|16                           5|D1
                  D5|14                           4|D2
                  D6|12                    10kPUP_0|D3
RX SSer/HSer swap D7|13                LED_10kPUP_2|D4
TX SSer/HSer swap D8|15                            |GND
                 3V3|__                            |5V
                       |                           |
                       |___________________________|

       ______________
      |              |
      |   max7219    |
      |              |
      |              |
      |              |
      |______________|
      |   |   |   |  |
     VCC GND DIN CS CLK

      
// connection schema
D1 mini -> max7219

3V3 -> VCC
GND -> GND
D4 -> CS
D5 -> CLK
D7 - DIN

*/

#include <Arduino.h>
#include <arduino_homekit_server.h>   // https://github.com/datjan/Arduino-HomeKit-ESP8266
#include "wifi_info.h"
#include "LedControl.h"          // https://github.com/wayoda/LedControl


#define LOG_D(fmt, ...)   printf_P(PSTR(fmt "\n") , ##__VA_ARGS__);



// ----------------------------- SETUP -----------------------------------

int countModules = 1; // max7219 modules count
int intense = 3; // intense: 8 (default), Range 1 - 16
int orientation = 3; // orientation: 1 (default), 2 (90°), 3 (180°), 4 (270°)
int inverted = 0; // inverted: 0 (default), 1 (inverted)

// ----------------------------- SETUP -----------------------------------



String image = ""; // image identifier
int pinDIN = 13;
int pinCLK = 14;
int pinCS = 2;

unsigned long delaytime = 100;  /* we always wait a bit between updates of the display */

LedControl lc = LedControl(pinDIN,pinCLK,pinCS,countModules); 


void setup() {
  Serial.begin(115200);
  wifi_connect(); // in wifi_info.h
  //homekit_storage_reset(); // to remove the previous HomeKit pairing storage when you first run this new HomeKit example
  my_homekit_setup();
  displayInitAll(intense);
}

void loop() {
  my_homekit_loop();
  delay(10);
}

//==============================
// HomeKit setup and loop
//==============================

// access your HomeKit characteristics defined in my_accessory.c
extern "C" homekit_server_config_t config;
extern "C" homekit_characteristic_t cha_switch_on_01;
extern "C" homekit_characteristic_t cha_switch_on_02;
extern "C" homekit_characteristic_t cha_switch_on_03;

static uint32_t next_heap_millis = 0;



//Called when the switch value is changed by iOS Home APP
void cha_switch_on_01_setter(const homekit_value_t value) {
  cha_switch_on_01.value.bool_value = value.bool_value;  //sync the value

  if (value.bool_value==true) {
    displayInitAll(intense);

    switchOff(0);
  } else { displayInitAll(intense); }
}
void cha_switch_on_02_setter(const homekit_value_t value) {
  cha_switch_on_02.value.bool_value = value.bool_value;  //sync the value

  if (value.bool_value==true) {
    displaySet(0, intense, "emoji-smile", orientation, inverted);
    
    switchOff(2);
  } else { displayInitAll(intense); }
}
void cha_switch_on_03_setter(const homekit_value_t value) {
  cha_switch_on_03.value.bool_value = value.bool_value;  //sync the value

  if (value.bool_value==true) {
    displaySet(0, intense, "lightning", orientation, inverted);
    
    switchOff(3);
  } else { displayInitAll(intense); }
}


void switchOff(int exclude) {
  // Set switches to off (exclude any switch id)
  if (exclude!=1) {
    cha_switch_on_01.value.bool_value = false;
    homekit_characteristic_notify(&cha_switch_on_01, cha_switch_on_01.value);
  }
  if (exclude!=2) {
    cha_switch_on_02.value.bool_value = false;
    homekit_characteristic_notify(&cha_switch_on_02, cha_switch_on_02.value);
  }
  if (exclude!=3) {
    cha_switch_on_03.value.bool_value = false;
    homekit_characteristic_notify(&cha_switch_on_03, cha_switch_on_03.value);
  }
}

void my_homekit_setup() {

  //Add the .setter function to get the switch-event sent from iOS Home APP.
  cha_switch_on_01.setter = cha_switch_on_01_setter;
  cha_switch_on_02.setter = cha_switch_on_02_setter;
  cha_switch_on_03.setter = cha_switch_on_03_setter;
  
  arduino_homekit_setup(&config);

}

void my_homekit_loop() {
  arduino_homekit_loop();
  const uint32_t t = millis();
  if (t > next_heap_millis) {
    // show heap info every 5 seconds
    next_heap_millis = t + 5 * 1000;
    LOG_D("Free heap: %d, HomeKit clients: %d",
        ESP.getFreeHeap(), arduino_homekit_connected_clients_count());

  }
}

void displayInitAll(int intense) {
  
  lc = LedControl(pinDIN,pinCLK,pinCS,countModules); 

  for(int i=0;i<countModules;i++){
    lc.shutdown(i,false);
    lc.setIntensity(i,intense);
    lc.clearDisplay(i);
  }

  delay(delaytime);
    
}

void displaySet(int display, int intense, String image, int ori, int inv) {
  
  lc.shutdown(display,false);
  lc.setIntensity(display,intense);
  lc.clearDisplay(display);


  byte off[8] = {B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000};

  byte skull[8]={B00100010,B00011100,B00111110,B00101010,B00111110,B00011100,B00100010,B00000000};    
  byte lightning[8]={B01100000,B00110000,B00011000,B01111100,B00110000,B00011000,B00001100,B00000000};
  byte stars[8]={B00100000,B01110000,B00100010,B00000111,B00100010,B01110000,B00100000,B00000000};
  byte umbrella[8]={B00111100,B01111110,B11111111,B11111111,B00001000,B00001000,B00101000,B00010000};
  byte box[8]={B11111111,B10000001,B10000001,B10000001,B10000001,B10000001,B10000001,B11111111};

  byte heating[8]={B00000000,B00000000,B00000000,B00000000,B00000000,B11111110,B10101010,B10101010};
  byte heatingheat[8]={B00000000,B01001001,B01001001,B10010010,B00000000,B11111110,B10101010,B10101010};
  
  byte pacman[8]={B00111100,B01001110,B11001100,B11111000,B11111000,B11111100,B01111110,B00111100};
  byte pacghost[8]={B00111100,B01111110,B01101101,B01101101,B11111111,B11111111,B11111111,B11011011};

  byte emojismile[8]={B00111100,B01111110,B11011011,B11111111,B10111101,B11000011,B01111110,B00111100};
  byte emojiworried[8]={B00111100,B01111110,B11011011,B11111111,B11000011,B10111101,B01111110,B00111100};
  byte emojiconfused[8]={B00111100,B01111110,B11011011,B11111111,B10110011,B11001101,B01111110,B00111100};
  byte emojiopenmouth[8]={B00111100,B01111110,B11011011,B11111111,B11100111,B11000011,B11100111,B00111100};
 
  byte num1[8]={B00000000,B00011000,B00111000,B00011000,B00011000,B00011000,B00111100,B00000000};
  byte num2[8]={B00000000,B00111100,B01100110,B00000110,B00111100,B01100000,B01111110,B00000000};
  byte num3[8]={B00000000,B00111100,B00000110,B00011100,B00000110,B01100110,B00111100,B00000000};
  byte num4[8]={B00000000,B00001100,B00011100,B00111100,B01101100,B01111110,B00001100,B00000000};
  byte num5[8]={B00000000,B01111100,B01100000,B01111100,B00000110,B01100110,B00111100,B00000000};
  byte num6[8]={B00000000,B00111100,B01100000,B01111100,B01100110,B01100110,B00111100,B00000000};
  byte num7[8]={B00000000,B01111110,B01100110,B00000110,B00001100,B00011000,B00011000,B00000000};
  byte num8[8]={B00000000,B00111100,B01100110,B00111100,B01100110,B01100110,B00111100,B00000000};
  byte num9[8]={B00000000,B00111100,B01100110,B01100110,B00111110,B00000110,B00111100,B00000000};
  byte num0[8]={B00000000,B00111100,B01100110,B01101110,B01110110,B01100110,B00111100,B00000000};

  byte digA[8] = {B00000000,B00111100,B01100110,B01100110,B01111110,B01100110,B01100110,B00000000};
  byte digB[8] = {B00000000,B01111100,B01100110,B01111100,B01100110,B01100110,B01111100,B00000000};
  byte digC[8] = {B00000000,B00111100,B01100110,B01100000,B01100000,B01100110,B00111100,B00000000};
  byte digD[8] = {B00000000,B01111100,B01100110,B01100110,B01100110,B01100110,B01111100,B00000000};
  byte digE[8] = {B00000000,B01111110,B01100000,B01111100,B01100000,B01100000,B01111110,B00000000};
  byte digF[8] = {B00000000,B01111110,B01100000,B01100000,B01111100,B01100000,B01100000,B00000000};
  byte digG[8] = {B00000000,B00111100,B01100110,B01100000,B01101110,B01100110,B00111110,B00000000};
  byte digH[8] = {B00000000,B01100110,B01100110,B01111110,B01100110,B01100110,B01100110,B00000000};
  byte digI[8] = {B00000000,B00111100,B00011000,B00011000,B00011000,B00011000,B00111100,B00000000};
  byte digJ[8] = {B00000000,B00001110,B00000110,B00000110,B00000110,B01100110,B00111100,B00000000};
  byte digK[8] = {B00000000,B01100110,B01101100,B01111000,B01111000,B01101100,B01100110,B00000000};
  byte digL[8] = {B00000000,B01100000,B01100000,B01100000,B01100000,B01100000,B01111110,B00000000};
  byte digM[8] = {B00000000,B01100010,B01110110,B01111110,B01101010,B01100010,B01100010,B00000000};
  byte digN[8] = {B00000000,B01100010,B01110010,B01111010,B01101110,B01100110,B01100010,B00000000};
  byte digO[8] = {B00000000,B00111100,B01100110,B01100110,B01100110,B01100110,B00111100,B00000000};
  byte digP[8] = {B00000000,B01111100,B01100110,B01100110,B01100110,B01100000,B01100000,B00000000};
  byte digQ[8] = {B00000000,B01111100,B01100110,B01100110,B01101110,B01100100,B01111010,B00000000};
  byte digR[8] = {B00000000,B01111100,B01100110,B01100110,B01111100,B01100110,B01100110,B00000000};
  byte digS[8] = {B00000000,B00111100,B01100000,B00111100,B00000110,B01100110,B00111100,B00000000};
  byte digT[8] = {B00000000,B01111110,B00011000,B00011000,B00011000,B00011000,B00011000,B00000000};
  byte digU[8] = {B00000000,B01100110,B01100110,B01100110,B01100110,B01100110,B00111100,B00000000};
  byte digV[8] = {B00000000,B01100110,B01100110,B01100110,B01100110,B00111100,B00011000,B00000000};
  byte digW[8] = {B00000000,B01100010,B01100010,B01101010,B01111110,B01110110,B01100010,B00000000};
  byte digX[8] = {B00000000,B01100110,B01100110,B00111100,B00111100,B01100110,B01100110,B00000000};
  byte digY[8] = {B00000000,B01100110,B01100110,B01100110,B00111100,B00011000,B00011000,B00000000};
  byte digZ[8] = {B00000000,B01111110,B00001110,B00011100,B00111000,B01110000,B01111110,B00000000};
  byte digDOT[8] = {B00000000,B00000000,B00000000,B00000000,B00000000,B00011000,B00011000,B00000000};
  byte digEX[8] = {B00000000,B00011000,B00011000,B00011000,B00011000,B00000000,B00011000,B00000000};
  
  byte actByte[8] = {B10000001,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B10000001};


  if(image=="off") { copy(off,actByte,8); };
  if(image=="") { copy(off,actByte,8); };

  if(image=="skull") { copy(skull,actByte,8); };
  if(image=="lightning") { copy(lightning,actByte,8); };
  if(image=="stars") { copy(stars,actByte,8); };
  if(image=="umbrella") { copy(umbrella,actByte,8); };

  if(image=="pacman") { copy(pacman,actByte,8); };
  if(image=="pacghost") { copy(pacghost,actByte,8); };

  if(image=="emoji-smile") { copy(emojismile,actByte,8); };
  if(image=="emoji-worried") { copy(emojiworried,actByte,8); };
  if(image=="emoji-confused") { copy(emojiconfused,actByte,8); };
  if(image=="emoji-openmouth") { copy(emojiopenmouth,actByte,8); };

  if(image=="1") { copy(num1,actByte,8); };
  if(image=="2") { copy(num2,actByte,8); };
  if(image=="3") { copy(num3,actByte,8); };
  if(image=="4") { copy(num4,actByte,8); };
  if(image=="5") { copy(num5,actByte,8); };
  if(image=="6") { copy(num6,actByte,8); };
  if(image=="7") { copy(num7,actByte,8); };
  if(image=="8") { copy(num8,actByte,8); };
  if(image=="9") { copy(num9,actByte,8); };
  if(image=="0") { copy(num0,actByte,8); };

  if(image=="A") { copy(digA,actByte,8); };
  if(image=="B") { copy(digB,actByte,8); };
  if(image=="C") { copy(digC,actByte,8); };
  if(image=="D") { copy(digD,actByte,8); };
  if(image=="E") { copy(digE,actByte,8); };
  if(image=="F") { copy(digF,actByte,8); };
  if(image=="G") { copy(digG,actByte,8); };
  if(image=="H") { copy(digH,actByte,8); };
  if(image=="I") { copy(digI,actByte,8); };
  if(image=="J") { copy(digJ,actByte,8); };
  if(image=="K") { copy(digK,actByte,8); };
  if(image=="L") { copy(digL,actByte,8); };
  if(image=="M") { copy(digM,actByte,8); };
  if(image=="N") { copy(digN,actByte,8); };
  if(image=="O") { copy(digO,actByte,8); };
  if(image=="P") { copy(digP,actByte,8); };
  if(image=="Q") { copy(digQ,actByte,8); };
  if(image=="R") { copy(digR,actByte,8); };
  if(image=="S") { copy(digS,actByte,8); };
  if(image=="T") { copy(digT,actByte,8); };
  if(image=="U") { copy(digU,actByte,8); };
  if(image=="V") { copy(digV,actByte,8); };
  if(image=="W") { copy(digW,actByte,8); };
  if(image=="X") { copy(digX,actByte,8); };
  if(image=="Y") { copy(digY,actByte,8); };
  if(image=="Z") { copy(digZ,actByte,8); };
  if(image==".") { copy(digDOT,actByte,8); };
  if(image=="!") { copy(digEX,actByte,8); };


    if(ori==1){
      // ^
      for(int i=0;i<8;i++){
        byte newByte = actByte[i];
        if(inv==1){
          newByte = ((byte)~newByte); // Invert Byte
        }
        lc.setRow(display,i,newByte);
      }
    }
    if(ori==2){
      // >
      int counter = 8;
      for(int i=0;i<8;i++){
        counter = counter - 1;
        byte newByte = actByte[i];
        if(inv==1){
          newByte = ((byte)~newByte); // Invert Byte
        }
        lc.setColumn(display,counter,newByte);
      }
    }
    if(ori==3){
      // v
      int counter = 8;
      for(int i=0;i<8;i++){
        counter = counter - 1;
        byte oldByte = actByte[i];
        byte newByte = ((oldByte&0x01)<<7)|((oldByte&0x02)<<5)|((oldByte&0x04)<<3)|((oldByte&0x08)<<1)|((oldByte&0x10)>>1)|((oldByte&0x20)>>3)|((oldByte&0x40)>>5)|((oldByte&0x80)>>7); // Reverse Byte
        if(inv==1){
          newByte = ((byte)~newByte); // Invert Byte
        }
        lc.setRow(display,counter,newByte);
      }
    }
    if(ori==4){
      // <
      for(int i=0;i<8;i++){
        byte oldByte = actByte[i];
        byte newByte = ((oldByte&0x01)<<7)|((oldByte&0x02)<<5)|((oldByte&0x04)<<3)|((oldByte&0x08)<<1)|((oldByte&0x10)>>1)|((oldByte&0x20)>>3)|((oldByte&0x40)>>5)|((oldByte&0x80)>>7); // Reverse Byte
        if(inv==1){
          newByte = ((byte)~newByte); // Invert Byte
        }
        lc.setColumn(display,i,newByte);
      }
    }
    
    delay(delaytime);
    
}

void copy(byte* src, byte* dst, int len) {
    memcpy(dst, src, sizeof(src[0])*len);
}
