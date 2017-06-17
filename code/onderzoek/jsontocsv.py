import json
from dateutil import parser


with open('school_temp_door_22-5-17_26-5-17.json') as data_file:
    data = json.load(data_file)

f = open("gnuplot_fmt.dat", "w")

for d in data['data']:
    time = dt = parser.parse(data['data'][d]['DevEUI_uplink']['Time'])
    payload = bytearray.fromhex(data['data'][d]['DevEUI_uplink']['payload_hex']).decode().split('|')
    temp = float(payload[1]) / 100

    if temp < 100.0:
        print("{:d} {:s} {:.2f}".format(int(time.timestamp()), payload[0],temp))
        f.write("{:d} {:s} {:.2f}\n".format(int(time.timestamp()), payload[0],temp))