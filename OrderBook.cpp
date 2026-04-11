#include <iostream>
#include "OrderBook.h"
#include <iomanip>

void OrderBook::addOrder(const Order& order) {
    uint32_t remainingQty = order.quantity;

    //Checking if the user is selling or buying
    if (order.side == Side::Buy) {
        //Checking if we have any sell orders with price greater or equal to the buy price
        while (remainingQty > 0 && !asks.empty() && order.price >= asks.begin()->first) {
            auto priceLevelIt = asks.begin();
            double bestAskPrice = priceLevelIt->first;
            //Finding the list of people in that price level
            auto& orderList = priceLevelIt->second;

            //If the selling order has less quanity than buyers then we eat the whole sell order
            while (remainingQty > 0 && !orderList.empty()) {
                //To get the person in the front of the list
                Order& frontOrder = orderList.front();

                //Checking if the front guy has enough quantity to fullfil the buy order
                //If not then we exhaust the order and then move ahead
                if (frontOrder.quantity <= remainingQty) {
                    std::cout<<"----Trade: BUY----" << frontOrder.quantity << " @ " << bestAskPrice << "\n";
                    remainingQty -= frontOrder.quantity;

                    orderLookUp.erase(frontOrder.order_id);
                    orderList.pop_front();
                } 
                //If the sell order has a greater quanitity then we buy the required stocks and reduce the sellers quantity from order
                else {
                    std::cout<<"----Trade: BUY----" << remainingQty << " @ " << bestAskPrice << "\n";
                    frontOrder.quantity -= remainingQty;
                    remainingQty = 0;
                }
            }

            if (orderList.empty()) asks.erase(priceLevelIt);
            
        }
        //If buying quanity is left, we create a bid for it with the quantity remaining
        if (remainingQty > 0) {
            Order newOrder = order;
            newOrder.quantity = remainingQty;
            bids[order.price].push_back(newOrder);
            orderLookUp[newOrder.order_id] = std::prev(bids[order.price].end());
        }

    }
    else {
        uint32_t remainingQty = order.quantity;
    
        //Checking whether if there are any buying orders and whether the price is less than the buying order
        while (remainingQty > 0 && !bids.empty() && order.price <= bids.begin()->first) {
            auto priceLevelIt = bids.begin();
            double bestBidPrice = priceLevelIt->first;
            auto& orderList = priceLevelIt->second;

            while (remainingQty > 0 && !orderList.empty()) {
                Order& frontOrder = orderList.front();
            
                //If the buying order has less quantity than the selling order, we eat the whole buy order
                if (frontOrder.quantity <= remainingQty) {
                    std::cout<<"----Trade: SELL----" << frontOrder.quantity << " @ " << bestBidPrice << "\n";
                    remainingQty -= frontOrder.quantity;
                    orderLookUp.erase(frontOrder.order_id);
                    orderList.pop_front();
                }

                //If the buy order has a greater quanitity then we sell the required stocks and reduce the buyers quantity from order
                else {
                    std::cout<<"----Trade: SELL----" << remainingQty << " @ " <<bestBidPrice << "\n";
                    frontOrder.quantity -= remainingQty;
                    remainingQty = 0;
                }
            }

            //To remove the order list if the price is empty
            if (orderList.empty()) bids.erase(priceLevelIt);

        }

        //If quanuity is left, creating or adding it to an exisiting sell order
        if (remainingQty > 0) {
            Order newOrder = order;
            newOrder.quantity = remainingQty;
            asks[order.price].push_back(newOrder);
            orderLookUp[newOrder.order_id] = std::prev(asks[order.price].end());
        }
    }
};

void OrderBook::printBook() {
    //To print the Order Book Contents
    std::cout<<"\n----/CURRENT MARKET/----\n";
    std::cout<< std::fixed << std::setprecision(2);

    std::cout<<"\n---ASKS(Sellers)----\n";
    //Used to loop through the asks order list, creating a pointer for all the list elements
    for (auto it = asks.rbegin();it != asks.rend(); ++it) {
        u_int32_t totalQuantity = 0;
        //Used to calculate the total quantity, adding all the quantity of the same price 
        for (const auto& order: it->second) totalQuantity += order.quantity;
        std::cout << "Price: " << it->first << "| Quantity: " << totalQuantity << "\n";
    }

    std::cout<<"\n---BUYS(Buyers)----\n";
    //Same as asks but looping through bids list 
    for (auto it = bids.rbegin(); it != bids.rend(); ++it) {
        u_int32_t totalQuantity = 0;
        for (const auto& order: it->second) totalQuantity += order.quantity; 
        std::cout << "Price: " << it->first << "| Quantity: " << totalQuantity << "\n";
    }
};

void OrderBook::cancelOrder(u_int64_t OrderId) {
    //Fail safe to check whether the orderId evens exists. If it doesn't we go back
    if (orderLookUp.find(OrderId) == orderLookUp.end()) {
        std::cout<< OrderId << " not found" << "\n";
        return;
    }

    //Getting the location of the order to be cancelled
    auto it = orderLookUp[OrderId];

    double price = it->price;
    Side side = it->side;

    //Checking if its a buy or sell order
    if (side == Side::Buy) {
        //Deleting the buy order of that specific trader rather than the whole price order
        bids[price].erase(it);
        //deleting the whole buy order if its empty
        if (bids[price].empty()) bids.erase(price);
    }
    else {
        //Deleting the sell order of that specific trader rather than the whole price order
        asks[price].erase(it);
        //deleting the whole sell order if its empty
        if (asks[price].empty()) asks.erase(price);
    }

    //Deleting the orderId of the cancelled order
    orderLookUp.erase(OrderId);
    std::cout<<"Order "<< OrderId << " Cancelled" << "\n";
};