import json

with open('data.json') as data_file:
    data = json.load(data_file)

f = open("data.csv", "w")

for d in data['data']:
    time = data['data'][d]['DevEUI_uplink']['Time']
    payload = bytearray.fromhex(data['data'][d]['DevEUI_uplink']['payload_hex']).decode().split('|')
    temp = float(payload[1]) / 100

    if temp < 100.0:
        print("{:s},{:s},{:.2f}".format(time, payload[0],temp))
        f.write("{:s},{:s},{:.2f}\n".format(time, payload[0],temp))