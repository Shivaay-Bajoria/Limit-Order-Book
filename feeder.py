import asyncio
import websockets
import json
import sys

#The coinbase link which we will use to stream the data for bitcoin and ethereum
COINBASE_WS_URL = "wss://ws-feed.exchange.coinbase.com"

async def feed_engine():
    #Connecting with binance
    async with websockets.connect(COINBASE_WS_URL) as websocket:

        subscribe_message = {
            "type": "subscribe",
            "product_ids": ["BTC-USD", "ETH-USD"],
            #used to say that we want live trades
            "channels": ["matches"]
        }

        await websocket.send(json.dumps(subscribe_message))

        #To create a infinite loop
        while True:
            try:
                #Recieving data
                message = await websocket.recv()
                data = json.loads(message)

                if data.get('type') == 'match': 

                    #formatting the data into the applicable c++ format
                    formatted_order = {
                        "exchange": "Coinbase",
                        "symbol": data['product_id'],
                        "order_id": int(data['trade_id']),
                        "side": data['side'],
                        #To elimate floating numbers and convert the price to integer, so that it can fit into uint64_t
                        #Multiplying by 3 to remove cents
                        "price": int(float(data['price']) * 100),
                        #To elimate floating numbers and convert the quanities to integer, so that it can fit into uint64_t
                        "quantity": int(float(data['size']) * 100000000),
                    }

                    #flush=True is imp to send the json to C++
                    print(json.dumps(formatted_order), flush=True)

            except websockets.exceptions.ConnectionClosed:
                print(json.dumps({"error": "WebSocket Connection Closed"}), flush=True)
                break

            except Exception as e:
                pass

if __name__ == "__main__":
    try:
        asyncio.run(feed_engine())
    except KeyboardInterrupt:
        sys.exit(0)