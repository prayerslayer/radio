#! /bin/sh

# start mopidy server
nohup mopidy &

# start connection between mopidy and  arduino
nohup python ~/Sites/radio/pi2uno.py &
