def make_edges(n):
    edges = []
    i = 1
    for u in range(n):
        edges.append([])
        for v in range(n):
            if u == v:
                edges[u].append(0)
            elif v < u:
                edges[u].append(-edges[v][u])
            else:
                edges[u].append(i)
                i <<= 1
    return edges

def get_edge(edge_index, graph):
    if (edge_index < 0):
      edge_dir = True
      edge_index *= -1
    else:
      edge_dir = False

    if graph & edge_index:
      edge_dir = not edge_dir

    return edge_dir

def make_adj_matrix(graph, edges, n):
    res = []
    for u in range(n):
        res.append([])
        for v in range(n):
            res[-1].append(1 if get_edge(edges[u][v], graph) else 0)
    return res

edges = make_edges(8)
with open("representatives.txt") as input_file, open("adjacency.txt", "w") as output_file:
    for line in input_file:
        graph, count = map(int, line.strip().split())
        print(make_adj_matrix(graph, edges, 8), count, file=output_file)
