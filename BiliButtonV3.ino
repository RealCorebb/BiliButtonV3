#define NUMstrip 14
#define Digit1 0
#define Digit2 7
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
int DisplayNum = 0;
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
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.println("Gogogo!");
  


  Serial.println(" CONNECTED");
  
  
  strip.Begin();
  strip.SetBrightness(100);
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
//-------------------------------------------------------------------------------//
 //STARUP ANIMATION//
 /*
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<NUMstrip; i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / NUMstrip);
      // RgbColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.SetPixelColor(i, strip.gamma32(RgbColorHSV(pixelHue)));
    }
    strip.Show(); // Update strip with new contents
    delay(10);  // Pause for a moment
  }
  */
}

void DrawDigit(int offset, int r,int g,int b, int n)
{

  if (n == 2 || n == 3 || n == 4 || n == 5 || n == 6 || n == 8 || n == 9) //MIDDLE
  {
    strip.SetPixelColor(0 + offset, RgbColor(r, g, b));
  }
  else
  {
    strip.SetPixelColor(0 + offset, RgbColor(0, 0, 0));
  }
  if (n == 0 || n == 1 || n == 2 || n == 3 || n == 4 || n == 7 || n == 8 || n == 9) //TOP RIGHT
  {
    strip.SetPixelColor(6 + offset, RgbColor(r, g, b));
  }
  else
  {
    strip.SetPixelColor(6 + offset, RgbColor(0, 0, 0));
  }
  if (n == 0 || n == 2 || n == 3 || n == 5 || n == 6 || n == 7 || n == 8 || n == 9) //TOP
  {
    strip.SetPixelColor(5 + offset, RgbColor(r, g, b));
  }
  else
  {
    strip.SetPixelColor(5 + offset, RgbColor(0, 0, 0));
  }
  if (n == 0 || n == 4 || n == 5 || n == 6 || n == 8 || n == 9) //TOP LEFT
  {
    strip.SetPixelColor(4 + offset, RgbColor(r, g, b));
  }
  else
  {
    strip.SetPixelColor(4 + offset, RgbColor(0, 0, 0));
  }
  if (n == 0 || n == 2 || n == 6 || n == 8) //BOTTOM LEFT
  {
    strip.SetPixelColor(3 + offset, RgbColor(r, g, b));
  }
  else
  {
    strip.SetPixelColor(3 + offset, RgbColor(0, 0, 0));
  }
  if (n == 0 || n == 2 || n == 3 || n == 5 || n == 6 || n == 8 || n == 9) //BOTTOM
  {
    strip.SetPixelColor(2 + offset, RgbColor(r, g, b));
  }
  else
  {
    strip.SetPixelColor(2 + offset, RgbColor(0, 0, 0));
  }
  if (n == 0 || n == 1 || n == 3 || n == 4 || n == 5 || n == 6 || n == 7 || n == 8 || n == 9) //BOTTOM RIGHT
  {
    strip.SetPixelColor(1 + offset, RgbColor(r, g, b));
  }
  else
  {
    strip.SetPixelColor(1 + offset, RgbColor(0, 0, 0));
  }
 
}
/*

void RainbowDrawDigit(int offset, int pixelHue, int n)
{

  if (n == 2 || n == 3 || n == 4 || n == 5 || n == 6 || n == 8 || n == 9) //MIDDLE
  {
    strip.SetPixelColor(0 + offset, strip.gamma32(RgbColorHSV(pixelHue)));
  }
  else
  {
    strip.SetPixelColor(0 + offset, RgbColor(0, 0, 0));
  }
  if (n == 0 || n == 1 || n == 2 || n == 3 || n == 4 || n == 7 || n == 8 || n == 9) //TOP RIGHT
  {
    strip.SetPixelColor(6 + offset, strip.gamma32(RgbColorHSV(pixelHue)));
  }
  else
  {
    strip.SetPixelColor(6 + offset, RgbColor(0, 0, 0));
  }
  if (n == 0 || n == 2 || n == 3 || n == 5 || n == 6 || n == 7 || n == 8 || n == 9) //TOP
  {
    strip.SetPixelColor(5 + offset, strip.gamma32(RgbColorHSV(pixelHue)));
  }
  else
  {
    strip.SetPixelColor(5 + offset, RgbColor(0, 0, 0));
  }
  if (n == 0 || n == 4 || n == 5 || n == 6 || n == 8 || n == 9) //TOP LEFT
  {
    strip.SetPixelColor(4 + offset, strip.gamma32(RgbColorHSV(pixelHue)));
  }
  else
  {
    strip.SetPixelColor(4 + offset, RgbColor(0, 0, 0));
  }
  if (n == 0 || n == 2 || n == 6 || n == 8) //BOTTOM LEFT
  {
    strip.SetPixelColor(3 + offset, strip.gamma32(RgbColorHSV(pixelHue)));
  }
  else
  {
    strip.SetPixelColor(3 + offset, RgbColor(0, 0, 0));
  }
  if (n == 0 || n == 2 || n == 3 || n == 5 || n == 6 || n == 8 || n == 9) //BOTTOM
  {
    strip.SetPixelColor(2 + offset, strip.gamma32(RgbColorHSV(pixelHue)));
  }
  else
  {
    strip.SetPixelColor(2 + offset, RgbColor(0, 0, 0));
  }
  if (n == 0 || n == 1 || n == 3 || n == 4 || n == 5 || n == 6 || n == 7 || n == 8 || n == 9) //BOTTOM RIGHT
  {
    strip.SetPixelColor(1 + offset, strip.gamma32(RgbColorHSV(pixelHue)));
  }
  else
  {
    strip.SetPixelColor(1 + offset, RgbColor(0, 0, 0));
  }
  strip.Show();
}
*/

int period = 1000;
unsigned long time_now = 0;


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
        if  (DisplayNum>99) DisplayNum=0;
             DisplayNum++;
    }
  
  int Display1 = (DisplayNum/10)%10;
  int Display2 = (DisplayNum/1)%10;
           DrawDigit(Digit1,r,g,b,Display1);
           DrawDigit(Digit2,r,g,b,Display2);
   strip.Show();
}

