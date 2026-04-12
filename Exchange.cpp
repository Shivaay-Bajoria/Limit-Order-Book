#include "Exchange.h"
#include <iostream>

void Exchange::processOrder(const std::string& symbol, const Order& order) {
    //Main routing code to send the code to the required symbol
    orderBooks[symbol].addOrder(order);
};

void Exchange::cancelOrder(const std::string& symbol, uint64_t orderId) {
    //Checking if the order is even there in the book or not
    if (orderBooks.find(symbol) != orderBooks.end()) {
        orderBooks[symbol].cancelOrder(orderId);
    } else {
        std::cout<<"Order ID " << orderId << " not found.";
    }
};

void Exchange::printOrder(const std::string& symbol) {
    if (orderBooks.find(symbol) != orderBooks.end()) {
        std::cout << "Market Orders -------" << symbol << "----\n";
        orderBooks[symbol].printBook();
    } else {
        std::cout << "Symbol " << symbol << " not found in the books";
    }
};