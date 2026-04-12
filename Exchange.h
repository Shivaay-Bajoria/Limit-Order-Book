#pragma once
#include "OrderBook.h"
#include <unordered_map>
#include <string>

class Exchange {

    private:
        //Creating a map to store all the orderBooks inside an exchange, for easy differentiation when changing the symbols
        std::unordered_map<std::string, OrderBook> orderBooks;

    public:
    //Creating the respective functions for managing the exchanges
        void processOrder(const std::string& symbol, const Order& order);
        void cancelOrder(const std::string& symbol, uint64_t orderId);
        void printOrder(const std::string& symbol);
};