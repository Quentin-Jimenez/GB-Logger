import paho.mqtt.client as mqtt
import os
import json
from discord_webhook import DiscordWebhook
import datetime

MQTT_ADDRESS = '192.168.1.6'
MQTT_USER = 'brothers1883'
MQTT_PASSWORD = 'brothers1883'
MQTT_TOPIC = 'Lodge/Room8/GBData'


def on_connect(client, userdata, flags, rc):
    """ The callback for when the client receives a CONNACK response from the server."""
    print('Connected with result code ' + str(rc))
    client.subscribe(MQTT_TOPIC)


def on_message(client, userdata, msg):
    """The callback for when a PUBLISH message is received from the server."""
    message = str(msg.payload)
    now = datetime.datetime.now()
    
    if 'water' in message:
        # Water Change
        filename = os.path.join('waterdata.json')
        with open(filename, 'r') as f:
            data = json.load(f)  
        data[str(len(data))] = now.strftime('%H:%M %m/%d/%Y')
        with open(filename, 'w') as f:  # saves data to file
            json.dump(data, f, indent=4)
        
    elif 'bottle' in message:
        # Bottle Change
        filename = os.path.join('bottledata.json')
        with open(filename, 'r') as f:
            data = json.load(f)  
        data[str(len(data))] = now.strftime('%H:%M %m/%d/%Y')
        with open(filename, 'w') as f:  # saves data to file
            json.dump(data, f, indent=4)
        
    else:
        # Someone Took a GB
        message = message.split(',')
        
        filename = os.path.join('gbdata.json')
        gbdata = {'name': message[0][2:], 'size': message[1], 'room': message[2][:-1], 'Time': now.strftime('%H:%M %m/%d/%Y')}
        with open(filename, 'r') as f:
            data = json.load(f)  
        data[str(len(data))] = gbdata
        with open(filename, 'w') as f:  # saves data to file
            json.dump(data, f, indent=4)


def main():
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message

    mqtt_client.connect(MQTT_ADDRESS, 1883)
    mqtt_client.loop_forever()


if __name__ == '__main__':
    print('MQTT to InfluxDB bridge')
    main()