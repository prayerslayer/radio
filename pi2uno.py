import serial

# connect to arduino uno
uno = serial.Serial( "/dev/ttyACM0", 9600 )

# there really is no keyword for true in python 2
# and this sentence rhymes.
while 1 : 
    print uno.readline()
