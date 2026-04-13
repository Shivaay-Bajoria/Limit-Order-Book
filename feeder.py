import asyncio
import websockets
import json
import sys
import socket
from bot import MomentumBot

COINBASE_WS_URL = "wss://ws-feed.exchange.coinbase.com"

#Coneecting to C++
engine_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
engine_socket.connect(('127.0.0.1', 5555))
print("Connected to C++ to port 5555")

#Creating the bot inside the feeder.py
algo_bot = MomentumBot()


async def feed_engine():
    global fake_usd, fake_btc, bot_order_id, price_history
    
    async with websockets.connect(COINBASE_WS_URL) as websocket:
        subscribe_message = {
            "type": "subscribe",
            "product_ids": ["BTC-USD"],
            "channels": ["matches"] 
        }
        await websocket.send(json.dumps(subscribe_message))

        while True:
            try:
                message = await websocket.recv()
                data = json.loads(message)
                
                if data.get('type') == 'match':
                    live_price_float = float(data['price'])
                    live_price_ticks = int(live_price_float * 100)
                    
                    # 1. Pipe the live internet trade to C++
                    live_order = {
                        "exchange": "Coinbase",
                        "symbol": data['product_id'],
                        "order_id": int(data['trade_id']),
                        "side": data['side'], 
                        "price": live_price_ticks,
                        "quantity": int(float(data['size']) * 100000000) 
                    }

                    #Need to send live_order to c++ through the socket
                    engine_socket.sendall(json.dumps(live_order).encode('utf-8'))
                    engine_socket.recv(1024)
                    
                    #Now calling the bot and seeing what the bot wants to do
                    bot_orders = algo_bot.process_trade(live_price_float=live_price_float, live_price_ticks=live_price_ticks)
                    
                    #Now sending the bot orders to c++
                    for order in bot_orders:
                        engine_socket.sendall(json.dumps(order).encode('utf-8'))
                        response_bytes = engine_socket.recv(1024)
                        response_data = json.loads(response_bytes.decode('utf-8'))
                        sys.stderr.write(f"\033[34m[C++ SAYS] {response_data['status']} - BOT ORDER INJECTED\033[0m\n")

            except websockets.exceptions.ConnectionClosed:
                break
            except Exception as e:
                pass 

if __name__ == "__main__":
    try:
        asyncio.run(feed_engine())
    except KeyboardInterrupt:
        #To see the final values
        print(f"\n\n--- TRADING SESSION ENDED ---")
        print(f"Final USD: ${algo_bot.fake_usd:.2f}")
        print(f"Final BTC: {algo_bot.fake_btc}")
        sys.exit(0)