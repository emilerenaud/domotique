
import time
import datetime
try:
    import paho.mqtt.client as paho
except:
    print("Library paho not install")
    exit()

broker = "192.168.2.101"
port = 1883

def on_publish(client,data,result):
    # print("data publish")
    pass


def main():
    print("MQTT - Clock - Python")
    client = paho.Client("clock1")
    client.on_publish = on_publish
    client.connect(broker,port)
    loopVar = True
    while(loopVar):
        client.loop()
        client.publish("clock/now",time.strftime("%H:%M:%S", time.localtime()))
        time.sleep(5)


if __name__== "__main__" :
    main()