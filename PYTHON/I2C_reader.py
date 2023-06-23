import serial
import time

# instancia de la clase serial
serialArduino = serial.Serial("COM6",115200)
# damos un tiempo para que se establezca la conexión
time.sleep(1)

# ciclo infinito para ver si hay algo en el puerto serie: Este programa 
# solo tiene el objetivo de recoger lo que hay en el puerto serie e imprimirlo
# por la consola de Python, o llevarlo a un archivo .txt
while True:
	# leemos lo que hay en el puerto serie (decodificando)
	data = serialArduino.readline().decode()
	print (data)
