#include "OrderBook.h"
#include "Types.h"
#include <iostream>

int main() {
    OrderBook book;

    std::cout << "\n--- PHASE 1: Building the Queue --- \n";
    // Three sellers all want $200.00
    // Because it's a list, ID 101 is first in line, 102 is second, 103 is third.
    book.addOrder({101, Side::Sell, 200.0, 10});
    book.addOrder({102, Side::Sell, 200.0, 15});
    book.addOrder({103, Side::Sell, 200.0, 20});
    
    // There should be 45 total quantity at $200.00
    book.printBook();


    std::cout << "\n--- PHASE 2: The Precision Cancel --- \n";
    // The middle guy (ID 102) cancels. 
    // This should remove 15 quantity, leaving exactly 30 at $200.00.
    book.cancelOrder(102);
    book.printBook();


    std::cout << "\n--- PHASE 3: The Whale Match --- \n";
    // A buyer wants to buy 25 units at up to $205.00.
    // They should buy 10 units from ID 101 (who was first in line).
    // ID 102 is gone, so they buy their remaining 15 units from ID 103.
    book.addOrder({201, Side::Buy, 205.0, 25});
    
    // There should only be 5 units left at $200.00 (ID 103's leftovers).
    book.printBook();

    return 0;
}