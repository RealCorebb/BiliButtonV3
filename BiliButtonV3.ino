
#include <WiFi.h>          

//needed for library
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>    
#include <HTTPClient.h>
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
int MODE=1;                //0 color picker          //1 rainbowloop             //2 RainbowSmooth
int brightness=1;
//-----------------------//

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
  


  Serial.println(" CONNECTED");
  
  
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
  //------------------------------WIFI---------------------------//
 WiFiManager wifiManager;
 wifiManager.autoConnect("BiliButton");
   
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
//-------------------------------------------------------------------------------//
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
//////////


long firstPixelHue = 0;
void loop() {
    if (deviceConnected) {
        pTxCharacteristic->setValue(&txValue, 1);
        pTxCharacteristic->notify();
        txValue++;
      delay(5); // bluetooth stack will go into congestion, if too many packets are sent
  }


    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        //delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
  
  if(millis() > time_now + period){
        time_now = millis();

        HTTPClient http;  //Declare an object of class HTTPClient
 
    http.begin("http://api.bilibili.com/x/relation/stat?vmid=625268");  //Specify request destination
    int httpCode = http.GET();                                                                  //Send the request
 
    if (httpCode == 200) { //Check the returning code
 
      String payload = http.getString();   //Get the request response payload
      String subs=payload.substring(payload.indexOf("\"follower\":")+11,payload.indexOf("}}"));
      int subscriberCount=subs.toInt();
      Serial.println(subscriberCount);
     
    
      http.end();  
      DisplayNum=subscriberCount;
  }  
        /*
        if  (DisplayNum>99999) DisplayNum=0;
             DisplayNum++;
             */
             
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
           switch(MODE){
            case 0:
                 for (int i=0;i<NUMstrip;i++){
                  if (pixel[i]==1){
                   strip.SetPixelColor(i, RgbColor(r, g, b));
                   }
                  else {
                    strip.SetPixelColor(i,RgbColor(0, 0, 0));
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
                    if(millis() > Rainbowtime_now + Rainbowperiod){
                        Rainbowtime_now = millis();
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
           }
 

   strip.Show();
}