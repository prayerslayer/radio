# Uno-Pi WiFi Radio

Raspberry Pi + Mopidy + Plugins (SoundCloud, Internet Radio) + Arduino.

# Setup

## Hardware

tba

## Software

0. Install necessary software on Pi (see section "Dependencies")
1. Connect Arduino and Pi via USB interface
2. Deploy Arduino Code on Arduino, modify pins as needed
3. Create folder ``~/.radio`` on Pi
4. Put pi2uno.py there, modify Mopidy or Arduino ports as needed
5. Run ``start-radio.sh`` on Pi after every boot
6. Profit

# Todo

* Persist state of radio (volume, last source+channel) somewhere
* Load persisted state at radio start
* Working automatic boot of radio (start mopidy+pi2uno)
* Mopidy-SoundCloud extension may cause client to timeout because synchronous loading of tracks. Check later if problem persists without npmcpp.
* Fade volume in pi2uno at least a teeny bit — nicer to the ears
* Switch sources and channels
* White noise between channels
* Arduino needs quite long to get out of WAIT status. This is because it can only read 1 char/loop from serial and 2 seconds/loop are used for blinking the status LED. Equals to 12 seconds (6 chars * 2 seconds/loop).

## Plan

Pi runs Mopidy music server. Possibly also a REST API that connects to it, but that's left for later. Arduino has hardware controls attached to it. Arduino reads state of hardware. Arduino prints state to serial when appropriate. Python script on Pi reads serial from Arduino. Python script uses Python library for mpd connection. Python script translates serial input from Arduino to mpd commands. Everyone is happy.

## Parts

* 1x Raspberry Pi B
* 1x Wifi Dongle for Pi
* 1x Power for Pi
* 1x Arduino Uno
* 2x **Rotary encoders** (for setting "frequency" and volume)
* 1x flip switch for on/off
* 2x LED (green/red)
* 2x wheels that turn potentiometers
* 1x 3-button-pop-out-switch-whatever 

# Dependencies

* install only pip 1.2.1 because SSL!
* ``sudo apt-get install mopidy`` because dependencies (first add mopidy repos to apt)
* ``sudo apt-get remove mopidy`` because pip is preferred because plugins
* ``sudo pip install Mopidy Mopidy-SoundCloud``
* ``sudo apt-get install python-serial`` because Arduino
* ``sudo pip install mopidy-radio-de``
* ``sudo pip install python-mpd2``