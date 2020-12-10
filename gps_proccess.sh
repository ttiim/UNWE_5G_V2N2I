#!/bin/bash

# $GPHDT - Heading, True
#   $GPGLL - Geographic position, latitude / longitude
# GNGGA Time, position, and fix related data of the receiver.
#Side note on the Ublox 8 series GPGGA is replaced by GNGGA by default to indicate the navigation information is from multiple GNSS sources (I.e American GPS and Russian GLONASS normally


	#https://www.systutorials.com/docs/linux/man/1-gpxlogger/


timeout --foreground 5 cgps  

 while :

 do

# #GPS_LINE="$(gpspipe -w -n 10  | grep  | head -1)"   #original uses GPGGA. GNGGA should be the same
# #GPS_LINE=$(gpxlogger)
# #echo "${GPS_LINE}" #> gps_data.txt
timeout --foreground 1 gpxlogger > gps_logger.xml
timeout --foreground 3 cgps  
done