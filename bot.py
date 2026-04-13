import sys

class MomentumBot:
    def __init__(self, starting_usd = 100000):
        self.fake_usd = starting_usd
        self.fake_btc = 0.0
        self.bot_order_id = 9000

        self.price_history = []
        self.WINDOW_SIZE = 10

    def process_trade(self, live_price_float, live_price_ticks):
        #Takes the current price of the market, runs the algorithm to decide if it wants to trade
        orders_to_place = []
        self.price_history.append(live_price_float)

        if (len(self.price_history) > self.WINDOW_SIZE):
            self.price_history.pop(0)
        
            moving_average = sum(self.price_history) / self.WINDOW_SIZE

            #If price is surging, but BTC
            if live_price_float > moving_average + 2.0 and self.fake_usd >= live_price_float:
                self.fake_usd -= live_price_float
                self.fake_btc += 1.0

                #We are not sedning the order just creating a dict
                orders_to_place.append({
                    "exchange": "ALGO_BOT", "symbol": "BTC-USD", "order_id": self.bot_order_id,
                    "side": "buy", "price": live_price_ticks, "quantity": 100000000
                })
                self.bot_order_id += 1
                self.price_history.clear()

            #If price is crashing sell 1 BTC
            elif live_price_float < moving_average - 2.0 and self.fake_btc >= 1.0:
                self.fake_usd += live_price_float
                self.fake_btc -= 1.0

                orders_to_place.append({
                    "exchange": "ALGO_BOT", "symbol": "BTC-USD", "order_id": self.bot_order_id,
                    "side": "sell", "price": live_price_ticks, "quantity": 100000000
                })
                self.bot_order_id += 1
                self.price_history.clear()

            #To track the amount which we have
            net_worth = self.fake_usd + (self.fake_btc * live_price_float)
            sys.stderr.write(f"\033[33m[BOT PNL] USD: ${self.fake_usd:.2f} | BTC: {self.fake_btc} | NET WORTH: ${net_worth:.2f}\033[0m\n")
        
        return orders_to_place



