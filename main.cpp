#include <tdzdd/DdEval.hpp>
#include <tdzdd/DdSpec.hpp>
#include <tdzdd/DdStructure.hpp>
#include <tdzdd/spec/PathZdd.hpp>
#include <tdzdd/util/Graph.hpp>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

// Graph class
class Graph {
private:
  int const s;
  int const t;
  int const n;
  int const m;
  std::vector<std::pair<int, int> > const edge_list;

public:
  Graph(int s, int t, int n, int m, std::vector<std::pair<int, int> > edge_list)
    :s(s), t(t), n(n), m(m), edge_list(edge_list){
  }

  std::pair<int, int> getEdge (int e) const{
    return std::make_pair(edge_list[e].first, edge_list[e].second);
  }

  int getStart() const {
    return s;
  }

  int getTerminal() const {
    return t;
  }

  int numVertices() const {
    return n;
  }

  int numEdges() const {
    return m;
  }

  void printEdges() const {
    for(auto it = edge_list.begin() ; it != edge_list.end() ; ++it){
      std::cout << (*it).first << ", " << (*it).second << std::endl;
    }
  }

  void print() const{
    std::cout << "#Vertices: " << numVertices() << std::endl;
    std::cout << "#Edges: " << numEdges() << std::endl;
    std::cout << "Start, Terminal = " << s << ", " << t << std::endl;
  }
};

// TdZdd
class PathZDD
  : public tdzdd::SimpathBasedImpl<PathZDD, tdzdd::Cycle, true> {
public:
  explicit PathZDD(tdzdd::Graph const& graph, bool lookahead = true)
    : tdzdd::SimpathBasedImpl<PathZDD, tdzdd::Cycle, true>(graph, lookahead) {
  }
};

// Read input file
Graph readGraph(const char* path) {
  std::ifstream ifs(path);
  if (!ifs) {
    std::cerr << "Error: cannot open file " << path << "\n";
    std::exit(1);
  }

  int s = 0;
  int t = 0;
  if (!(ifs >> s >> t)) {
    std::cerr << "Error: invalid graph header\n";
    std::exit(1);
  }

  std::vector<std::pair<int, int> > edge_list;
  int n = 0;
  int u = 0;
  int v = 0;
  while (ifs >> u >> v) {
    edge_list.push_back(std::make_pair(u, v));
    if (n < u) n = u;
    if (n < v) n = v;
  }
  Graph G(s, t, n, static_cast<int>(edge_list.size()), edge_list);
  return G;
}

std::string writeEdgeTemp(Graph const& g) {
  std::string path = "edges.tmp";
  std::ofstream ofs(path, std::ios::out | std::ios::trunc);
  if (!ofs) {
    std::cerr << "Error: cannot create temp file\n";
    std::exit(1);
  }

  for (int i = 0; i < g.numEdges(); ++i) {
    std::pair<int, int> e = g.getEdge(i);
    ofs << e.first << " " << e.second << "\n";
  }

  ofs.close();
  if (!ofs) {
    std::cerr << "Error: failed to write temp file\n";
    std::exit(1);
  }
  return path;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <graph_file> [--all]\n";
    return 1;
  }

  bool printAll = false;
  if (argc >= 3 && std::string(argv[2]) == "--all") {
    printAll = true;
  }

  Graph g = readGraph(argv[1]);
  g.print();
  std::string tempPath = writeEdgeTemp(g);

  tdzdd::Graph graph;
  graph.readEdges(tempPath);
  std::remove(tempPath.c_str());

  tdzdd::HamiltonCycleZdd spec(graph, true);
  tdzdd::DdStructure<2> dd(spec);
  dd.zddReduce();

  {
    std::ofstream ofs("zdd.dot", std::ios::out | std::ios::trunc);
    if (!ofs) {
      std::cerr << "Error: cannot open zdd.dot for writing\n";
      return 1;
    }
    dd.dumpDot(ofs, "ZDD");
  }

  if (printAll) {
    if (g.numVertices() > 16) {
      std::cerr << "Error: --all is intended for small graphs (e.g. 4x4).\n";
      return 1;
    }

    size_t idx = 0;
    int m = g.numEdges();

    for (tdzdd::DdStructure<2>::const_iterator it = dd.begin();
         it != dd.end(); ++it) {
      ++idx;
      std::cout << "cycle " << idx << ":";
      std::set<int> const& levels = *it;
      std::vector<std::vector<int> > adj(static_cast<size_t>(g.numVertices() + 1));
      
      for (int level : levels) {
        int edgeIdx = m - level;
        if (edgeIdx < 0 || edgeIdx >= m) continue;
        std::pair<int, int> const& e = g.getEdge(edgeIdx);
        adj[e.first].push_back(e.second);
        adj[e.second].push_back(e.first);
      }

      int start = 1;
      while (start <= g.numVertices() && adj[start].empty()) ++start;
      
      int prev = 0;
      int cur = start;
      std::vector<int> cycle;
      cycle.reserve(static_cast<size_t>(g.numVertices() + 1));
      
      do {
        cycle.push_back(cur);
        int next = 0;
        if (adj[cur].size() == 2) {
          next = (adj[cur][0] == prev) ? adj[cur][1] : adj[cur][0];
        } else if (adj[cur].size() == 1) {
          next = adj[cur][0];
        } else {
          break;
        }
        prev = cur;
        cur = next;
      } while (cur != start && cycle.size() <= static_cast<size_t>(g.numVertices() + 1));
      
      if (cur == start) {
        for (size_t i = 0; i < cycle.size(); ++i) {
          std::cout << (i == 0 ? " " : "->") << cycle[i];
        }
        std::cout << "->" << start << "\n";
      } else {
        std::cout << " invalid\n";
      }
    }
  
  } else {
    auto count = dd.evaluate(tdzdd::ZddCardinality<>());
    std::cout << count << "\n";
  }
  return 0;
}
