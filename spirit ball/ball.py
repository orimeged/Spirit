import socket
server_socket = socket.socket()
server_socket.bind(("0.0.0.0", 1337))
server_socket.listen()
print("Server is up and running")
(client_socket, client_address) = server_socket.accept()
print("Client connected")
data=" "
while(data != "q"):
    reply = input()
    client_socket.send(reply.encode())
    if(reply == "power"):
        print(">")
        command = input()
        client_socket.send(command.encode())
    else
        data = client_socket.recv(1024).decode()
client_socket.close()
server_socket.close()