#!/usr/bin/env python3
from sys import argv, stdin, exit
from time import sleep
import heapq
import os
import shlex
import subprocess

if len(argv) != 2:
    print(f"Usage: {argv[0]} <nprocs>")
    exit(1)

nprocs = int(argv[1])
aggregate_info = open("agg_info.txt", "w")

class File:
    def __init__(self, filename: str, size: int):
        self.filename = filename
        self.size = size

    def __leq__(self, other):
        return self.size <= other.size

    def __lt__(self, other):
        return self.size < other.size

combined = []
for line in stdin:
    start, stop = map(int, line.split())
    heapq.heappush(combined, File(f"download/8_{start}-{stop}.txt.new", stop - start))

# From pid to ranges
children = {}

out_num = 0
available = nprocs
while len(combined) > 1 or children:
    print(len(combined))
    for i in range(available):
        if len(combined) >= 2:
            left = heapq.heappop(combined)
            right = heapq.heappop(combined)
            out_num += 1
            filename = f"aggregate/{out_num}.agg"
            pid = subprocess.Popen(["./combine", filename, left.filename, right.filename]).pid
            children[pid] = File(filename, left.size + right.size)
            print(filename, left.filename, right.filename, left.size + right.size, sep=',', file=aggregate_info)
            available -= 1
    try:
        pid = os.wait3(0)[0] # Wait for a process to finish
        available += 1
    except ChildProcessError:
        if len(combined) > 1:
            raise
        else:
            break
    heapq.heappush(combined, children[pid])
    del children[pid]

print("Finished?", children)
