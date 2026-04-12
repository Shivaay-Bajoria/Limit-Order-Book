#include "Types.h"
#pragma once
#include <unordered_map>
#include <list>
#include <map>

//Normally map sorts from lowest to highest value
class OrderBook {
    private: 
        //uint64_t is the price of the stock
        //list<order> is used to keep an ordered list of the orders coming in
        std::map<uint64_t, std::list<Order>> asks;
        //Using greater<double> to sort it from highest to lowest
        std::map<uint64_t, std::list<Order>, std::greater<uint64_t> bids;

        //Using to find the order the trader wants to cancel
        std::unordered_map<uint64_t, std::list<Order>::iterator> orderLookUp;

    public:
    //We are passing the reference of Order using '&' to tell the compiler to not make new copies when adding orders
    //And to stop it from making changes to the prices, we add the keyword const, to allow the compiler to just look at the order
        void addOrder(const Order& order);
        void printBook();
        void cancelOrder(uint64_t orderId);
};



