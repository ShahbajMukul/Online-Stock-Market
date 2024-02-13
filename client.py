import socket

s=socket.socket()

port = 42069

s.connect(('127.0.0.1', port))

# s.sendall(b'LIST')
# data = s.recv(1024)

# print("Recieved", repr(data))

# print("\n")

# BUY
# Send a BUY command to the server

""" buy_command = 'BUY 1 TSLA 2'.encode('utf-8')
s.sendall(buy_command)
import time
time.sleep(1)
# Recieve the response from the server
buy_data = s.recv(1299)
print("Recieved", repr(buy_data))
 """
 
# SELL
# Send a SELL command to the server

""" sell_command = 'SELL 1 MSFT 1'.encode('utf-8')
s.sendall(sell_command)
sell_data = s.recv(1024)
print("Recieved", repr(sell_data))

s.close() """

# BALANCE
# Send a BALANCE command to the server

balance_command = 'BALANCE 1'.encode('utf-8')
s.sendall(balance_command)
balance_data = s.recv(1024)
print("Recieved", repr(balance_data))