//============================================================================
// Name        : Server.cpp
// Author      :	Abdullah Mahith
// Version     :	2/7/2024

//============================================================================

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <map>
using namespace std;


/**
 * Class to hold the stock details and
 * user details
 */
class Stocks
{
public:
	int ID;
	string stock_symbol;
	string stock_name;
	double stock_balance;
	int user_id;

	Stocks() {
		ID = 0;
		stock_symbol = "";
		stock_balance= 0;
		user_id = 0;
	}

	int getId() const {
		return ID;
	}

	void setId(int id) {
		ID = id;
	}

	double getStockBalance() const {
		return stock_balance;
	}

	void setStockBalance(double stockBalance) {
		stock_balance = stockBalance;
	}

	const string& getStockName() const {
		return stock_name;
	}

	void setStockName(const string &stockName) {
		stock_name = stockName;
	}

	const string& getStockSymbol() const {
		return stock_symbol;
	}

	void setStockSymbol(const string &stockSymbol) {
		stock_symbol = stockSymbol;
	}

	int getUserId() const {
		return user_id;
	}

	void setUserId(int userId) {
		user_id = userId;
	}
};

/**
 * Class to hold the user details
 */
class Users
{
public:
	int ID;
	string email;
	string first_name;
	string last_name;
	string user_name;
	string password;
	double usd_balance;

	Users() {
		ID = 0;
		email = "";
		first_name=  "";
		last_name= "";
		password = "";
		usd_balance = 0;
	}

	const string& getEmail() const {
		return email;
	}

	void setEmail(const string &email) {
		this->email = email;
	}

	const string& getFirstName() const {
		return first_name;
	}

	void setFirstName(const string &firstName) {
		first_name = firstName;
	}

	int getId() const {
		return ID;
	}

	void setId(int id) {
		ID = id;
	}

	const string& getLastName() const {
		return last_name;
	}

	void setLastName(const string &lastName) {
		last_name = lastName;
	}

	const string& getPassword() const {
		return password;
	}

	void setPassword(const string &password) {
		this->password = password;
	}

	double getUsdBalance() const {
		return usd_balance;
	}

	void setUsdBalance(double usdBalance) {
		usd_balance = usdBalance;
	}

	const string& getUserName() const {
		return user_name;
	}

	void setUserName(const string &userName) {
		user_name = userName;
	}
};

map<int, Users*> userMap;
map<int, Stocks*> stockMap;

/**
 * Function to load the stock data from stocks.txt file
 */
void loadStockData() {
	fstream stock_file;
	stock_file.open("stocks.txt", ios::in);
	if (stock_file.is_open()) {
		string line;
		while (getline(stock_file, line)) {
			stringstream line_stream(line);
			string word;
			vector<string> items;
			while (line_stream >> word) {
				items.push_back(word);
			}

			Stocks *stocks = new Stocks();
			stocks->ID = stoi(items[0]);
			stocks->stock_symbol = items[1];
			stocks->stock_name = items[1];
			stocks->stock_balance = stod(items[1]);
			stocks->user_id = stoi(items[1]);
			stockMap[stocks->ID] = stocks;
		}
		stock_file.close();
	}


}

/**
 * Function to load the stock data from users.txt file
 */
void loadUserData() {
	fstream user_file;
	user_file.open("users.txt", ios::in);
	if (user_file.is_open()) {
		string line;
		while (getline(user_file, line)) {
			stringstream line_stream(line);
			string word;
			vector<string> items;
			while (line_stream >> word) {
				items.push_back(word);
			}

			Users *users = new Users();
			users->ID = stoi(items[0]);
			users->email =  items[1];
			users->first_name = items[2];
			users->last_name =  items[3];
			users->user_name = items[4];
			users->password = items[5];
			users->usd_balance = stod(items[6]);
			userMap[users->ID] = users;
		}
		user_file.close();
	}
}

/**
 * Function to return the stock object for given name and uid
 */
Stocks* getStock(string name, int uid) {
	map<int, Stocks*>::iterator it = stockMap.begin();
	while (it != stockMap.end()) {
		Stocks *st = it->second;
		if (st->stock_name == name && st->user_id == uid) {
			return st;
		}
		++it;
	}
	return nullptr;
}

/**
 * Function to return the User object for given uid
 */
Users* getUsers(int uid) {
	map<int, Users*>::iterator it = userMap.begin();
	while (it != userMap.end()) {
		Users *users = it->second;
		if (users->ID == uid) {
			return users;
		}
		++it;
	}
	return nullptr;
}

/**
 * Function use d to return the command type
 */
int getType(string command) {
	if (command == "BUY") {
		return 0;
	}
	else if (command == "SELL") {
		return 1;
	}
	else if (command == "LIST") {
		return 2;
	}
	else if (command == "BALANCE") {
		return 3;
	}
	else if (command == "SHUTDOWN") {
		return 4;
	}
	else if (command == "QUIT") {
		return 5;
	}
	else{
		return -1;
	}
}

/**
 * Function used to handle the client side data and process
 * the commands, reply back to client
 */
int handleData(string auctionData, int socketId) {
	cout <<"Received: "<<auctionData<<endl;
	stringstream auctionStream(auctionData);
	vector <string> tokens;
	string intermediate;

	while(getline(auctionStream, intermediate, ' '))
	{
		tokens.push_back(intermediate);
	}

	int type = getType(tokens[0]);
	switch(type) {
	case 0:{
		if (tokens.size()!=5) {
			string mes = "403 message format error";
			send(socketId, mes.c_str(), mes.size(), 0);
			break;
		}
		string stock_name = tokens[1];
		double stock_amount = stod(tokens[2]);
		double price = stod(tokens[3]);
		int uid = stoi(tokens[4]);
		double total = stock_amount * price;
		Stocks *stocks  = getStock(stock_name, uid);
		Users *user  = getUsers(uid);
		if (user == nullptr) {
			//cout<<"user "<< uid <<" doesn’t exist\n";
			break;
		}
		if (stocks != nullptr){
			if (user->usd_balance < total){
				//cout <<"not enough USD\n";
				string msg = "not enough USD";
				send(socketId, msg.c_str(), msg.size(), 0);
			}
			else{
				user->usd_balance -= total;
				stocks->stock_balance += stock_amount;
				//cout<<"200 OK\n";
				//cout<<"BOUGHT: New balance: "<<stocks->stock_balance <<" "<<stock_name<<". USD balance $"<<user->usd_balance<<endl;
				string msg = "200 OK\nBOUGHT: New balance: " + to_string(stocks->stock_balance) +" "+ stock_name+". USD balance $"+to_string(user->usd_balance);
				send(socketId, msg.c_str(), msg.size(), 0);
			}
		}
		else{
			stocks = new Stocks();
			if (stockMap.size() > 0) {
				Stocks *last = stockMap[stockMap.size()-1];
				stocks->ID = last->ID + 1;
			}
			else{
				stocks->ID = 1;
			}

			stocks->stock_balance = stock_amount;
			stocks->stock_name = stock_name;
			stocks->stock_symbol = stock_name;
			stocks->user_id = uid;

			if (user->usd_balance < total){
				//cout <<"not enough USD\n";
				string msg = "not enough USD";
				send(socketId, msg.c_str(), msg.size(), 0);
			}
			else{
				user->usd_balance -= total;
				stockMap[stocks->ID] = stocks;
				//cout<<"200 OK\n";
				//cout<<"BOUGHT: New balance: "<<stock_amount <<" "<<stock_name<<". USD balance $"<<user->usd_balance<<endl;
				string msg = "200 OK\nBOUGHT: New balance: " + to_string(stock_amount) +" " +stock_name+". USD balance $"+to_string(user->usd_balance);
				send(socketId, msg.c_str(), msg.size(), 0);
			}
		}
		break;
	}

	case 1:{
		if (tokens.size()!=5) {
			string mes = "403 message format error";
			send(socketId, mes.c_str(), mes.size(), 0);
			break;
		}
		string stock_name = tokens[1];
		double stock_amount = stod(tokens[2]);
		double price = stod(tokens[3]);
		int uid = stoi(tokens[4]);
		double total = stock_amount * price;
		Stocks *stocks  = getStock(stock_name, uid);
		Users *user  = getUsers(uid);
		if (user == nullptr) {
			//cout<<"user "<< uid <<" doesn’t exist\n";
			string msg = "user "+ to_string(uid) +" doesn’t exist";
			send(socketId, msg.c_str(), msg.size(), 0);
			break;
		}
		if (stocks != nullptr){
			if (stocks->stock_balance < stock_amount){
				//cout <<"Not enough "<<stocks->stock_symbol <<" stock balance\n";
				string msg = "Not enough " +stocks->stock_symbol + " stock balance";
				send(socketId, msg.c_str(), msg.size(), 0);
			}
			else{
				user->usd_balance += total;
				stocks->stock_balance -= stock_amount;
				//cout<<"200 OK\n";
				//cout<<"SOLD: New balance: "<<stocks->stock_balance <<" "<<stock_name<<". USD balance $"<<user->usd_balance<<endl;
				string msg = "200 OK\nSOLD: New balance: " + to_string(stocks->stock_balance) +" " +stock_name+". USD balance $"+to_string(user->usd_balance);
				send(socketId, msg.c_str(), msg.size(), 0);
			}
		}
		else{
			//cout <<"Stock "<< stock_name <<" does not exists\n";
			string msg = "Stock "+stock_name +" does not exists";
			send(socketId, msg.c_str(), msg.size(), 0);
		}
		break;
	}

	case 2:{

		//cout<<"200 OK\n";
		//cout<<"The list of records in the Stocks database for user 1" <<endl;
		string mes = "200 OK\nThe list of records in the Stocks database for user 1\n";

		map<int, Stocks*>::iterator it = stockMap.begin();
		while (it != stockMap.end()) {
			Stocks *stocks = it->second;
			cout << stocks->ID <<" " <<stocks->stock_symbol <<" " <<stocks->stock_balance <<" " <<stocks->user_id <<endl;
			mes += to_string(stocks->ID) + " " + stocks->stock_symbol +" " +to_string(stocks->stock_balance) +" " + to_string(stocks->user_id);
			send(socketId, mes.c_str(), mes.size(), 0);
			++it;
		}
		break;
	}

	case 3:{
		Users *user = getUsers(1);
		cout<<"200 OK\n";
		cout<<"Balance for user "<< user->ID <<": $" <<user->usd_balance <<endl;
		string mes = "200 OK\n Balance for user "+ user->first_name+ " "+ user->last_name +": $" +to_string(user->usd_balance);
		send(socketId, mes.c_str(), mes.size(), 0);
		break;
	}

	case 4:{
		string mes = "200 OK\n";
		send(socketId, mes.c_str(), mes.size(), 0);
		return -1;
	}
	case 5:{
		string mes = "200 OK\n";
		send(socketId, mes.c_str(), mes.size(), 0);
		return 1;
	}
	case -1:
		string mes = "400 invalid command";
		send(socketId, mes.c_str(), mes.size(), 0);
		break;
	}
	return 0;
}


//DRIVER CODE
int main(int argc, char *argv[])
{
	loadUserData();
	loadStockData();

	if(argc != 2)
	{
		cerr << "Usage: port" << endl;
		exit(0);
	}
	int portId = atoi(argv[1]);
	char buffer[1500];

	sockaddr_in serverAddress;
	bzero((char*)&serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(portId);

	int serverId = socket(AF_INET, SOCK_STREAM, 0);
	if(serverId < 0)
	{
		cerr << "Error establishing the server socket" << endl;
		exit(0);
	}
	int status = bind(serverId, (struct sockaddr*) &serverAddress,
			sizeof(serverAddress));
	if(status < 0)
	{
		cerr << "Error binding socket to local address" << endl;
		exit(0);
	}

	listen(serverId, 5);
	sockaddr_in newAddress;
	socklen_t addrSize = sizeof(newAddress);
	bool shutdown = false;
	bool quit = false;
	int newSocketId = -1;
	while(1) {
		cout << "Waiting for new client to connect..." << endl;
		newSocketId = accept(serverId, (sockaddr *)&newAddress, &addrSize);
		if(newSocketId < 0)
		{
			cerr << "Error accepting request from client!" << endl;
			exit(1);
		}
		cout << "Connected with client!" << endl;
		while(1)
		{
			memset(&buffer, 0, sizeof(buffer));
			recv(newSocketId, (char*)&buffer, sizeof(buffer), 0);
			if(!strcmp(buffer, "exit"))
			{
				cout << "Client has quit the session" << endl;
				quit = true;
				break;
			}

			string auctionData(buffer);
			int ret = handleData(auctionData, newSocketId); // @suppress("Function cannot be resolved")

			if (ret == 1) {
				shutdown = true;
				break;
			}
			if (ret == -1) {
				quit = true;
				break;
			}
		}

		if (shutdown) {
			break;
		}
	}
	close(newSocketId);
	close(serverId);
	return 0;
}
