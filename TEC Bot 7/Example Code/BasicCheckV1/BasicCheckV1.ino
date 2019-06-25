// For TEC Bot 7.0 with grbl 1.1 loaded on the Nano V3 co-processor
// Grbl settings changed from default using the Universal Gcode sender as follows:
// $100=32 -- For the x axis (left motor) set to 1/16th micro stepping, this makes 100 steps equal to 1 full rotation of the wheel
// $101=32 -- Same, but for the Y axis (right motor)
// $110=64000 -- Fastest stable tested speed for 1/16th micro stepping.  Can go faster by selecting different micro stepping (hardware switch)
// $111=64000 -- Same for Y
// $120=1200 -- Acceleratin that reliably prevents skipping steps (turn down for 1/8 microstepping)
// $121=1200 -- Same for Y
// 1/16th micro stepping is selected on the blue hardware switch by switching the 3rd switch on. (Off, Off, On)
// For faster speeds, you could select 1/8 microstepping (On, On, Off) but be sure to turn down acceleration to somewhere around
// $120 and $121 = 980 or maybe 910.

// This program sends instructions to grbl and is set with enough time between commands so that grbl doesn't get behind.
// It is also possible to send much smaller step sizes in a loop.  When grbl gets two back to back travel commands, it smoothly
// accellerates to the new command...so smooth driving can be achieved easily instead of start stop start stop.  You can use this to
// travel continuously at a constant speed.

// Grbl also keeps the x and y axis coordinated so they start and stop at the same time.  This makes it easy to do smooth turns for example.

// The FastLED library is used instead of the Neopixel library because the Neopixel library conflicts with the m5Stack library somehow

// This example makes the robot turn back and forth.

#include <M5Stack.h>
#include "FastLED.h"

#define Neopixel_PIN 1
#define NUM_LEDS 7

CRGB leds[NUM_LEDS];
uint8_t gHue = 0;
static TaskHandle_t FastLEDshowTaskHandle = 0;
static TaskHandle_t userTaskHandle = 0;

int forwardSpeed = 21000; //medium
int backSpeed = 21000; //medium
int driveDistance = 0; //positive numbers are FWD. 100 = 1 full revolution
int driveRotation = 21;  //positive numbers are CW
unsigned long timeOfLastTurn = 0;
int timeSinceLastTurn = 0;
int turnInterval = 310;

void setup() 
{
  M5.begin();
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(CYAN); M5.Lcd.setTextSize(2); M5.Lcd.setCursor(40, 0);
  M5.Lcd.println("Robot Dance...");
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(0, 25);
  M5.Lcd.println();
  M5.Lcd.println("- Display rainbow effect.");
  M5.Lcd.println();
  M5.Lcd.println("- Turn back and forth.");
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
  timeSinceLastTurn=millis()-timeOfLastTurn;

  if(timeSinceLastTurn>turnInterval)
  {
    Serial2.print("\r\n\r\n");
    delay(2);
    Serial2.print("$J=G21G91X");
    Serial2.print(driveDistance+driveRotation);
    Serial2.print("Y");
    Serial2.print(driveDistance-driveRotation);
    Serial2.print("F");
    Serial2.print(forwardSpeed);
    driveRotation=-driveRotation;
    timeOfLastTurn=millis();    
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
