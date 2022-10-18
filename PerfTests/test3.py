import aiohttp
import asyncio
import json
from concurrent.futures import ThreadPoolExecutor

"""

$ curl --data '{"jsonrpc": "2.0", "method": "transfer", "params": ["sendfrom", "sendto", "0.01", "RVP", "Here is the cash", "true"], "id": 1}' http://127.0.0.1:8090/rpc  | json_pp

"""


####  config  ####

host = "172.17.0.5"
port = 8093

workersnumber    = 8
blockinterval    = 5
numbertxperblock = 500

sendfrom = "test022"
sendto   = "test019"
summ     = "1.001"

#### #### #### ####

async def make_transfer(session, request_id):
    query = {
        "jsonrpc": "2.0",
        "method": "transfer",
        "params": [sendfrom, sendto, summ, "RVP", "Here is the cash", "true"],
        "id": request_id,
    }
    url     = f"http://{host}:{port}/rpc"
    payload = json.dumps(query, ensure_ascii=False).encode("utf8")
    headers = {"content-type": "application/json"}

    async with session.post(url, data=payload, headers=headers) as resp:
        print(request_id, resp.status);
        if resp.status != 200:
            transfer = await resp.json(content_type=None)
            print(request_id, json.dumps(transfer, indent=4))

async def transfer_processor(worker_id):
    async with aiohttp.ClientSession() as session:
        tasks = []
        for request_id in range(numbertxperblock): # Build Requests
            worker_request_id = worker_id * 10000 + request_id
            tasks.append(asyncio.ensure_future(make_transfer(session, worker_request_id)))
        await asyncio.gather(*tasks) # Exec all the requests

def transfer_processor_wrapper(worker_id):
    asyncio.run(transfer_processor(worker_id))

async def main():
    worker = ThreadPoolExecutor(workersnumber)
    while True :
        print ("Start new block:")
        for worker_id in range(1, workersnumber+1):
            worker.submit(transfer_processor_wrapper, worker_id)
            #await loop.run_in_executor(transfer_processor, payment, worker_id)
        await asyncio.sleep(blockinterval)

asyncio.run(main())
