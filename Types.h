#pragma once
#include <cstdint>

//This file tells the compiler what all declarations of classes and structures will be used in the main program
//Using '.h' tells that we will be declaring whatever exsits in our code

enum class Side {
    Buy,
    Sell
};

struct Order {
    uint64_t order_id; //Massive size for order id
    Side side;
    double price;
    uint32_t quantity;
};