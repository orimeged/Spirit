import socket
server_socket = socket.socket()
server_socket.bind(("0.0.0.0", 1337))
server_socket.listen()
print("Server is up and running")
(client_socket, client_address) = server_socket.accept()
print("Client connected")
reply=" "
command = " "
while(reply != "exit_of_server"):
    reply = input()
    client_socket.send(reply.encode())
    if(reply == "power"):
        while(command != "exit_power"):
            print(">")
            command = input()
            client_socket.send(command.encode())

client_socket.close()
server_socket.close()