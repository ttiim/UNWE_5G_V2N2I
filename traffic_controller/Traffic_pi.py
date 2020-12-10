'''
Software for the Traffic Side Raspberry Pi.

IoT Hub: AURORA - under vpri-poc10 Resource Group

Functionality:
1. Send SPAT Data to the Cloud (D2C) periodically
2. Receive requests from IoT Hub (C2D) to change Traffic Lights
'''

import threading
import time
from azure.iot.device import IoTHubDeviceClient, Message
from datetime import datetime
import DecodeSPAT
import json
from datetime import datetime
import ntplib
import ast

CONNECTION_STRING = 'HostName=AURORA.azure-devices.net;DeviceId=Traffic_Side_Pi;SharedAccessKey=lJVeLqrbEPflqtXJbN6XlU7P+uqgnI+ygBKet21egBA='

RECEIVED_MESSAGES = 0
ChangeLights = 0

ntpc = ntplib.NTPClient()
NTP_REQUEST = 'pool.ntp.org'

# Create an IoT Hub client
def iothub_client_init():
    client = IoTHubDeviceClient.create_from_connection_string(CONNECTION_STRING)
    return client

def iothub_receive_message(client): #Thread to receive Requests to Change Traffic Signal Lights from IoT Hub
    global RECEIVED_MESSAGES
    global ChangeLights 
    while True:
        message = client.receive_message()
        RECEIVED_MESSAGES += 1
        cur_time = datetime.now()
        resp = ntpc.request(NTP_REQUEST)
        cur_time = datetime.fromtimestamp(resp.tx_time)
        cur_time = float(cur_time.strftime("%S.%f"))

        #print(message.data) #Prints message coming from IoT Hub
        Change_Lights = str(message.data).split(":")[1].split("}")[0].split(',')[0]
        prev_time = float(str(message.data).split(",")[1].split(":")[1].split("}")[0])
        print("IoT Hub to Traffic Pi Delay: ", (cur_time-prev_time), ' seconds') 
        if Change_Lights == 'True':
            print('*** Traffic Lights Changing.... ***')
            ChangeLights = 1
            
            #For now, print out message to change Traffic Lights...
            #Replace with logic later to actually change the traffic lights
			
        print( "Total Messages received: {}".format(RECEIVED_MESSAGES))


def iothub_send_message():
    try:
        client = iothub_client_init() #Creates Client

        #Start the Receive Messages Thread from IoT Hub Thread
        message_listener_thread = threading.Thread(target=iothub_receive_message, args=(client,))
        message_listener_thread.daemon = True
        message_listener_thread.start()

        print("Traffic Side Pi sending and receiving data to and from IoT Hub, press Ctrl-C to exit")
        global ChangeLights 
        ChangeLights = 0

        while True: #Sending messages is the main thread
            
            #Need to determine how to synchronize and send SPAT Message periodically to IoT Hub  
            #SPAT_msg = DecodeSPAT.ReturnSPAT()

            #Parsed_TSCBM.txt
            file = open("Parsed_TSCBM.txt","r")
            with open('Parsed_TSCBM.txt', 'r') as file:  # tsbm.c writes the decoded output to this file
                data = file.readline().replace('\n', '')
            SPAT_msg = str(data)  #should just be a string now>?
            res= json.loads(SPAT_msg);
            SPAT_msg = res
          
            cur_time = datetime.now()
            resp = ntpc.request(NTP_REQUEST)
            cur_time = datetime.fromtimestamp(resp.tx_time)
            cur_time = cur_time.strftime("%S.%f") #Adding time to determine delay between messages
            SPAT_msg['time'] = cur_time
            SPAT_msg['DeviceName'] = 'Traffic_Side_Pi'
            message = Message(json.dumps(SPAT_msg)) #Converts Dict to String for Azure IoT

            print("Sending SPAT Data")
            #print("Sending Message: \n {}".format(message))
            client.send_message(message) #sends message to IoT Hub
            print("Message successfully sent")
            print(cur_time)

            time.sleep(5) #Temporary


    except KeyboardInterrupt:
        print("Vehicle Pi stopped")

if __name__ == '__main__':
    iothub_send_message()
