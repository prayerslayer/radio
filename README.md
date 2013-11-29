# Uno-Pi WiFi Radio

Raspberry Pi + Mopidy + Plugins (SoundCloud, Internet Radio) + Arduino.

## Todo

* Mopidy-SoundCloud extension may cause client to timeout because synchronous loading of tracks. Check later if problem persists without npmcpp.
* Fade volume in pi2uno at least a teeny bit - nicer to the ears
* Switch SC playlists or radio stations with potentiometer
* Persist state of radio (volume, last source+channel) anywhere
* Working automatic boot of radio (start mopidy+pi2uno)
* White noise between channels

## Plan

Pi runs Mopidy music server. Possibly also a REST API that connects to it, but that's left for later. Arduino has hardware controls attached to it. Arduino reads state of hardware. Arduino prints state to serial when appropriate. Python script on Pi reads serial from Arduino. Python script uses Python library for mpd connection. Python script translates serial input from Arduino to mpd commands. Everyone is happy.

## Parts

* 1x Raspberry Pi B
* 1x Wifi Dongle for Pi
* 1x Power for Pi
* 1x Arduino Uno
* 2x Potentiometer (for setting "frequency" and volume)
* 1x flip switch for on/off
* 2x LED (green/red)
* 2x wheels that turn potentiometers
* 1x 3-button-pop-out-switch-whatever 

## Reality

* install only pip 1.2.1 because SSL!
* sudo apt-get install mopidy because dependencies (first add mopidy repos to apt)
* sudo apt-get remove mopidy because pip is preferred because plugins
* sudo pip install Mopidy Mopidy-SoundCloud
* sudo apt-get install python-serial because Arduino