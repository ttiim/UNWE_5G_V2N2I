#!/bin/bash

# $GPHDT - Heading, True
#   $GPGLL - Geographic position, latitude / longitude
# GNGGA Time, position, and fix related data of the receiver.
#Side note on the Ublox 8 series GPGGA is replaced by GNGGA by default to indicate the navigation information is from multiple GNSS sources (I.e American GPS and Russian GLONASS normally


	#https://www.systutorials.com/docs/linux/man/1-gpxlogger/

while :

do

GPS_LINE="$(gpspipe -w -n 10 | grep lat | tr -d ':''""'lonlat{})"   #original uses GPGGA. GNGGA should be the same
echo "${GPS_LINE}" > gps_data.txt
sleep 0.9
done