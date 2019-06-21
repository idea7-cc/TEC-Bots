#include <M5Stack.h>
#include "FastLED.h"

#define Neopixel_PIN 1
#define NUM_LEDS 7

CRGB leds[NUM_LEDS];
uint8_t gHue = 0;
static TaskHandle_t FastLEDshowTaskHandle = 0;
static TaskHandle_t userTaskHandle = 0;

int dashSpeed = 500; //resulting speed depends on microstep selection on board
int backingSpeed = 150;
int driveDistance = 7; //positive numbers are FWD.
int driveRotation = 0;  //positive numbers are CW
unsigned long timeLastBackedUp = 0;
int timeSinceBackedUp = 0;

int backUpInterval = 10500;
bool readyToDashForward = 0;


void setup() 
{
  M5.begin();
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(CYAN); M5.Lcd.setTextSize(2); M5.Lcd.setCursor(40, 0);
  M5.Lcd.println("Colorful Back-N-Dash");
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(0, 25);
  M5.Lcd.println();
  M5.Lcd.println("- Display rainbow effect.");
  M5.Lcd.println();
  M5.Lcd.println("- Back up 7 revolutions.");
  M5.Lcd.println();
  M5.Lcd.println("- Dash FWD 7 revolutions.");
  M5.Lcd.println();
  M5.Lcd.println("- Repeat.");

  // Neopixel initialization
  FastLED.addLeds<WS2811,Neopixel_PIN,GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(10);
  xTaskCreatePinnedToCore(FastLEDshowTask, "FastLEDshowTask", 2048, NULL, 2, NULL, 1);

  // Serial2.begin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, bool invert)
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  
  //give grbl time to start up on Nano
  delay(4000); 
}

void loop() 
{
  timeSinceBackedUp=millis()-timeLastBackedUp;

  if(timeSinceBackedUp>backUpInterval)
  {
    if(readyToDashForward)
    {
      Serial2.print("\r\n\r\n");
      delay(2);
      Serial2.print("$J=G21G91X");
      Serial2.print(driveDistance+driveRotation);
      Serial2.print("Y");
      Serial2.print(driveDistance-driveRotation);
      Serial2.print("F");
      Serial2.print(dashSpeed);
      readyToDashForward=0;    
    }
    else
    {
      timeLastBackedUp=millis();
      readyToDashForward=1;
      Serial2.print("\r\n\r\n");
      delay(2);
      Serial2.print("$J=G21G91X");
      Serial2.print(-driveDistance-driveRotation);
      Serial2.print("Y");
      Serial2.print(-driveDistance+driveRotation);
      Serial2.print("F");
      Serial2.print(backingSpeed);
      
    }
  }
}  

void FastLEDshowESP32()
{
    if (userTaskHandle == 0) {
        userTaskHandle = xTaskGetCurrentTaskHandle();
        xTaskNotifyGive(FastLEDshowTaskHandle);
        const TickType_t xMaxBlockTime = pdMS_TO_TICKS( 200 );
        ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
        userTaskHandle = 0;
    }
}

void FastLEDshowTask(void *pvParameters)
{
    for(;;) {
        fill_rainbow(leds, NUM_LEDS, gHue, 7);// rainbow effect
        FastLED.show();// must be executed for neopixel becoming effective
        EVERY_N_MILLISECONDS( 20 ) { gHue++; }
    }
}
