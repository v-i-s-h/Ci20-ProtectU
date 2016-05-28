
"""
 mjpg_streamer -i "/usr/local/lib/mjpg-streamer/input_uvc.so" -o "/usr/local/lib/mjpg-streamer/output_http.so -w /usr/local/share/mjpg-streamer/www"
"""

import time
import atexit
import serial
import subprocess
from flask import Flask, render_template, request

## BOT COMMAND DEFINITIONS
INIT_SEQ		= '\xF1' * 8
CMD_FORWARD		= '\x10'
CMD_BACKWARD	= '\x20'
CMD_STOP		= '\x30'
CMD_BRAKE		= '\x40'
CMD_RTLFT		= '\x50'
CMD_RTRGT		= '\x60'
CMD_PING		= '\xF1'

EXEC_DELAY		= 0.200	# Command execution delay

START_STREAM	= [ "mjpg_streamer",  "-i", "/usr/local/lib/mjpg-streamer/input_uvc.so", "-o", "/usr/local/lib/mjpg-streamer/output_http.so -w /usr/local/share/mjpg-streamer/www" ]

streamProc		= subprocess.Popen( START_STREAM )

bot	= serial.Serial( "/dev/ttyS0", 115200 );
bot.timeout = 1;

app = Flask(__name__)

# Cleanup any open objects
def cleanup():
	print "Cleaning up...."
	bot.close()
	streamProc.terminate()

# Load the main form template on webrequest for the root page
@app.route("/")
def root():
	# Create a template data dictionary to send any data to the template
	templateData = {
		'title' : 'Protect U'
		}
	# Pass the template data into the template picam.html and return it to the user
	return render_template( 'index.html', **templateData)

# The function below is executed when someone requests a URL with a move direction
@app.route("/<direction>")
def move(direction):
	# Choose the direction of the request
	if direction == 'LFT':
		print "COMMAND: LFT"
		bot.write( CMD_RTLFT );
		res	= bot.read( 1 );
		time.sleep( EXEC_DELAY );
		bot.write( CMD_STOP );	
		return 'LFT -> ' + str(res)
	elif direction == 'RGT':
		print "COMMAND: RGT"
		bot.write( CMD_RTRGT );
		res	= bot.read( 1 );
		time.sleep( EXEC_DELAY );
		bot.write( CMD_STOP );	
		return 'RGT -> ' + str(res)
	elif direction == 'FWD':
		print "COMMAND: FWD"
		bot.write( CMD_FORWARD );
		res	= bot.read( 1 );
		time.sleep( EXEC_DELAY );
		bot.write( CMD_STOP );	
		return 'FWD -> ' + str(res)
	elif direction == 'BCK':
		print "COMMAND: BCK"
		bot.write( CMD_BACKWARD );
		res	= bot.read( 1 );
		time.sleep( EXEC_DELAY );
		bot.write( CMD_STOP );	
		return 'BCK -> ' + str(res)

# Clean everything up when the app exits
atexit.register( cleanup )

if __name__ == "__main__":
	# Connect to bot
	print "Sending init sequence .... "
	bot.write( INIT_SEQ );
	res	= bot.read( 13 );
	print "RESPONSE: " + res

	app.run( host='0.0.0.0', port=5000 )