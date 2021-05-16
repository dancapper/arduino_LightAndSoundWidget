// Light and Sound widget with DFPlayer Mini
//
// Dan Capper <dan@ub3r.geek.nz>
// 13 May 2021
//
// Plays a random .mp3 from the SD card and lights an LED when a button is pressed
// Based on example code from the DFPlayer Mini MP3 library by Makuna
//
// SOFTWARE REQUIREMENTS:
//
// Arduino IDE, or whatever is your preferred flavour
// DFPlayer Mini MP3 Library by Makuna
// LowPower_LowPowerLab Library by LowPowerLab
//
// HARDWARE REQUIREMENTS:
//
// * DFPlayer Mini module
// * MicroSD Card (loaded with your choice of mp3 files)
// * Arduino or clone (Most anything should work; I used a Leonardo/Micro clone)
// * Momentary Switch
// * Power source (3 x AA batteries in series, making 4.5v, worked for me. The DFPlayer Mini accepts 3.2-5.5v)
// * Wire, resistors, etc, as detailed under "WIRING"
// * Some kind of enclosure probably (I used a dollar store battery powered push button light)
// * Optional: LED(s), NPN transistor (for driving higher power/multiple LEDs), Main power switch
//
// WIRING:
//
// DFPlayer Mini:
//
// TX via 1k resistor to Arduino pin 14
// RX via 1k resistor to Arduino pin 15
// VCC to Arduino VCC (or battery+)
// GND to Arduino GND
// SPK+/SPK- to speaker
//
// Arduino:
//
// VCC to VCC or Battery+
// GND to GND
// OR use USB for power
// Momentary switch between pin 2 and GND
// LED optional:
// For a single LED, Pin 3 to 330 ohm to LED+, GND to LED-
// For more LEDs in parallel or high power LEDs, use an NPN transistor:
// Pin 3 to 1k to Base, GND to Emitter, LED- to Collector, LED+ to 330 ohm (or as suits) to VCC
//
// A NOTE ON BATTERY LIFE:
//
// The DFPlayer Mini still draws around 20mA in "Sleep" mode, based on my testing (at 4.5v).
// I suggest incorporating a physical power switch if you want acceptable battery life in a portable device.
//
// One could try a low-gate drive MOSFET attached to an output pin which is HIGH only when the Arduino
// isn't sleeping, and use that to switch the DFPlayer off completely. This is left as an exercise
// for the reader. I imagine there would be a need to initialise it again, and mine at least emits a small 
// noise when powered on. For my intended purpose, the additional effort did not seem worthwhile.
//
// Many arduino and clone boards include a "power on" LED. Deleting this will help battery life some.
//
// KNOWN LIMITATIONS:
//
// Because MP3 playback happens asynchronously, the code will not wait for MP3 playback to finish before sleeping.
//
// Either set SLEEP_TIMER longer than your longest mp3 file, or fix it if you feel so inclined. OnPlayFinished()
// might be useful here.

#include <LowPower.h>
#include <SoftwareSerial.h>
#include <DFMiniMp3.h>

#define SLEEP_TIMER 45000      // this is how long the LED will stay on in milliseconds; default is 45 seconds
#define SET_VOLUME  25         // volume to set DFPlayer to; 0-30
#define SW_SER_RX   14         // Software serial RX (connect to TX on DFPlayer Mini)
#define SW_SER_TX   15         // Software serial TX (connect to RX on DFPlayer Mini)
#define BUTTON      2          // Pushbutton interrupt/wake pin - recommend pin 2 for most arduinos
#define LED         3          // LED pin; HIGH = ON

// implement a notification class,
// its member methods will get called 
//
class Mp3Notify
{
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
  {
    if (source & DfMp3_PlaySources_Sd) 
    {
        Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) 
    {
        Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) 
    {
        Serial.print("Flash, ");
    }
    Serial.println(action);
  }
  static void OnError(uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished(DfMp3_PlaySources source, uint16_t track)
  {
    Serial.print("Play finished for #");
    Serial.println(track);  
  }
  static void OnPlaySourceOnline(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "removed");
  }
};

// instance a DFMiniMp3 object, 
// defined with the above notification class and the hardware serial class
//
//DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial1);

// Some arduino boards only have one hardware serial port, so a software serial port is needed instead.
// comment out the above definition and uncomment these lines

SoftwareSerial secondarySerial(SW_SER_RX, SW_SER_TX); // RX, TX
DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(secondarySerial);

uint16_t count;
long track;
boolean play;
unsigned long sleeptime;

void setup() 
{
  Serial.begin(115200);
  Serial.println("initializing...");

  pinMode(BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON), playrandom, FALLING);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  randomSeed(analogRead(0));
  
  mp3.begin();
  mp3.reset(); 
  
  // show some properties and set the volume
  uint16_t volume = mp3.getVolume();
  Serial.print("volume ");
    Serial.println(volume);
  mp3.setVolume(SET_VOLUME);
  
  count = mp3.getTotalTrackCount(DfMp3_PlaySource_Sd);
  Serial.print("files ");
    Serial.println(count);
  mp3.setRepeatPlayAllInRoot(false);
  mp3.setRepeatPlayCurrentTrack(false);
  
  uint16_t mode = mp3.getPlaybackMode();
  Serial.print("playback mode ");
    Serial.println(mode);
    
  Serial.println("starting...");
  sleeptime = millis();
}

void loop() 
{
  // calling mp3.loop() periodically allows for notifications 
  // to be handled without interrupts
  if(play) {
        mp3.setPlaybackSource(DfMp3_PlaySource_Sd);
        track = random(1, count);
        Serial.print("Playing ");
          Serial.println(track);
        mp3.playGlobalTrack(track);
        digitalWrite(LED, HIGH);
        play = false;
        sleeptime = millis();
  } else if ((millis() - sleeptime) >= SLEEP_TIMER) {
        Serial.println("Sleep...");
        mp3.sleep();
        digitalWrite(LED, LOW);
        LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }
  mp3.loop();
}

void playrandom()
{
  play = true;
}
