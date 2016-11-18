# libreLeds
#### Free Software and Hardware designs for controling led ligths.
libreLeds is a combination of firmware, control app and pcbs that allows you to build led_enabled gadgets. 
#### All libre for you!.


### Getting started:

 -Flash firmware on the esp8266 via arduino ide.

 -Flash extra data.

 -Install app on your phone, set it to host a wifi AP named "ledMaster" with pass "clubmate", also make sure it will be 192.168.43.1 (default on android).

 -Fire the app and turn on the esp8266, it should autoconnect to the app.
 
 -Configure Wifi and led Settings
 
 -Profit!
 
 
### Hardware compatible:

 -WS2812 (aka Neopixel) Strip & matrix(single & chained) through NeoPixel & NeoMatrix Library.

 -APA102 (aka DotStar)  Strip & matrix(single & chained) through FastLed Library


### LedGadget
 ...Explain here...

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


### Setup environment:
You will need latest arduino with esp8266 support, latest QT, and some weird shit I'll explain someday.


### Wifi Modes:
...Explain here...
