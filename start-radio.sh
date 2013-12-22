#! /bin/sh

# start mopidy server
nohup mopidy -v

# start connection between mopidy and  arduino
nohup python ~/Sites/radio/py2uno.py
