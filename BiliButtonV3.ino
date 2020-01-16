#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <DNSServer.h>
#include <HTTPClient.h>
WiFiManager wm;
#define NUMstrip 35
#define Digit1 0
#define Digit2 7
#define Digit3 14
#define Digit4 21
#define Digit5 28
#define PIN 23
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
//--------------------------//
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
//#include <Adafruit_NeoPixel.h>
#include <NeoPixelBrightnessBus.h>
NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> strip(NUMstrip, PIN);
//GLOBAL------------------//
int r=255;
int g=255;
int b=255;
int pixel[NUMstrip]={0};
int DisplayNum = 0;
uint32_t MODE_DIGIT=1;                //0 color picker          //1 rainbowloop             //2 RainbowSmooth
uint32_t MODE_LIGHTING=2;            //0 color picker                     //2 RainbowSmooth
int brightness=1;
int oldSubscriberCount=0;
int Anilooptime=0;
//LED--------SETUP-----------//
int LEDR=25;
int LEDG=26;
int LEDB=27;
uint32_t R=255;  
uint32_t G=255; 
uint32_t B=255;     
uint8_t LEDcolorhue = 0;
boolean LED_PICK_CHANGE=false;
const boolean invert = false; // set true if common anode, false if common cathode
int LED_brightness = 127;
//BLE------SETUP--------//
BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;
//BLE-----JOBS------------
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        String command = rxValue.c_str();
        command = command.substring(0,2);
    
        if(command == "!C"){
          r = (uint8_t)rxValue[2];
          g = (uint8_t)rxValue[3];
          b = (uint8_t)rxValue[4];
         
          Serial.printf("R: %d | G: %d | B: %d \n\r",r,g,b);
        }
        if(command == "!O"){
          R = (uint8_t)rxValue[2];
          
          G = (uint8_t)rxValue[3];
          B = (uint8_t)rxValue[4];
           LED_PICK_CHANGE=true;
          Serial.printf("LED_OwO R: %d | G: %d | B: %d \n\r",R,G,B);
        }
        if(command == "!M"){
          MODE_DIGIT=(uint8_t)rxValue[2];

          }
        if(command == "!L"){
          MODE_LIGHTING=(uint8_t)rxValue[2];
          }
      }
      Serial.println("out");
    }
};

//--------------------------------


void setup() {
  int looptime=0;
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.println("Gogogo!");
  

  
  
  strip.Begin();
  strip.SetBrightness(brightness);
//BLE//----------------------------------------
// Create the BLE Device
  BLEDevice::init("BiliButtonV3");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

   // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
                    CHARACTERISTIC_UUID_TX,
                    BLECharacteristic::PROPERTY_NOTIFY
                  );

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
                       CHARACTERISTIC_UUID_RX,
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");

 //STARUP ANIMATION//


  for(long firstPixelHue = 0; firstPixelHue < 360; firstPixelHue ++) {
    
    if (looptime==0){
        for(int i=0; i<NUMstrip; i++) { // For each pixel in strip...
         // Offset pixel hue by an amount to make one full revolution of the
          // color wheel (range of 65536) along the length of the strip
          // (strip.numPixels() steps):    
          int pixelHue = firstPixelHue + (i * 360L / NUMstrip);
          // RgbColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
          // optionally add saturation and value (brightness) (each 0 to 255).
         // Here we're using just the single-argument hue variant. The result
         // is passed through strip.gamma32() to provide 'truer' colors
         // before assigning to each pixel:
         strip.SetPixelColor(i, HslColor(pixelHue/360.0f,1.0f,0.5f));
         strip.Show(); // Update strip with new contents
         if(brightness<80){
            brightness+=1;
            strip.SetBrightness(brightness);
         }
       delay(10);  // Pause for a moment
    }
      //------------------------------WIFI---------------------------//
 WiFi.mode(WIFI_STA);   

    Serial.begin(115200);
    if(wm.autoConnect("BiliButtonV3")){
        Serial.println("connected...yeey :)");
    }
    else {
        Serial.println("Configportal running");
    }
//-------------------------------------------------------------------------------//
    }
    else{
        for(int i=0; i<NUMstrip; i++) { 
            int pixelHue = firstPixelHue + (i * 360L / NUMstrip);
            strip.SetPixelColor(i, HslColor(pixelHue/360.0f,1.0f,0.5f));
          }
        }
       if (looptime!=0){
        if(brightness<80){
            brightness+=1;
            strip.SetBrightness(brightness);
         }
        strip.Show();
        delay(10);
       }
       looptime++;
    }
 //LED------------------------------//
   ledcAttachPin(LEDR, 1); // assign RGB led pins to channels
  ledcAttachPin(LEDG, 2);
  ledcAttachPin(LEDB, 3);
   ledcSetup(1, 5000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(2, 5000, 8);
  ledcSetup(3, 5000, 8);   
  }


void DrawDigit(int offset, int r,int g,int b, int n)
{

  if (n == 2 || n == 3 || n == 4 || n == 5 || n == 6 || n == 8 || n == 9) //MIDDLE
  {
    pixel[0+offset]=1;
   // strip.SetPixelColor(0 + offset, RgbColor(r, g, b));
  }
  else
  {
    pixel[0+offset]=0;
   // strip.SetPixelColor(0 + offset, RgbColor(0, 0, 0));o
  }
  if (n == 0 || n == 1 || n == 2 || n == 3 || n == 4 || n == 7 || n == 8 || n == 9) //TOP RIGHT
  {
    pixel[6+offset]=1;
   // strip.SetPixelColor(6 + offset, RgbColor(r, g, b));
  }
  else
  {
    pixel[6+offset]=0;
 //   strip.SetPixelColor(6 + offset, RgbColor(0, 0, 0));
  }
  if (n == 0 || n == 2 || n == 3 || n == 5 || n == 6 || n == 7 || n == 8 || n == 9) //TOP
  {
    pixel[5+offset]=1;
  //  strip.SetPixelColor(5 + offset, RgbColor(r, g, b));
  }
  else
  {
    pixel[5+offset]=0;
 //   strip.SetPixelColor(5 + offset, RgbColor(0, 0, 0));
  }
  if (n == 0 || n == 4 || n == 5 || n == 6 || n == 8 || n == 9) //TOP LEFT
  {
    pixel[4+offset]=1;
  //  strip.SetPixelColor(4 + offset, RgbColor(r, g, b));
  }
  else
  {
    pixel[4+offset]=0;
 //   strip.SetPixelColor(4 + offset, RgbColor(0, 0, 0));
  }
  if (n == 0 || n == 2 || n == 6 || n == 8) //BOTTOM LEFT
  {
    pixel[3+offset]=1;
 //   strip.SetPixelColor(3 + offset, RgbColor(r, g, b));
  }
  else
  {
    pixel[3+offset]=0;
   // strip.SetPixelColor(3 + offset, RgbColor(0, 0, 0));
  }
  if (n == 0 || n == 2 || n == 3 || n == 5 || n == 6 || n == 8 || n == 9) //BOTTOM
  {
    pixel[2+offset]=1;
    //strip.SetPixelColor(2 + offset, RgbColor(r, g, b));
  }
  else
  {
    pixel[2+offset]=0;
   // strip.SetPixelColor(2 + offset, RgbColor(0, 0, 0));
  }
  if (n == 0 || n == 1 || n == 3 || n == 4 || n == 5 || n == 6 || n == 7 || n == 8 || n == 9) //BOTTOM RIGHT
  {
    pixel[1+offset]=1;
   // strip.SetPixelColor(1 + offset, RgbColor(r, g, b));
  }
  else
  {
    pixel[1+offset]=0;
    //strip.SetPixelColor(1 + offset, RgbColor(0, 0, 0));
  }
 
}

//TIMING SETTINGS//
int period = 600;
unsigned long time_now = 0;
int Rainbowperiod = 20;
unsigned long Rainbowtime_now = 0;
int BLEdelay = 10;
unsigned long BLEdelay_now = 0;
int BLEDisconnected = 500;
unsigned long BLEDisconnected_now = 0;
int Rainbowperiod_LED = 20;
unsigned long Rainbowtime_LED_now = 0;
int animation = 10;
unsigned long animation_now = 0;
//////////


long firstPixelHue = 0;
int changing=0;
void loop() {
  wm.process();
    if (deviceConnected) {
      if(millis() > BLEdelay_now + BLEdelay){
                        BLEdelay_now = millis();
        pTxCharacteristic->setValue(&txValue, 1);
        pTxCharacteristic->notify();
        txValue++;
      }
  }


    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
            if(millis() > BLEDisconnected_now + BLEDisconnected){
                        BLEDisconnected_now = millis();
        //delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
            }
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
  
  if(millis() > time_now + period){
        time_now = millis();

        HTTPClient http;  //Declare an object of class HTTPClient
 
    http.begin("http://api.bilibili.com/x/relation/stat?vmid=13646693");  //Specify request destination
    int httpCode = http.GET();                                                                  //Send the request
 
    if (httpCode == 200) { //Check the returning code
 
      String payload = http.getString();   //Get the request response payload
      String subs=payload.substring(payload.indexOf("\"follower\":")+11,payload.indexOf("}}"));
      int subscriberCount=subs.toInt();
      //Serial.println(subscriberCount);
        if (subscriberCount!=oldSubscriberCount){
        changing=1;
        Serial.println(changing);
        }
      oldSubscriberCount=subscriberCount;
      http.end();  
      DisplayNum=subscriberCount;
  }  
        /*
        if  (DisplayNum>99999) DisplayNum=0;
             DisplayNum++;
             */
             if(DisplayNum<100){
                   int Display1 = (DisplayNum/10)%10;
                    int Display2 = (DisplayNum/1)%10;
                    DrawDigit(Digit3,r,g,b,Display1);
                     DrawDigit(Digit4,r,g,b,Display2);
              }
              else if(DisplayNum<1000){
                int Display1 = (DisplayNum/100)%10;
                  int Display2 = (DisplayNum/10)%10;
                    int Display3 = (DisplayNum/1)%10;
                               DrawDigit(Digit2,r,g,b,Display1);
                                DrawDigit(Digit3,r,g,b,Display2);
                                 DrawDigit(Digit4,r,g,b,Display3);
                }
                else if (DisplayNum<10000){
                 int Display1 = (DisplayNum/1000)%10;
                 int Display2 = (DisplayNum/100)%10;
                  int Display3 = (DisplayNum/10)%10;
                    int Display4 = (DisplayNum/1)%10;
                      DrawDigit(Digit2,r,g,b,Display1);
                              DrawDigit(Digit3,r,g,b,Display2);
                                 DrawDigit(Digit4,r,g,b,Display3);
                                    DrawDigit(Digit5,r,g,b,Display4);
                  }
             else if(DisplayNum<100000){
               int Display1 = (DisplayNum/10000)%10;
                int Display2 = (DisplayNum/1000)%10;
                 int Display3 = (DisplayNum/100)%10;
                  int Display4 = (DisplayNum/10)%10;
                    int Display5 = (DisplayNum/1)%10;
           DrawDigit(Digit1,r,g,b,Display1);
           DrawDigit(Digit2,r,g,b,Display2);
           DrawDigit(Digit3,r,g,b,Display3);
           DrawDigit(Digit4,r,g,b,Display4);
           DrawDigit(Digit5,r,g,b,Display5);
             }
  }
           switch(MODE_DIGIT){
            case 0:
                if(changing==0){
                 for (int i=0;i<NUMstrip;i++){
                  if (pixel[i]==1){
                   strip.SetPixelColor(i, RgbColor(r, g, b));
                   }
                  else {
                    strip.SetPixelColor(i,RgbColor(0, 0, 0));
                 }
                 }
                }
                if(changing==1){
                        for (int i=0;i<2;i++){
                        int count=0;
                        int firstPixelHue=random(0,360);
                        int anii=NUMstrip;
                        if (Anilooptime==0){                
                           if(millis() > animation_now + animation){
                                animation_now=millis();
                               if(anii>0) anii--;
                               int pixelHue = firstPixelHue + (count * 360L / NUMstrip);
                                  if (pixel[i]==1){
                                        count++;
                                       strip.SetPixelColor(i, HslColor(pixelHue/360.0f,1.0f,0.5f));
                                  }
                                  else{
                                      strip.SetPixelColor(i, RgbColor(0,0,0));
                                  }
                                  strip.Show();
                                  firstPixelHue++;
                                  if(firstPixelHue>360) firstPixelHue==0;                             
                                 // delay(10);
                              }
                        }
                        else{
                          for(int i=NUMstrip; i>0; i--) { 
                            if (pixel[i]==1)
                                 strip.SetPixelColor(i, RgbColor(r,g,b));      
                            else
                                strip.SetPixelColor(i, RgbColor(0,0,0));   

                          strip.Show();
                          delay(10);
                          }
                          Anilooptime=0;
                          changing=0;
                          Serial.println(changing);
                          break;              
                        }
                              Anilooptime++;
                        }
                }
                                 break;
                 
            case 1:
                      if(millis() > Rainbowtime_now + Rainbowperiod){
                        int count=0;
                        Rainbowtime_now = millis();
                      if(firstPixelHue < 360){
                          firstPixelHue ++;                       
                           for(int i=0; i<NUMstrip; i++) { 
                               int pixelHue = firstPixelHue + (count * 360L / NUMstrip);
                                  if (pixel[i]==1){
                                        count++;
                                       strip.SetPixelColor(i, HslColor(pixelHue/360.0f,1.0f,0.5f));
                                  }
                                  else{
                                      strip.SetPixelColor(i, RgbColor(0,0,0));
                                  }
                              }
                            }     
                            else firstPixelHue=0;
                           }          
                 break;
             case 2:
                    if(millis() > Rainbowtime_LED_now + Rainbowperiod_LED){
                        Rainbowtime_LED_now = millis();
                        if(firstPixelHue < 360){
                          firstPixelHue ++;  
                    for(int i=0; i<NUMstrip; i++) { 
                               int pixelHue = firstPixelHue + (360L / NUMstrip);
                                  if (pixel[i]==1){
                                       strip.SetPixelColor(i, HslColor(pixelHue/360.0f,1.0f,0.5f));
                                  }
                                  else{
                                      strip.SetPixelColor(i, RgbColor(0,0,0));
                                  }
                              }           
                           }    
                           else firstPixelHue=0;
                    }   
                    break;                   
           }
   strip.Show();
           switch(MODE_LIGHTING){
              case 0:
                     if(LED_PICK_CHANGE){
                     ledcWrite(1, R); // write red component to channel 1, etc.
                      ledcWrite(2, G);   
                        ledcWrite(3, B); 
                        Serial.printf("Just changed LED Color");
                        LED_PICK_CHANGE=false;
                        
                     }
                        break;
              case 2:
                     if(millis() > Rainbowtime_LED_now + Rainbowperiod_LED){
                      Rainbowtime_LED_now = millis();
                      hueToRGB(LEDcolorhue, LED_brightness);
                      ledcWrite(1, R); // write red component to channel 1, etc.
                      ledcWrite(2, G);   
                        ledcWrite(3, B); 
                        LEDcolorhue++;
                      if (LEDcolorhue>255){
                        LEDcolorhue=0;
                      }
                     }
                     break;
                    } 
}
void hueToRGB(uint8_t hue, uint8_t brightness)
{
    uint16_t scaledHue = (hue * 6);
    uint8_t segment = scaledHue / 256; // segment 0 to 5 around the
                                            // color wheel
    uint16_t segmentOffset =
      scaledHue - (segment * 256); // position within the segment

    uint8_t complement = 0;
    uint16_t prev = (brightness * ( 255 -  segmentOffset)) / 256;
    uint16_t next = (brightness *  segmentOffset) / 256;

    if(invert)
    {
      brightness = 255 - brightness;
      complement = 255;
      prev = 255 - prev;
      next = 255 - next;
    }

    switch(segment ) {
    case 0:      // red
        R = brightness;
        G = next;
        B = complement;
    break;
    case 1:     // yellow
        R = prev;
        G = brightness;
        B = complement;
    break;
    case 2:     // green
        R = complement;
        G = brightness;
        B = next;
    break;
    case 3:    // cyan
        R = complement;
        G = prev;
        B = brightness;
    break;
    case 4:    // blue
        R = next;
        G = complement;
        B = brightness;
    break;
   case 5:      // magenta
    default:
        R = brightness;
        G = complement;
        B = prev;
    break;
    }
}
