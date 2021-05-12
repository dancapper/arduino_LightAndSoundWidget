Light and Sound widget with DFPlayer Mini

Dan Capper <dan@ub3r.geek.nz>

13 May 2021

Plays a random .mp3 from the SD card and lights an LED when a button is pressed. Based on example code from the DFPlayer Mini MP3 library by Makuna.

SOFTWARE REQUIREMENTS:

* Arduino IDE, or whatever is your preferred flavour
* DFPlayer Mini MP3 Library by Makuna
* LowPower_LowPowerLab Library by LowPowerLab

HARDWARE REQUIREMENTS:

* DFPlayer Mini module
* Arduino or clone (Most anything should work; I used a Leonardo/Micro clone)
* Momentary Switch
* Power source (3 x AA batteries in series, making 4.5v, worked for me. The DFPlayer Mini accepts 3.2-5.5v)
* Wire, resistors, etc, as detailed under "WIRING"
* Some kind of enclosure probably (I used a dollar store battery powered push button light)
* Optional: LED(s), NPN transistor (for driving higher power/multiple LEDs), Main power switch

WIRING:

DFPlayer Mini:

* TX via 1k resistor to Arduino pin 14
* RX via 1k resistor to Arduino pin 15
* VCC to Arduino VCC (or battery+)
* GND to Arduino GND
* SPK+/SPK- to speaker

Arduino:

* VCC (or battery+) to VCC
* GND to GND (or use USB for power)
* Momentary switch between pin 2 and GND

LED optional:

* For a single LED, Pin 3 to 330 ohm to LED+, GND to LED-

For more LEDs in parallel or high power LEDs, use an NPN transistor:

* Pin 3 to 1k to Base, GND to Emitter, LED- to Collector, LED+ to 330 ohm (or as suits) to VCC

A NOTE ON BATTERY LIFE:

The DFPlayer Mini still draws around 20mA in "Sleep" mode, based on my testing (at 4.5v).
I suggest incorporating a physical power switch if you want acceptable battery life in a portable device.

One could try a low-gate drive MOSFET attached to an output pin which is HIGH only when the Arduino
isn't sleeping, and use that to switch the DFPlayer off completely. This is left as an exercise
for the reader. I imagine there would be a need to initialise it again, and mine at least emits a small 
noise when powered on. For my intended purpose, the additional effort did not seem worthwhile.
