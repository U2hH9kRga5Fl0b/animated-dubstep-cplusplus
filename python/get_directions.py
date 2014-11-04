#!/usr/bin/python

import time
import json
import urllib
import decodeencode
import os
import shutil
import random
import sys

def save_directions(begin, end, filename):
	time.sleep(3)
	depuretime=int(time.time())
	j = json.loads(urllib.urlopen("https://maps.googleapis.com/maps/api/directions/json?alternatives=false&origin=" + begin + "&destination=" + end + "&departure_time=" + str(depuretime) + '&mode=driving&language=en-US').read())
	l=0
	try:
		j['routes'][0]
	except:
		print('unable to get directions:');
		print(j);
		return;
	
	for leg in j['routes'][0]['legs']:
		f = open(filename + str(l) + '.txt', 'w')
		l=l+1
		
		f.write(str(depuretime));
		f.write('\n')
		f.write(str(leg['distance']['value']))
		f.write('\n')
		f.write(str(leg['duration']['value']))
		f.write('\n')
		
		f.write(str(leg['start_address']))
		f.write('\n')
		f.write(str(leg['start_location']['lng']))
		f.write('\t')
		f.write(str(leg['start_location']['lat']))
		f.write('\n')
		
		f.write(str(leg['end_address']))
		f.write('\n')
		f.write(str(leg['end_location']['lng']))
		f.write('\t')
		f.write(str(leg['end_location']['lat']))
		f.write('\n')
		
		i=0
		for step in leg['steps']:
			f.write(str(i))
			f.write('\n\t')
			f.write(str(step['distance']['value']))
			f.write('\t')
			f.write(str(step['duration']['value']))
			f.write('\n\t')
			f.write(str(step['start_location']['lng']))
			f.write('\t')
			f.write(str(step['start_location']['lat']))
			f.write('\n\t')
			f.write(str(step['end_location']['lng']))
			f.write('\t')
			f.write(str(step['end_location']['lat']))
			f.write('\n\t')
			f.write(str(decodeencode.decode(str(step['polyline']['points']))))
			f.write('\n')
			i=i+1
		f.close()

# Denver lower bound:  39.558185, -105.226243
# Denver upper bound:  40.211516, -104.745591
def get_random_point():
    return str(random.uniform(39.558185, 40.211516)) + "," + str(random.uniform(-105.226243, -104.745591))

def save_several_directions(num):
    i=0
    while i < num:
        try:
            save_directions(get_random_point(), get_random_point(), "./directions/directions." + str(time.time()));
            print("Saved " + str(i));
        except:
	    pass
        i=i+1

if len(sys.argv) < 2:
    print("Usage: " + sys.argv[0] + " <number of queries to make>")
else:
    save_several_directions(int(sys.argv[1]));


