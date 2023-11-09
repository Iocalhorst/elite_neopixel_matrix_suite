import socket
import sys



ip = "yourHostIP"
port = 9001
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

server_address = (ip, port)
s.bind(server_address)

print("####### Server is listening #######")
while True:
    data, address = s.recvfrom(128)
    try :
    #print(data.decode('utf-8'), end='')
        print(data.decode(),end='')    
    except :
        print(data);
    #send_data = input("Type some text to send => ")
    #s.sendto(send_data.encode('utf-8'), address)
    #print("\n\n 1. Server sent : ", send_data,"\n\n")