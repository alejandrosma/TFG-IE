import paho.mqtt.client as mqtt

def B(x):
    """ Aproximación lineal B(x)
        input (x) : posición x 
        output : campo magnético en la posición x
    """
    return 0.2348 * x + 8.319 


def x(B):
    """ Función inversa de la aproximación lineal B(x)
        input (B) : campo magnético medido por el sensor
        output : posición x asociada a ese campo magnético 
    """
    return (B - 8.319) / 0.2348

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("arduino/simple/asm/prueba")

def on_message(client, userdata, msg):
    print(msg.topic+" "+msg.payload.decode())
    # ejemplo de aplicación
    Bs0 = float(msg.payload.decode().split()[0]) # señal del sensor s0
    Bs1 = float(msg.payload.decode().split()[1]) # señal del sensor s1
    Bs2 = float(msg.payload.decode().split()[2]) # señal del sensor s2
    Bs3 = float(msg.payload.decode().split()[3]) # señal del sensor s3
    dx = (x(Bs0) - x (Bs1)) / 2 # desplazamiento a realizar en la dirección x con el micrometro
    dy = (x(Bs2) - x (Bs3)) / 2 # desplazamiento a realizar en la dirección y con el micrometro
    print (f"Bs0 = {Bs0:.2f} mT , Bs1 = {Bs1:.2f} mT, Bsol = {B(min(x(Bs0),x(Bs1)) + dx):.2f} mT , dx = {dx:.2f} mm")
    print (f"Bs2 = {Bs2:.2f} mT , Bs3 = {Bs3:.2f} mT, Bsol = {B(min(x(Bs2),x(Bs3)) + dy):.2f} mT , dy = {dy:.2f} mm")

    with open('data.txt','a+') as fp:
         fp.write("Message received: "  + msg.payload.decode() + "\n")

client = mqtt.Client()
# seleccionamos las funciones que se llamaran después en el interior 
# de la función  loop.forever()
client.on_connect = on_connect
client.on_message = on_message

client.connect("test.mosquitto.org", 1883, 60)
client.loop_forever()
