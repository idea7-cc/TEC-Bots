// For TEC Bot 7.0 with grbl4bots loaded on the Nano V3 co-processor
// Grbl settings changed from default as follows
// $100=32 -- For the x axis (left motor) set to 1/16th micro stepping, this makes 100 steps equal to 1 full rotation of the wheel
// $101=32 -- Same, but for the Y axis (right motor)
// $110=63000 -- Fastest stable tested speed for 1/16th micro stepping.  Can go faster by selecting different micro stepping (hardware switch)
// $111=63000 -- Same for Y
// $120=980 -- Acceleratin that reliably prevents skipping steps
// $121=980 -- Same for Y
// 1/16th micro stepping is selected on the blue hardware switch by switching the 3rd switch on. (Off, Off, On)
// For faster speeds, you could select 1/8 microstepping (On, On, Off) but then it's possible to skip steps from too much acceleration or speed
// To use 1/8th microstepping more reliably, the config.h file in the grbl4bots library can be edited to change "DEFAULTS_TB7_16" to "DEFAULTS_TB7_8"
// For most, just stick with 1/16th microstepping!

// This program sends instructions to grbl4bots and is set with enough time between commands so that grbl doesn't get behind.
// It is also possible to send much smaller step sizes in a loop.  When grbl gets two back to back travel commands, it smoothly
// accellerates to the new command...so smooth driving can be achieved easily instead of start stop start stop.  You can use this to
// travel continuously at a constant speed.  Sending too many commands to fast can crash the buffer and cause erratic behavior.
// This will be changed in grbl4bots eventually. (Or you could fix it for us!)

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
  timeSinceLastTurn=millis()-timeOfLastTurn; //calculate time since last turn (or time since start)

  if(timeSinceLastTurn>turnInterval) //if it's time to turn again
  {
    Serial2.print("\r\n\r\n"); //wakes up grbl
    delay(2);
    Serial2.print("$J=G21G91X");  //send move command to grbl
    Serial2.print(driveDistance+driveRotation);  //move left motor this far
    Serial2.print("Y");
    Serial2.print(driveDistance-driveRotation);  //move right motor this far
    Serial2.print("F");
    Serial2.print(forwardSpeed);  //move this fast (acceleration is automatically handled by grbl)
    driveRotation=-driveRotation;  //flip the direction
    timeOfLastTurn=millis();    //capture the time we did this turn so we can decide when to do the next one
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
