
import time
import datetime

try:
    import paho.mqtt.client as mqtt
except:
    print("Library paho not install : pip install paho-mqtt")
    exit()

# MQTT Broker
broker_address = "192.168.2.100"  # Change this to your MQTT broker's address
port = 1883  # Default MQTT port
topic = "time"  # Topic to publish the time

# Create a MQTT client instance
client = mqtt.Client(protocol=mqtt.MQTTv311)  # Specify MQTT protocol version

# Connect to the MQTT broker
client.connect(broker_address, port)

while True:
    # Get current time
    current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    # Publish the time to the MQTT topic
    client.publish(topic, current_time)

    # Print the time for confirmation
    print("Published:", current_time)

    # Wait for 1 second before publishing again
    time.sleep(1)

# Disconnect from the MQTT broker
client.disconnect()
