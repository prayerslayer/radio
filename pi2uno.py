# arduino serial connection
import serial

# regexes
import re

# mpd library
from mpd import MPDClient

# sleep function
from time import sleep

# ==== UTILS =====

NMBR_REGEX  = re.compile( "\D*(\d{1,3})" ) # anything but a digit with 1 to 3 digits at the end

# ==== SETUP =====

print "Pi 2 Uno connection establishing..."

# connect to arduino uno
uno = serial.Serial( "/dev/ttyACM0", 9600, timeout=1 )

# wait 18 sec
sleep( 18 )

# connect to mpd
mop = MPDClient()
mop.connect( "localhost", 6600 ) # default port

# play first playlist
currentPlaylist = 0
playlists = map( lambda pl: pl[ "playlist" ], mop.listplaylists() )
mop.load( playlists[ currentPlaylist] )

# tell arduino we're ready
uno.write( "CMD RUN\n\r" )

sleep( 12 )
mop.play( 0 )

# ==== FUNCTIONS ====

def processCommand( input ):
	# if command doesn't start with CMD return
	if not( input.startswith( "CMD" ) ):
		return
	# check if it's SET VOLUME
	if input.startswith( "CMD SET VOLUME" ):
		# match number regex on command
		vol_result = NMBR_REGEX.findall( input )
		# get first match,convert to int
		vol = int( vol_result[ 0 ] )
		# update mpd
		mop.setvol( vol )
		print "DEBUG set volume to " + str( vol )

# ==== LOOP ====

print "Pi 2 Uno loop running..."

# there really is no keyword for true in python 2
command = "noop"

while 1:
	# read serial from arduino
	command = uno.readline()
	#print "DEBUG: " + command
	# repeat if there is no command
	if len( command ) > 0:
		processCommand( command )
