# libreLeds
#### Free Software and Hardware designs for controling led ligths.
libreLeds is a combination of firmware, control app and pcbs that allows you to build led_enabled gadgets. 
#### All libre for you!.


### Getting started:

 -Flash firmware on the esp8266 via arduino ide.

 -Flash extra data via:
 https://github.com/esp8266/arduino-esp8266fs-plugin

 -Install app on your phone, set it to host a wifi AP named "ledMaster" with pass "clubmate", also make sure it will be 192.168.43.1 (default on android).

 -Fire the app and turn on the esp8266, it should autoconnect to the app.
 
 -Configure Wifi and led Settings
 
 -Enjoy!
 
 
### Hardware compatible:

 -WS2812 (aka Neopixel) Strip & matrix(single & chained) through NeoPixel & NeoMatrix Library.

 -APA102 (aka DotStar)  Strip & matrix(single & chained) through FastLed Library
 
 -WS2801


### LedGadget
....

### Features:

 -Control through QT app(linux/android/mac/win).

 -Artnet support (DMX over wifi).

 -Music analizer with MSGEQ7 (vuLevel, spectrum, peak, bpms)

 -Color Effects.

 -Effect Playlist.

 -Http Updater.

 -Http file uploader.


### Known things:
 -Stop the animations before changing the led config, shit can happen otherwise.
 -In AP mode there is a memory leak and in some minutes you loose remote control.


### Setup environment:
-Compile firmware:
You will need latest arduino with esp8266 support from:
https://github.com/esp8266/Arduino
Fastled Library from:
https://github.com/FastLED/FastLED/releases
Neopixel, matrix and gfx from adafruit.
https://learn.adafruit.com/adafruit-arduino-ide-setup/arduino-1-dot-6-x-ide

the libraries included in the dependencies folder.

-Compile app
latest QT for the desktop app, also android sdk for compiling android app.


### Wifi Modes:
...Explain here...
