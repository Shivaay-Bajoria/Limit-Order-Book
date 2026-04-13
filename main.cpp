#include "OrderBook.h"
#include "Types.h"
#include "json.hpp"
#include <iostream>
#include <string>
#include "Exchange.h"
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using json = nlohmann::json;

int main() {

    //Creating our TCP Socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    //To specify we are using port 5555
    address.sin_port = htons(5555); 

    //Binding the socket
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    OrderBook book;
    Exchange newExchange;

    std::cout<<"------Started------\n";
    std::cout<<"waiting for python to connect to port 5555\n";

    int python_socket = accept(server_fd, nullptr, nullptr);
    std::cout << "Python Bot Connected! Starting data feed...\n";

    char buffer[1024] = {0};

    //Running a continous loop, for two-way connection
    while(true) {
        int valread = read(python_socket, buffer, 1024);
        //Disconnect pytyon, if no data sent by python
        if (valread <= 0) break; 

        //Getiing the input from python
        std::string json_order(buffer, valread);

        try {
            //Parsing the incoming data 
            json parsedData = json::parse(json_order);

            std::string symbol = parsedData["symbol"];

            //Converting the json to our C++ struct
            Order incomingOrder;
            incomingOrder.order_id = parsedData["order_id"];
            incomingOrder.price = parsedData["price"];
            incomingOrder.quantity = parsedData["quantity"];
            incomingOrder.side = (parsedData["side"] == "buy") ? Side::Buy : Side::Sell;

            //Sending into our exchange, so that it can create the order book in the required order book
            newExchange.processOrder(symbol, incomingOrder);

            //Printing the order book of the symbol which has changed
            newExchange.printOrder(symbol);

            //Sending the AK back to python
            std::string response = "{\"status\": \"CONFIRMED\", \"message\": \"Order received by C++ Engine\"}";
            send(python_socket, response.c_str(), response.length(), 0);
            
        } catch (const json::parse_error& e) {
            std::cerr << "Network collision detected, dropping malformed packet.\n";
        }
    }

    close(python_socket);
    close(server_fd);
    return 0;
}