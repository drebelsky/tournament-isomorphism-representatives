Note: while some of the scripts are set up to support arbitrary tournament sizes, others will only work up to size 8, and others may only work for size exactly 8. The rough process to recreate the results are given below.

Running `make` will construct two executables: `main` and `combine`.

* `main` is generated from `main.cpp` and is the primary code for finding representatives and duplicates. `main` takes in three arguments: `n`, `start`, and `stop`. `n` is the number of nodes to construct the tournament from (integers less than or equal to 8 should work fine). The program works by incrementing an integer treating it as a bit mask for the edges in the tournament. The integer starts at `start` and stops before `stop`, so to get the full range of possible graphs for a given `n`, `start` should be 0 and stop should be 2^{n * (n - 1) / 2}. `start` and `stop` are individually controllable to allow parallel execution (and multiprocessing was chosen over multithreading to support running on multiple computers).

* `parallel.py` is a utility script to queue tasks in parallel. It takes in two arguments: the number of processes to keep alive and the process name to search for. Commands to execute are passed in via stdin and are executed using bash.

Once all the files are generated from invoking `main` (perhaps using `parallel.py`), they can be moved to a folder named `downloads` in the same directory. A directory named `aggregate` should be created. At this point, `aggregate.py` can be run. `aggregate.py` takes in the number of processes to keep running and takes the ranges to combine from stdin formatted as follows.

```
start1 end1
start2 end2
start3 end3
```

where as in `main`, `start`s are inclusive and `end`s are exclusive. `aggregate.py` makes use of the `combine` executable.

Once `aggregate.py` has run, `finalize.py` can be run (although line 31 should be updated with the last file created in the `aggregate` directory (which should be the one with the highest number)).

Finally, to generate the `graphs.txt` file, run `make_adj_matrix.py`.
