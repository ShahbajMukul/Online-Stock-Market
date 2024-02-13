from calendar import c
import socket
import threading
import sqlite3
import threading

# Server config
SERVER_IP = '127.0.0.1'
SERVER_PORT = 42069

# Create a socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# binding the socket to the server ip and port
server_socket.bind((SERVER_IP, SERVER_PORT))

# Listen
server_socket.listen()
print(f"\nSERVER LISTENING ON {SERVER_IP}:{SERVER_PORT}\n")

# Handle client calls
def handle_client(client_socket):
    with client_socket, sqlite3.connect('stock_trading.db') as conn:
       # recive the command from the client
        command = client_socket.recv(1024).decode('utf-8')
        print(f"\{command} Requested")
# 
        command_text = command.split()
        
        # comply with the request
        if command_text[0] == "LIST":
            c = conn.cursor()
            c.execute("SELECT * FROM StockMarket")
            stocks = c.fetchall(); # fetch all 

            response = "LIST of stocks:" + "".join([f"{stock[0]} - {stock[1]} - {stock[2]}: {stock[3]} shares " for stock in stocks])
            client_socket.send(response.encode('utf-8'))

        if command_text[0] == "BUY":
            if len(command_text) >= 4:
                user_id, stock_symbol, stock_quantity = int(command_text[1]), command_text[2], int(command_text[3])
                response = handle_buy_command(conn, user_id, stock_symbol, stock_quantity)
                client_socket.send(response.encode('utf-8'))
            else:
                response = "Error: BUY command format is incorrect."
                client_socket.send(response.encode('utf-8'))
        
        if command_text[0] == "SELL":
            if len(command_text) >= 4:
                user_id, stock_symbol, stock_quantity = int(command_text[1]), command_text[2], int(command_text[3])
                response = handle_sell_command(conn, user_id, stock_symbol, stock_quantity)
                client_socket.send(response.encode('utf-8'))
            else:
                response = "Error: SELL command format is incorrect."
                client_socket.send(response.encode('utf-8'))
        if command_text[0] == "BALANCE":
            if len(command_text) >= 2:
                user_id = int(command_text[1])
                c = conn.cursor()
                c.execute("SELECT usd_balance FROM Users WHERE ID = ?", (user_id,))
                user_balance = c.fetchone()
                if user_balance:
                    response = f"User {user_id} has a balance of ${user_balance[0]}"
                else:
                    response = "User not found"
            else:
                response = "Error: BALANCE command format is incorrect."
            client_socket.send(response.encode('utf-8'))
def handle_buy_command(conn, user_id, stock_symbol, req_stock_quantity):
    c = conn.cursor()
    # check if the stock is available
    c.execute("SELECT stock_price FROM StockMarket WHERE stock_symbol = ?", (stock_symbol,))
    stock_price = c.fetchone()
    if not stock_price:
        return "Stock not found" # if we don't have the stock, there won't be a price for it
    
    stock_price = stock_price[0]
    total_price = stock_price * req_stock_quantity

    # check if the user has enough balance
    c.execute("SELECT usd_balance FROM Users WHERE ID = ?", (user_id,))
    user_balance = c.fetchone()
    if not user_balance:
        return "User not found"
    if user_balance[0] < total_price:
        return "Insufficient balance"
    else:
        new_balance = user_balance[0] - total_price
        c.execute("UPDATE Users SET usd_balance = ? WHERE ID = ?", (new_balance, user_id))
        update_or_insert_stock(conn, user_id, stock_symbol, stock_price, req_stock_quantity)
        conn.commit()
        return f"Successfully bought {req_stock_quantity} shares of {stock_symbol} for ${total_price}. Wallet: ${new_balance}"

def update_or_insert_stock(conn, user_id, stock_symbol, stock_price, req_stock_quantity):
    c = conn.cursor()
    # Ensure to pass stock_symbol as a single-element tuple
    c.execute("SELECT stock_name FROM StockMarket WHERE stock_symbol = ?", (stock_symbol,))
    stock_name_result = c.fetchone()
    if not stock_name_result:
        return "Stock not found"
    stock_name = stock_name_result[0]
    
    c.execute("SELECT stock_quantity FROM Stocks WHERE user_id = ? AND stock_symbol = ?", (user_id, stock_symbol))
    stock = c.fetchone()
    if stock:  # If stock exists, update its quantity
        new_stock_quantity = stock[0] + req_stock_quantity
        c.execute("UPDATE Stocks SET stock_quantity = ? WHERE user_id = ? AND stock_symbol = ?", (new_stock_quantity, user_id, stock_symbol))
    else:  # If stock doesn't exist, insert a new record
        # test
        c.execute("INSERT INTO Stocks (stock_symbol, stock_name, stock_price, user_id, stock_quantity) VALUES (?, ?, ?, ?, ?)", (stock_symbol, stock_name, stock_price, user_id, req_stock_quantity))

    conn.commit() 

def handle_sell_command(conn, user_id, stock_symbol, req_stock_quantity):
    c = conn.cursor()

    c.execute("SELECT stock_price FROM StockMarket WHERE stock_symbol = ?", (stock_symbol,))
    stock_price_result = c.fetchone()
    if stock_price_result is None:
        return "Stock not found in the market."
    stock_price = stock_price_result[0]

    # test
    print("user_id:", user_id)
    print("stock_symbol:", stock_symbol)

    # check if user has the stock and enough of it
    c.execute("SELECT stock_quantity FROM Stocks WHERE user_id = ? AND stock_symbol = ?", (user_id, stock_symbol))
    user_stock_info = c.fetchone()
    print(f"user_stock_info: {user_stock_info}")
    if user_stock_info is None:
        print("User does not have this stock.")
        return "User does not have this stock."
    elif user_stock_info[0] < req_stock_quantity:
        return "Requested stock quantity exceeds the user's stock balance."
    total_price = stock_price * req_stock_quantity

    # update user's balance
    c.execute("SELECT usd_balance FROM Users WHERE ID = ?", (user_id,))
    user_balance = c.fetchone()[0]
    new_balance = user_balance + total_price
    c.execute("UPDATE Users SET usd_balance = ? WHERE ID = ?", (new_balance, user_id))

    # update user's stock balance
    user_stock_quantity = user_stock_info[0] - req_stock_quantity
    if user_stock_quantity == 0:
        c.execute("DELETE FROM Stocks WHERE user_id = ? AND stock_symbol = ?", (user_id, stock_symbol))
    else:
        c.execute("UPDATE Stocks SET stock_quantity = ? WHERE user_id = ? AND stock_symbol = ?", (user_stock_quantity, user_id, stock_symbol))

    conn.commit()
    return f"Successfully sold {req_stock_quantity} shares of {stock_symbol} for ${total_price}. Wallet: ${new_balance}"



while True:
    client_socket, client_address = server_socket.accept()
    print(f"\nConnection from {client_address} has been established\n")
    client_thread = threading.Thread(target=handle_client, args=(client_socket,))
    client_thread.start()