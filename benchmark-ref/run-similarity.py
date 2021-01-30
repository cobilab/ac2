#!/usr/bin/python

import sys
import subprocess

from os import listdir
from os.path import isfile, join, splitext, getsize

refs_dir = sys.argv[1]
targets_dir = sys.argv[2]


refs_files = [join(refs_dir, f) for f in listdir(refs_dir) if isfile(join(refs_dir, f)) and splitext(f)[1] == '.seq']
targets_files = [join(targets_dir, f) for f in listdir(targets_dir) if isfile(join(targets_dir, f)) and splitext(f)[1] == '.seq']

for r in refs_files:
	result = subprocess.run(['./AC2', '-tm', '1:1:1:0.9/1:1:0.9', '-tm', '3:10:1:0.95/2:10:0.96', '-tm', '5:200:5:0.98/4:50:0.95', r], stdout=subprocess.PIPE)
	bps_r = float(result.stdout.decode('utf-8').split("bps w/ no header")[0].split("bps, ")[1])
	for t in targets_files:
		result = subprocess.run(['./AC2', '-tm', '1:1:1:0.9/1:1:0.9', '-tm', '3:10:1:0.95/2:10:0.96', '-tm', '5:200:5:0.98/4:50:0.95', t], stdout=subprocess.PIPE)
		bps_t = float(result.stdout.decode('utf-8').split("bps w/ no header")[0].split("bps, ")[1])
		
		
		result = subprocess.run(['./AC2', '-rm', '1:1:1:0.9/1:1:0.9', '-rm', '3:10:1:0.95/2:10:0.96', '-rm', '5:200:5:0.98/4:50:0.95', '-rm', '7:1500:5:0.955/6:50:0.945', '-tm', '1:1:1:0.9/1:1:0.9', '-tm', '3:10:1:0.95/2:10:0.96', '-tm', '5:200:5:0.98/4:50:0.95', '-r', r, t], stdout=subprocess.PIPE)
		bps_rt = float(result.stdout.decode('utf-8').split("bps w/ no header")[0].split("bps, ")[1])
		
		
		result = subprocess.run(['./AC2', '-rm', '1:1:1:0.9/1:1:0.9', '-rm', '3:10:1:0.95/2:10:0.96', '-rm', '5:200:5:0.98/4:50:0.95', '-rm', '7:1500:5:0.955/6:50:0.945', '-r', t, r], stdout=subprocess.PIPE)
		bps_tr = float(result.stdout.decode('utf-8').split("bps w/ no header")[0].split("bps, ")[1])
		
		with open(t.split('.seq')[0]) as tf:
			print(i, r, t, bps_r, bps_t, bps_rt, bps_tr, max(bps_rt, bps_tr) / max(bps_t, bps_r), tf.readline().strip().replace(" ", "___"))
	
