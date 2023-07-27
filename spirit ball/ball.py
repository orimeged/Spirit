import socket
import os
import subprocess
from PIL import ImageGrab  # ספריית PIL מאפשרת צילום מסך בצד השרת

server_socket = socket.socket()
server_socket.bind(("0.0.0.0", 1337))
server_socket.listen()
print("Server is up and running")
(client_socket, client_address) = server_socket.accept()

print("Client connected")
reply = " "
command = " "

while reply != "exit_of_server":
    print("1")
    reply = input()
    print("2")
    client_socket.send(reply.encode())
    if reply == "power":
        while command != "exit_power":
            print(">")
            command = input()
            client_socket.send(command.encode())
            output = client_socket.recv(1024).decode()
            print(output)

    # פקודה מיוחדת לבצע צילום מסך ושליחת התמונה
    elif reply == "screenshot":
        screenshot = ImageGrab.grab()
        screenshot.save("screenshot.jpg")
        file_size = os.path.getsize("screenshot.jpg")
        client_socket.send(f"Sending screenshot ({file_size} bytes)".encode())
        with open("screenshot.jpg", "rb") as file:
            data = file.read(1024)
            while data:
                client_socket.send(data)
                data = file.read(1024)
        client_socket.send("screenshot_done".encode())

client_socket.close()
server_socket.close()
