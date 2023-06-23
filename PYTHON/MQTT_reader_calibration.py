import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("arduino/simple/asm/prueba")

def on_message(client, userdata, msg):
    print(msg.topic+" "+msg.payload.decode())
    # ejemplo de aplicación
    dBx = float(msg.payload.decode().split()[0]) # dBx = Bs0 - Bs1
    dBy = float(msg.payload.decode().split()[1]) # BBy = Bs2 - Bs3
    print (f"dBx = {dBx:.2f} uT")
    print (f"dBy = {dBy:.2f} uT")

    with open('data.txt','a+') as fp:
         fp.write("Message received: "  + msg.payload.decode() + "\n")

client = mqtt.Client()
# seleccionamos las funciones que se llamaran después en el interior 
# de la función  loop.forever()
client.on_connect = on_connect
client.on_message = on_message

client.connect("test.mosquitto.org", 1883, 60)
client.loop_forever()
