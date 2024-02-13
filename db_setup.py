import sqlite3

connection = sqlite3.connect('stock_trading.db')

cursor = connection.cursor()

cursor.execute('''CREATE TABLE IF NOT EXISTS StockMarket (
               stock_symbol VARCHAR(4) PRIMARY KEY,
                stock_name VARCHAR(20) NOT NULL,
                stock_price DOUBLE NOT NULL)'''
)
               

cursor.execute('''CREATE TABLE IF NOT EXISTS Users
              (ID INTEGER PRIMARY KEY AUTOINCREMENT,
               first_name TEXT,
               last_name TEXT,
               user_name TEXT NOT NULL,
               password TEXT,
               usd_balance DOUBLE NOT NULL)''')

cursor.execute('''CREATE TABLE IF NOT EXISTS Stocks
               (ID INTEGER PRIMARY KEY AUTOINCREMENT,
               stock_symbol VARCHAR(4) NOT NULL,
               stock_name VARCHAR(20) NOT NULL,
               stock_quantity DOUBLE NOT NULL, 
               user_id INTEGER,
               FOREIGN KEY (user_id) REFERENCES Users(ID),
                FOREIGN KEY (stock_symbol) REFERENCES StockMarket(stock_symbol))''')

connection.commit()
connection.close()