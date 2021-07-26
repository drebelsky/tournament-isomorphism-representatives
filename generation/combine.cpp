#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::array;
using std::cout;
using std::next_permutation;
using std::string;
using std::vector;

// TODO
using std::endl;

inline bool get_edge(long edge_index, long graph) {
  bool edge_dir;
  if (edge_index < 0) {
    edge_dir = true;
    edge_index *= -1;
  } else {
    edge_dir = false;
  }

  if (graph & edge_index) {
    edge_dir = !edge_dir;
  }
  return edge_dir;
}

bool is_identical(const vector<vector<long>> &edges, int n, long graph1,
                  long graph2, const vector<int> &permutation) {
  for (size_t u = 0; u < static_cast<size_t>(n); u++) {
    // static casts to avoid warnings from g++
    size_t u_prime = static_cast<size_t>(permutation[u]);
    for (size_t v = u + 1; v < static_cast<size_t>(n); v++) {
      size_t v_prime = static_cast<size_t>(permutation[v]);
      long edge1 = edges[u][v];
      long edge2 = edges[u_prime][v_prime];
      bool edge1_dir = get_edge(edge1, graph1);
      bool edge2_dir = get_edge(edge2, graph2);
      if (edge1_dir) {
        if (!edge2_dir) {
          return false;
        }
      } else if (edge2_dir) {
        return false;
      }
    }
  }
  return true;
}

int is_isometric(const vector<vector<long>> &edges, int n, long graph1,
                 long graph2) {
  vector<int> permutation;
  for (int i = 0; i < n; i++) {
    permutation.push_back(i);
  }
  int permutations = 1;

  while (true) {
    if (is_identical(edges, n, graph1, graph2, permutation)) {
      return permutations;
    }
    permutations++;
    if (!next_permutation(permutation.begin(), permutation.end())) {
      return 0;
    }
  }
}

void make_edges(vector<vector<long>> &edges, int n) {
  long i = 1;
  for (size_t u = 0; u < static_cast<size_t>(n); u++) {
    edges.push_back({});
    for (size_t v = 0; v < static_cast<size_t>(n); v++) {
      if (u == v) {
        // Since we use &, this is correct, graph & edges[u][u] will always be 0
        edges[u].push_back(0);
      } else if (v < u) {
        edges[u].push_back(-edges[v][u]);
      } else {
        edges[u].push_back(i);
        i <<= 1;
      }
    }
  }
}

void populate_degrees_3(array<uint16_t, 9> &in_degrees_1,
                        array<uint16_t, 9> &out_degrees_1,
                        array<array<uint16_t, 9>, 9> &in_degrees_2,
                        array<array<uint16_t, 9>, 9> &out_degrees_2,
                        array<array<array<uint16_t, 9>, 9>, 9> &in_degrees_3,
                        array<array<array<uint16_t, 9>, 9>, 9> &out_degrees_3,
                        vector<vector<long>> &edges, int n, long graph) {
  for (size_t i = 0; i < 9; i++) {
    in_degrees_1[i] = 0;
    out_degrees_1[i] = 0;
    for (size_t j = 0; j < 9; j++) {
      in_degrees_2[i][j] = 0;
      out_degrees_2[i][j] = 0;
      for (size_t k = 0; k < 9; k++) {
        in_degrees_3[i][j][k] = 0;
        out_degrees_3[i][j][k] = 0;
      }
    }
  }

  array<size_t, 9> in_degrees{0, 0, 0, 0, 0, 0, 0, 0, 0};
  array<size_t, 9> out_degrees{0, 0, 0, 0, 0, 0, 0, 0, 0};

  for (size_t u = 0; u < static_cast<size_t>(n); u++) {
    size_t in_degree = 0;
    size_t out_degree = 0;
    for (size_t v = 0; v < static_cast<size_t>(n); v++) {
      long edge = edges[u][v];
      bool dir = get_edge(edge, graph);
      if (dir) {
        out_degree++;
      } else {
        in_degree++;
      }
    }
    in_degrees[u] = in_degree;
    out_degrees[u] = out_degree;
  }

  for (size_t u = 0; u < static_cast<size_t>(n); u++) {
    out_degrees_1[out_degrees[u]]++;
    in_degrees_1[in_degrees[u]]++;
    for (size_t v = 0; v < static_cast<size_t>(n); v++) {
      long edge_uv = edges[u][v];
      bool dir_uv = get_edge(edge_uv, graph);
      if (dir_uv) {
        out_degrees_2[out_degrees[u]][out_degrees[v]]++;
      } else {
        in_degrees_2[in_degrees[u]][in_degrees[v]]++;
      }

      for (size_t w = 0; w < static_cast<size_t>(n); w++) {
        long edge_vw = edges[v][w];
        bool dir_vw = get_edge(edge_vw, graph);
        if (dir_uv && dir_vw) {
          out_degrees_3[out_degrees[u]][out_degrees[v]][out_degrees[w]]++;
        } else if (!dir_uv && !dir_vw) {
          in_degrees_3[in_degrees[u]][in_degrees[v]][in_degrees[w]]++;
        }
      }
    }
  }
}

void iterate_possibilites(std::ostream &out, std::ostream &out_dups,
                          std::istream &in1, std::istream &in2) {
  // Goes from [u, v] to lookup in long
  vector<vector<long>> edges;
  make_edges(edges, 8);

  vector<long> representatives;
  // We use uint16_ts because for degree 3, we might need to store up to
  // 8^3 = 512 > 256
  vector<array<uint16_t, 9>> rep_in_degrees;
  vector<array<uint16_t, 9>> rep_out_degrees;
  // [i][j] = number of vertices of degree i that have forward neighbors of
  // degree j
  vector<array<array<uint16_t, 9>, 9>> rep_in_degrees_2;
  vector<array<array<uint16_t, 9>, 9>> rep_out_degrees_2;

  // [i][j][k] = number of vertices of degree i that have forward neighbors of
  // degree j that have forward neighbors of degree k
  vector<array<array<array<uint16_t, 9>, 9>, 9>> rep_in_degrees_3;
  vector<array<array<array<uint16_t, 9>, 9>, 9>> rep_out_degrees_3;

  string line;
  while (getline(in1, line)) {
    if (line.find("new") == string::npos) continue;
    long graph = std::stol(line.substr(0, line.find(":")));

    array<uint16_t, 9> in_degrees;
    array<uint16_t, 9> out_degrees;
    array<array<uint16_t, 9>, 9> in_degrees_2;
    array<array<uint16_t, 9>, 9> out_degrees_2;
    array<array<array<uint16_t, 9>, 9>, 9> in_degrees_3;
    array<array<array<uint16_t, 9>, 9>, 9> out_degrees_3;
    populate_degrees_3(in_degrees, out_degrees, in_degrees_2, out_degrees_2,
                       in_degrees_3, out_degrees_3, edges, 8, graph);

    out << graph << ": new representative\n";
    representatives.push_back(graph);
    rep_in_degrees.push_back(in_degrees);
    rep_out_degrees.push_back(out_degrees);
    rep_in_degrees_2.push_back(in_degrees_2);
    rep_out_degrees_2.push_back(out_degrees_2);
    rep_in_degrees_3.push_back(in_degrees_3);
    rep_out_degrees_3.push_back(out_degrees_3);
  }

  while (getline(in2, line)) {
    if (line.find("new") == string::npos) continue;
    long graph = std::stol(line.substr(0, line.find(":")));

    array<uint16_t, 9> in_degrees;
    array<uint16_t, 9> out_degrees;
    array<array<uint16_t, 9>, 9> in_degrees_2;
    array<array<uint16_t, 9>, 9> out_degrees_2;
    array<array<array<uint16_t, 9>, 9>, 9> in_degrees_3;
    array<array<array<uint16_t, 9>, 9>, 9> out_degrees_3;
    populate_degrees_3(in_degrees, out_degrees, in_degrees_2, out_degrees_2,
                       in_degrees_3, out_degrees_3, edges, 8, graph);

    bool is_new = true;
    for (size_t rep_ind = 0; rep_ind < representatives.size(); rep_ind++) {
      // Quick check to eliminate looking for impossible isomorphic relabeling
      if (rep_in_degrees[rep_ind] != in_degrees ||
          rep_out_degrees[rep_ind] != out_degrees) {
        continue;
      }
      if (rep_in_degrees_2[rep_ind] != in_degrees_2 ||
          rep_out_degrees_2[rep_ind] != out_degrees_2) {
        continue;
      }
      if (rep_in_degrees_3[rep_ind] != in_degrees_3 ||
          rep_out_degrees_3[rep_ind] != out_degrees_3) {
        continue;
      }

      int isometric_permutation = is_isometric(edges, 8, representatives[rep_ind], graph);
      if (isometric_permutation) {
        out_dups << graph << ": " << representatives[rep_ind] << " ("
                 << isometric_permutation << ")\n";
        is_new = false;
        break;
      }
    }
    if (is_new) {
      out << graph << ": new representative\n";
      representatives.push_back(graph);
      rep_in_degrees.push_back(in_degrees);
      rep_out_degrees.push_back(out_degrees);
      rep_in_degrees_2.push_back(in_degrees_2);
      rep_out_degrees_2.push_back(out_degrees_2);
      rep_in_degrees_3.push_back(in_degrees_3);
      rep_out_degrees_3.push_back(out_degrees_3);
    }
  }
}

int main(int argc, char *argv[]) {
  std::ios::sync_with_stdio(false);

  if (argc != 4) {
    cout << "Usage: " << argv[0] << " <out name> <in name 1> <in name 2>"
         << std::endl;
    return 1;
  }

  string out = argv[1];
  string in1 = argv[2];
  string in2 = argv[3];

  std::ofstream output(out);
  std::ofstream output_dups(out + ".dups");
  std::ifstream input1(in1);
  std::ifstream input2(in2);

  iterate_possibilites(output, output_dups, input1, input2);
  return 0;
}
