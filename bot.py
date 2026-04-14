import sys
import time
import csv
from datetime import datetime

class MomentumBot:
    def __init__(self, starting_usd = 100000):
        self.fake_usd = starting_usd
        self.fake_btc = 0.0
        self.bot_order_id = 9000

        #Creating the candles brain of the bot
        self.candle_prices = []
        self.candle_start_time = time.time()
        self.CANDLE_DURATON = 5.0

        self.historical_candles = []
        self.WINDOW_SIZE = 5

        #To create CSV File
        self.csv_file = open('bot_performance.csv', mode='w', newline='')
        self.csv_writer = csv.writer(self.csv_file)
        self.csv_writer.writerow(['Timestamp', 'Fake_USD', 'Fake_BTC', 'Net_Worth'])

    def process_trade(self, live_price_float, live_price_ticks):
        #Takes the current price of the market, runs the algorithm to decide if it wants to trade
        orders_to_place = []
        current_time = time.time()

        self.candle_prices.append(live_price_float)

        if current_time - self.candle_start_time >= self.CANDLE_DURATON:

            #Calculating the average of the past 5 seconds
            candle_close_price = sum(self.candle_prices) / len(self.candle_prices)
            self.historical_candles.append(candle_close_price)

            #Resetting the timer
            self.candle_start_time = time.time()
            self.candle_prices.clear()

            if (len(self.historical_candles) > self.WINDOW_SIZE):
                self.historical_candles.pop(0)
            
                moving_average = sum(self.historical_candles) / self.WINDOW_SIZE

                #If price is surging, but BTC
                if candle_close_price > moving_average + 5.0 and self.fake_usd >= candle_close_price:
                    self.fake_usd -= candle_close_price
                    self.fake_btc += 1.0

                    #We are not sedning the order just creating a dict
                    orders_to_place.append({
                        "exchange": "ALGO_BOT", "symbol": "BTC-USD", "order_id": self.bot_order_id,
                        "side": "buy", "price": live_price_ticks, "quantity": 100000000
                    })
                    self.bot_order_id += 1

                #If price is crashing sell 1 BTC
                elif candle_close_price < moving_average - 5.0 and self.fake_btc >= 1.0:
                    self.fake_usd += candle_close_price
                    self.fake_btc -= 1.0

                    orders_to_place.append({
                        "exchange": "ALGO_BOT", "symbol": "BTC-USD", "order_id": self.bot_order_id,
                        "side": "sell", "price": live_price_ticks, "quantity": 100000000
                    })
                    self.bot_order_id += 1

                #To track the amount which we have every 5 seconds as well as loggin to excel
                net_worth = self.fake_usd + (self.fake_btc * live_price_float)
                timestamp = datetime.now().strftime('%H:%M:%S')

                self.csv_writer.writerow([timestamp, round(self.fake_usd, 2), self.fake_btc, round(net_worth, 2)])
                self.csv_file.flush()

                sys.stderr.write(f"\033[33m[BOT PNL] USD: ${self.fake_usd:.2f} | BTC: {self.fake_btc} | NET WORTH: ${net_worth:.2f}\033[0m\n")
        
        return orders_to_place



