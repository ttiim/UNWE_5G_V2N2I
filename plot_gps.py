
# Author: Timothy Wriglesworth
# University of British Columbia, 
# Radio Science Lab

# Requirements:
# Lat and Longitude
# Get the heading in a format like "North", "West", "East", "South",

# from http://aprs.gids.nl/nmea/
   # $GPBOD - Bearing, origin to destination
   # $GPBWC - Bearing and distance to waypoint, great circle
   # $GPGGA - Global Positioning System Fix Data
   # $GPGLL - Geographic position, latitude / longitude
   # $GPGSA - GPS DOP and active satellites 
   # $GPGSV - GPS Satellites in view
   # $GPHDT - Heading, True
   # $GPR00 - List of waypoints in currently active route
   # $GPRMA - Recommended minimum specific Loran-C data
   # $GPRMB - Recommended minimum navigation info
   # $GPRMC - Recommended minimum specific GPS/Transit data
   # $GPRTE - Routes
   # $GPTRF - Transit Fix Data
   # $GPSTN - Multiple Data ID
   # $GPVBW - Dual Ground / Water Speed
   # $GPVTG - Track made good and ground speed
   # $GPWPL - Waypoint location
   # $GPXTE - Cross-track error, Measured
   # $GPZDA - Date & Time


#ublox uses GNPGGA instead of GPNGGA
#run this program along gps_process.sh


#x is a longitude
#y is a latitude
import shutil, sys  
#from pynmea import nmea
import matplotlib.pyplot as plt
import pynmea2
from time import sleep
import numpy as np




TRX = -123.228546          #top right longitude
TRY = 49.279441            #top right latitude
BLX = -123.263335          #bottom left longitude
BLY = 49.248726             #bottom left latitude



long_min =-123.25661  #TRX
long_max = -123.25470
lat_min= 49.26383  #BLY
lat_max = 49.26257 #TRY


#format of gps data in gps_data.txt 
#$GNGGA,003343.00,4915.79761,N,12315.34404,W,1,06,2.42,90.2,M,-17.5,M,,*44
def heading(pos_x0,pos_x1,pos_y0,pos_y1):
	#print("delta_x")
	delta_x= (float(pos_x0) -float(pos_x1))
	#print(delta_x)
	
	#print("delta_y")
	delta_y = (float(pos_y0) -float(pos_y1))
	#print(delta_y)
	

	#print("Heading")
	
	if float(delta_x) < 0:
	 #	print("direction is South" )
	 	direction_x = "S"
	else:
	#	print("direction is North")
		direction_x = "N"

	if float(delta_y) < 0:
	 #	print("direction is West" )
	 	direction_y = "W"

	else:
	#	print("direction is East")
		direction_y = "E"

	#x is latitude is N/S.
	#y is longitude is W/E.

	#print("Larger Magnitude of Direction Triangle is:")

	if (float(delta_y) > float(delta_x)) :
	#	print (direction_y) 
		return direction_y
	
	elif(float(delta_x) > float(delta_y)):
	#	print(direction_x)
		return direction_x

	else:
		return "fault"


def position():


	file = open("gps_data.txt","r")
	with open('gps_data.txt', 'r') as file:
		data = file.read().replace('\n', '')
	data = data.split(",")

	# print("parsing")
	# print(data[6])
	# print(data[7])


	# #calc position
	pos_y = data[7]
	pos_x = data[6]

	#print (pos_x)
	#print (pos_y)

	Position_Lat_long = [pos_x,pos_y]

	return Position_Lat_long

	

	

def plotter(pos_x, pos_y,map_png):
	global BLX, BLY, TRX, TRY
	# print("plotter")
	# print(pos_x)
	# print(pos_y)
	
	plt.scatter(x=float(pos_x), y=float(pos_y),alpha = 0.5, s = 25, c='r')
	plt.xlabel('Longitude')
	plt.ylabel('Latitude')
	plt.title('POSITION (in Decimal Degrees)')
	im = plt.imread(map_png)
	implot = plt.imshow(im,extent=[BLX, TRX, BLY, TRY])
	#implot = plt.imshow(im,extent=[long_min, long_max, lat_min, lat_max])
	plt.show()

def file_operations(data):
	f = open("OWEN_gps_data", "w")
	f.write(str(data))
	f.close()
	print("file written")




def main():
	for x in range(1,5):
		print("main call")
		Position_Lat_long = position()
		print(Position_Lat_long)
		x0= Position_Lat_long[0]
		y0 = Position_Lat_long[1]
		plotter(y0,x0,'map.png')
		sleep(3.0)
		Position_Lat_long = position()
		x1= Position_Lat_long[0]
		y1 = Position_Lat_long[1]
		#print("values")
		#print(x0,x1,y0,y1)
		Cardinal_direction = heading(x0,x1,y0,y1)
	#	print (Cardinal_direction)
		#print(x0,y1)
		needed_data = [x0, y0, Cardinal_direction]
		#file_operations(needed_data) #take the running average
		print(needed_data) 
		sleep(3.0)  #if this is too small, faults will occur

if __name__ == "__main__":
	main()
