#!/usr/bin/python

import sys
import subprocess

from os import listdir, rename
from os.path import isfile, join, splitext, getsize

refs_dir = sys.argv[1]
targets_dir = sys.argv[2]


refs_files = sorted([join(refs_dir, f) for f in listdir(refs_dir) if isfile(join(refs_dir, f)) and splitext(f)[1] == '.seq'])
targets_files = sorted([join(targets_dir, f) for f in listdir(targets_dir) if isfile(join(targets_dir, f)) and splitext(f)[1] == '.seq'])

i = 0


def bps(fname, bits):
	return float(bits.split(',')[0])/getsize(fname)
	
def toid(fname):
	if("UP000005640" in fname):
		return "HS"
	if("UP000001519" in fname):
		return "GG"
	if("UP000001595" in fname):
		return "PA"
	if("UP000002277" in fname):
		return "PT"

for r in refs_files:
	for t in targets_files:
		if(t.split()[2] == r.split()[2]):
			
			result = subprocess.run(['./AC2', '-rm', '1:1:1:0.9/1:1:0.9', '-rm', '3:10:1:0.95/2:10:0.96', '-rm', '5:200:5:0.98/4:50:0.95', '-rm', '7:1500:5:0.955/6:50:0.945', '-rm', '15:2000:15:0.94/14:250:0.92', '-tm', '1:1:1:0.9/1:1:0.9', '-tm', '3:10:1:0.95/2:10:0.96', '-tm', '5:200:5:0.98/4:50:0.95', '-e', '-r', r, t], stdout=subprocess.PIPE)
			ac2_bps_rt = float(result.stdout.decode('utf-8').split("bps w/ no header")[0].split("bps, ")[1])
			ac_bps_rt = bps(t, result.stdout.decode('utf-8').split()[20])
			
			print(r, t, toid(r), toid(t), ac2_bps_rt, ac_bps_rt)
			i = i + 1

