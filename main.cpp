#include "OrderBook.h"
#include "Types.h"
#include "json.hpp"
#include <iostream>
#include <string>
#include "Exchange.h"

using json = nlohmann::json;

int main() {
    OrderBook book;
    std::string incomingLine;
    Exchange newExchange;

    std::cout<<"------Started------Listening for orders\n";

    //Running a continous loop, to always get the value, to take the inputs without stopping
    while(std::getline(std::cin, incomingLine)) {

        //Checking if input is empty, then skipping
        if (incomingLine.empty()) continue;

        try {
            //Parsing the incoming data 
            json parsedData = json::parse(incomingLine);

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

        } 
        catch (const json:: exception& e) {
            std::cerr<<"JSON Parsing Error " << e.what() << "\n";
        }
    }
    return 0;
}