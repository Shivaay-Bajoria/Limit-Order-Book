#include <iostream>
#include "OrderBook.h"
#include <iomanip>
#include <vector>

void OrderBook::addOrder(const Order& order) {
    uint64_t remainingQty = order.quantity;

    //Checking if the user is selling or buying
    if (order.side == Side::Buy) {
        //Checking if we have any sell orders with price greater or equal to the buy price
        while (remainingQty > 0 && !asks.empty() && order.price >= asks.begin()->first) {
            auto priceLevelIt = asks.begin();
            uint64_t bestAskPrice = priceLevelIt->first;
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
            uint64_t bestBidPrice = priceLevelIt->first;
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
    //Used to clear the terminal and move the cursor to top left
    std::cout<<"\033[2J\033[H";

    std::cout << "=================================================\n";
    std::cout << "        LIVE LIMIT ORDER BOOK (Top 10)           \n";
    std::cout << "=================================================\n\n";

    //Displaying the asks (sellers)
    //To set the terminal text to RED
    std::cout << "\033[31m"; 
    std::cout << "      PRICE (Ticks)     |     VOLUME (Satoshis)  \n";
    std::cout << "-------------------------------------------------\n";

    //Collecting the top 10 asks for the given symbol
    std::vector<std::pair<uint64_t, uint64_t>> bestAsks;
    int askCount = 0;
    //Using for loop to go through the top 10 asks value
    for (auto it = asks.begin(); it != asks.end() && askCount < 10; ++it) {
        uint64_t totalQty = 0;
        //Used to calculate the total quantity, adding all the quantity of the same price
        for (const auto& order: it->second) totalQty += order.quantity;
        bestAsks.push_back({it->first, totalQty});
        askCount++;
    }
    
    //And then we print the best asks
    for (auto it = bestAsks.rbegin(); it != bestAsks.rend(); ++it) {
        std::cout << "      " << it->first << "         |     " << it->second << "\n";
    }

    // Reset terminal text to DEFAULT
    std::cout << "\033[0m";
    std::cout << "\n------------------- SPREAD ----------------------\n\n";

    //Displaying the bids(buyers)
    //To set the terminal text to green
    std::cout << "\033[32m";
    std::cout << "      PRICE (Ticks)     |     VOLUME (Satoshis)  \n";
    std::cout << "-------------------------------------------------\n";

    //Same as asks but looping through bids list 
    std::vector<std::pair<uint64_t, uint64_t>> bestBids;
    int bidCount = 0;
    for (auto it = bids.begin(); it != bids.end() && bidCount < 10; ++it) {
        uint64_t totalQty = 0;
        for (const auto& order: it->second) totalQty += order.quantity;
        bestBids.push_back({it->first, totalQty});
        bidCount++;
    }

    //Printing the best bids
    for (auto it = bestBids.begin(); it != bestBids.end(); ++it) {
        std::cout << "      " << it->first << "         |     " << it->second << "\n";
    }


    // Reset terminal text to DEFAULT
    std::cout << "\033[0m"; 
    std::cout << "\n=================================================\n";
};

void OrderBook::cancelOrder(uint64_t OrderId) {
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