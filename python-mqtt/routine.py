

try:
    import paho.mqtt.client as paho
except:
    print("Library paho not install")
    exit()

broker = "192.168.2.101"
port = 1883
timeNow = ""

def set_timeNow(time):
    global timeNow
    timeNow = time

def on_message(client, userdata, message):
    print("message received",str(message.payload.decode("utf-8")))
    set_timeNow(str(message.payload.decode("utf-8")))
    # print("message topic=",message.topic)
    # print("message qos=",message.qos)
    # print("message retain flag=",message.retain)

def on_publish(client,data,result):
    print("data publish")
    pass

def main():
    print("MQTT - Routine - Python")
    client = paho.Client("routine1")
    client.connect(broker,port)

    # subscribe to topic clock/now and call on_message when a message is received
    client.subscribe("clock/now")
    client.on_message = on_message
    client.on_publish = on_publish




    loopVar = True
    oneTime = False

    while(loopVar):
        client.loop()
        # check the hour and minute in timeNow. if it is 8:00, then publish to topic routine/now
        hour = timeNow[0:2]
        minute = timeNow[3:5]
        
        if hour == "10" and minute == "45":
            if oneTime == False:
                client.publish("blinds/control","100")
                oneTime = True
        else:
            oneTime = False








        pass




if __name__== "__main__" :
    main()