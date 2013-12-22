from mpd import MPDClient

mop = MPDClient()
mop.connect( "localhost" , 6600 )

playlists = map( lambda pl: pl[ "playlist" ], mop.listplaylists() )

mop.load( playlists[ 0 ] )
mop.play( 0 )
