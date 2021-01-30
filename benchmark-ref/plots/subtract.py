#!/usr/bin/python

import sys
import subprocess

result = subprocess.run(['wc', '-c', sys.argv[1]], stdout=subprocess.PIPE)
length = int(result.stdout.decode('utf-8').split(" ")[0])

window_size = str(int(length / 1000))

with open(sys.argv[1]  + ".s", "w") as sf1, open(sys.argv[2] + ".s", "w") as sf2, open(sys.argv[1]) as f1, open(sys.argv[2]) as f2:
	subprocess.run(['gto_filter', '-p', '-c', '-w', window_size], stdin=f1, stdout=sf1)
	subprocess.run(['gto_filter', '-p', '-c', '-w', window_size], stdin=f2, stdout=sf2)


with open(sys.argv[1]  + ".s") as sf1, open(sys.argv[2]  + ".s") as sf2:
	for l1, l2 in zip(sf1, sf2):
		print(float(l1) - float(l2))
