
# Author: Timothy Wriglesworth
# University of British Columbia, 
# Radio Science Lab

# Requirements:
# Lat and Longitude
# Get the heading in a format like "North", "West", "East", "South",

# from http://aprs.gids.nl/nmea/

#ublox uses GNPGGA instead of GPNGGA
#run this program along gps_process.sh


#x is a longitude
#y is a latitude
import shutil, sys  
#from pynmea import nmea
import matplotlib.pyplot as plt
#import pynmea2
from time import sleep
import numpy as np




TRX = -123.228546          #top right longitude
TRY = 49.279441            #top right latitude
BLX = -123.263335          #bottom left longitude
BLY = 49.248726             #bottom left latitude

TRX_ = -123.24198        #top right longitude
TRY_ = 49.26783            #top right latitude
BLX_ = -123.24843          #bottom left longitude
BLY_ = 49.26545             #bottom left latitude


#long_min = -123.24863 #TRX 
#long_max = -123.24228
#lat_min= 49.26749 #BLY
#lat_max = 49.26529 #TRY


#format of gps data in gps_data.txt 
#is same as $cgps 
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

	

	

def plotter(pos_x, pos_y,map_png,timeout):
	global BLX, BLY, TRX, TRY
	global BLX_, BLY_, TRX_, TRY_
	# print("plotter")
	# print(pos_x)
	# print(pos_y)
	x=[-123.24480,-123.24506,-123.24523]
	y=[49.26561,49.26596,49.26618]
	#plt.scatter(x=float(pos_x), y=float(pos_y),alpha = 0.5, s = 25, c='r')
	plt.scatter(x,y,alpha = 0.5, s = 5, c='b')
	plt.plot(x,y)
	plt.xlabel('Longitude')
	plt.ylabel('Latitude')
	plt.title('POSITION (in Decimal Degrees)')
	im = plt.imread(map_png)
	implot = plt.imshow(im,extent=[BLX, TRX, BLY, TRY])
	#implot = plt.imshow(im,extent=[BLX_, TRX_, BLY_, TRY_])
	#implot = plt.imshow(im,extent=[long_min, long_max, lat_min, lat_max])
	plt.show(block=False)
	plt.pause(timeout)
	plt.close()

def file_operations(data):
	f = open("OWEN_gps_data", "w")
	f.write(str(data))
	f.close()
	print("file written")




def main():
    while (True): 
	#for x in range(1,2):
        print("main call")
        Position_Lat_long = position()
        print(Position_Lat_long)
        x0= Position_Lat_long[0]
        y0 = Position_Lat_long[1]
        #plotter(y0,x0,'map.png')
        plotter(y0,x0,'map.png',2)
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
        file_operations(needed_data) #take the running average
        print(needed_data)
        sleep(3.0)  #if this is too small, faults will occur

if __name__ == "__main__":
	main()

