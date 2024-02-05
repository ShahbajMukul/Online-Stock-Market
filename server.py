import socket
import threading

# Server config
SERVER_IP = '127.0.0.1'
SERVER_PORT = 42069

# Create a socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# binding the socket to the server ip and port
server_socket.bind(SERVER_IP, SERVER_PORT)

# Listen
server_socket.listen()
print(f"\nSERVER LISTENING ON {SERVER_IP}:{SERVER_PORT}\n")

# Handle client calls
def handle_client(client_socket):
    with client_socket:
        print("\nClient connected\n")

while True:
    client_socket, client_address = server_socket.accept()
    print(f"\nConnection from {client_address} has been established\n")
    client_thread = threading.Thread(target=handle_client, args=(client_socket,))
    client_thread.start()