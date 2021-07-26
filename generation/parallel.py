#!/usr/bin/env python3
from time import sleep
import shlex
import subprocess
import os
from sys import argv, stdin, exit

if len(argv) != 3:
    print(f"Usage: {argv[0]} <nprocs> <process name to search for name>")
    exit(1)

nprocs = int(argv[1])
proc_name = argv[2]

jobs = []
for line in stdin:
    jobs.append(line)
jobs.reverse()

while jobs:
    running_processes = len(
        subprocess.run(
            ["bash", "-c", f"ps aux | grep {proc_name} | grep -v grep | grep -v 'parallel.py'"],
            capture_output=True
        ).stdout.decode("utf8").splitlines()
    )
    print("Running", running_processes)
    for i in range(nprocs - running_processes):
        if jobs:
            subprocess.Popen(shlex.split(jobs.pop()))
    os.wait3(0) # Wait for a process to finish
