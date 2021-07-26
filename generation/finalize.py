from os import listdir, path
from functools import cache, lru_cache

def parse_perm_line(line):
    graph, rest = line.strip().split(":")
    parent, permutation = rest.split("(")
    permutation = permutation.rstrip(")")
    graph = int(graph.strip())
    parent = int(parent.strip())
    permutation = int(permutation.strip())
    return (graph, parent, permutation)

representatives = {}
path_info = {}

@cache
def get_parent(graph):
    if path_info[graph] is None:
        return graph
    return get_parent(path_info[graph][0])

@cache
def get_path(graph):
    if path_info[graph] is None:
        return "new"
    parent, perm = path_info[graph]
    return f"{parent}({perm}) " + get_path(parent)



with open("aggregate/2684.agg") as f:
    for line in f:
        graph = int(line.split(":")[0].strip())
        representatives[graph] = 0
        path_info[graph] = None
i = 0
for filename in listdir("aggregate"):
    if not filename.endswith("dups"):
        continue
    print(i)
    i += 1
    with open(path.join("aggregate", filename)) as f:
        for line in f:
            graph, parent, permutation = parse_perm_line(line)
            path_info[graph] = (parent, permutation)

i = 0
#paths_file = open("paths.txt", "w")
from os import devnull
paths_file = open(devnull, "w")
for filename in listdir("download"):
    # Clearing the caches leads to a performance hit, but benefits memory usage
    get_parent.cache_clear()
    get_path.cache_clear()
    with open(path.join("download", filename)) as f:
        print(i)
        i += 1
        for line in f:
            if "new" in line:
                graph = int(line.split(":")[0])
                representatives[get_parent(graph)] += 1
                print(f"{graph}:", get_path(graph), file=paths_file)
            else:
                graph, parent, permutation = parse_perm_line(line)
                representatives[get_parent(parent)] += 1
                print(f"{graph}: {parent}(perm)", get_path(parent), file=paths_file)
paths_file.close()

with open("representatives.txt", "w") as f:
    for rep, count in representatives.items():
        print(rep, count, file=f)
