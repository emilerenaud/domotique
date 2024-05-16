import paho.mqtt.client as mqtt
import logging
import time
import datetime

# Set up logging
logging.basicConfig(filename='/home/emile/domotique/python-mqtt/mqtt_publisher.log', level=logging.INFO)

# MQTT Broker
broker_address = "192.168.2.100"  # Change this to your MQTT broker's address
port = 1883  # Default MQTT port
topic = "time"  # Topic to publish the time

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        logging.info("Connected to MQTT broker")
    else:
        logging.error("Failed to connect to MQTT broker, rc=%d", rc)

def on_publish(client, userdata, mid):
    logging.info("Message published")

# Create a MQTT client instance
client = mqtt.Client("Time_Publisher")

# Set callback functions
client.on_connect = on_connect
client.on_publish = on_publish

# Connect to the MQTT broker
client.connect(broker_address, port)

# Main loop
try:
    while True:
        # Get current time
        current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        # Publish the time to the MQTT topic
        client.publish(topic, current_time)

        # Log the time for confirmation
        logging.info("Published: %s", current_time)

        # Wait for 1 second before publishing again
        time.sleep(1)

except KeyboardInterrupt:
    logging.info("Keyboard interrupt, stopping script")

finally:
    # Disconnect from the MQTT broker
    client.disconnect()
    logging.info("Disconnected from MQTT broker")