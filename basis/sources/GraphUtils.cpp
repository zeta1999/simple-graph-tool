#include "basis/headers/GraphUtils.h"
#include <unordered_map>
#include <utility>
#include <QString>
#include <QDebug>

std::list<std::pair<std::string, std::string>> GraphUtils::BFSToDemo(const Graph *graph, const std::string &source) {
    std::list<std::pair<std::string, std::string>> result;
    if (!graph->hasNode(source)) return result;

    auto nodes = graph->nodeList();
    std::queue<std::string> q;
    std::unordered_map<std::string, bool> visited;
    std::unordered_map<std::string, std::string> parent;
    q.push(source);
    visited[source] = true;
    std::cout << "BFS (source = " << source << "): ";

    while (!q.empty()) {
        auto vname = q.front();
        std::cout << vname << " ";
        result.emplace_back(parent[vname], vname);
        q.pop();

        for (auto &adj : nodes) {
            if (graph->hasEdge(vname, adj->name()) && !visited[adj->name()]) {
                visited[adj->name()] = true;
                q.push(adj->name());
                parent[adj->name()] = vname;
            }
        }
    }
    std::cout << "\n";
    return result;
}

std::list<std::pair<std::string, std::string>> GraphUtils::DFSToDemo(const Graph *graph, const std::string &source) {
    std::list<std::pair<std::string, std::string>> result;
    if (!graph->hasNode(source)) return result;

    auto nodes = graph->nodeList();
    std::stack<std::string> s;
    std::unordered_map<std::string, bool> visited;
    std::unordered_map<std::string, std::string> parent;
    s.push(source);
    visited[source] = true;

    std::cout << "DFS (source = " << source << "): ";
    while (!s.empty()) {
        auto vname = s.top();
        std::cout << vname << " ";
        visited[vname] = true;
        result.emplace_back(parent[vname], vname);
        s.pop();

        for (auto &adj: nodes) {
            if (graph->hasEdge(vname, adj->name()) && !visited[adj->name()]) {
                s.push(adj->name());
                parent[adj->name()] = vname;
            }
        }
    }
    std::cout << "\n";
    return result;
}

std::list<std::string> GraphUtils::BFS(const Graph *graph, std::string source) {
    auto nodes = graph->nodeList();
    if (source.empty())
        source = nodes.front()->name();
    std::list<std::string> steps;
    if (!graph->hasNode(source)) return steps;
    std::queue<std::string> q;
    std::unordered_map<std::string, bool> visited;
    q.push(source);
    visited[source] = true;
    while (!q.empty()) {
        auto vname = q.front();
        steps.push_back(vname);
        q.pop();
        for (auto &adj: nodes) {
            if (graph->hasEdge(vname, adj->name()) && !visited[adj->name()]) {
                visited[adj->name()] = true;
                q.push(adj->name());
            }
        }
    }
    return steps;
}

void GraphUtils::DFSUtil(const Graph *graph, const std::string &vname, std::unordered_map<std::string, bool> &visited,
                         std::list<std::string> &steps) {
    visited[vname] = true;
    steps.push_back(vname);
    for (auto &adj: graph->nodeList()) {
        if (graph->hasEdge(vname, adj->name()) && !visited[adj->name()])
            DFSUtil(graph, adj->name(), visited, steps);
    }
}

void UndirectedDFSUtil(const Graph *graph, const std::string &vname, std::unordered_map<std::string, bool> &visited,
                       std::list<std::string> &steps) {
    visited[vname] = true;
    steps.push_back(vname);
    for (auto &adj: graph->nodeList()) {
        if ((graph->hasEdge(vname, adj->name()) || graph->hasEdge(adj->name(), vname)) && !visited[adj->name()])
            UndirectedDFSUtil(graph, adj->name(), visited, steps);
    }
}

int DFSCount(const Graph &graph, const std::string &uname, std::unordered_map<std::string, bool> &visited) {
    visited[uname] = true;
    int count = 1;
    for (auto v: graph.nodeList())
        if (graph.hasEdge(uname, v->name()) && !visited[v->name()])
            count += DFSCount(graph, v->name(), visited);
    return count;
}

std::list<std::string> GraphUtils::DFS(const Graph *graph, std::string source) {
    auto nodes = graph->nodeList();
    if (source.empty())
        source = nodes.front()->name();
    std::list<std::string> steps;
    std::unordered_map<std::string, bool> visited;
    if (!graph->hasNode(source)) return steps;
    DFSUtil(graph, source, visited, steps);
    return steps;
}

bool GraphUtils::isConnectedFromUtoV(const Graph *graph, const std::string &uname, const std::string &vname) {
    if (!graph->hasNode(uname) || !graph->hasNode(vname)) return false;
    std::unordered_map<std::string, bool> visited;
    std::list<std::string> steps;
    DFSUtil(graph, uname, visited, steps);
    return visited[vname];
}

bool GraphUtils::isAllStronglyConnected(const Graph *graph) {
    auto nodes = graph->nodeList();
    for (auto &node: nodes) {
        std::unordered_map<std::string, bool> visited;
        std::list<std::string> steps;
        DFSUtil(graph, node->name(), visited, steps);
        for (auto &v: nodes) {
            if (!visited[v->name()])
                return false;
        }
    }
    return true;
}

bool GraphUtils::isAllWeaklyConnected(const Graph *graph) {
    auto nodes = graph->nodeList();
    std::queue<std::string> q;
    std::unordered_map<std::string, bool> visited;
    q.push(nodes.front()->name());
    visited[nodes.front()->name()] = true;

    while (!q.empty()) {
        auto vname = q.front();
        q.pop();
        for (auto &adj: nodes) {
            if ((graph->hasEdge(vname, adj->name()) || graph->hasEdge(adj->name(), vname)) &&
                !visited[adj->name()]) {
                visited[adj->name()] = true;
                q.push(adj->name());
            }
        }
    }
    for (auto &&i : visited)
        if (!i.second)
            return false;
    return true;
}

std::string minDistance(std::unordered_map<std::string, int> &dist, std::unordered_map<std::string, bool> &sptSet) {
    int min = INT_MAX;
    std::string min_index;
    int n = dist.size();
    for (auto &it: dist)
        if (!sptSet[it.first] && it.second < min)
            min = it.second, min_index = it.first;
    return min_index;
}

std::list<std::string> GraphUtils::Dijkstra(const Graph *graph, const std::string &start, const std::string &goal) {
    if (!graph->hasNode(start) || !graph->hasNode(goal))
        return std::list<std::string>();
    auto nodes = graph->nodeList();
    std::unordered_map<std::string, int> dist;
    for (auto &node: nodes)
        dist[node->name()] = INT_MAX;

    std::unordered_map<std::string, bool> sptSet;
    std::unordered_map<std::string, std::string> parent;
    dist[start] = 0;
    for (int count = 0; count < graph->countNodes() - 1; count++) {
        auto uname = minDistance(dist, sptSet);
        sptSet[uname] = true;
        for (auto &v: nodes) {
            if (!sptSet[v->name()] && graph->hasEdge(uname, v->name()) && dist[uname] != INT_MAX &&
                dist[uname] + graph->weight(uname, v->name()) < dist[v->name()]) {
                dist[v->name()] = dist[uname] + graph->weight(uname, v->name());
                parent[v->name()] = uname;
            }
        }
    }
    std::list<std::string> path;
    std::cout << "Dijkstra: shortest path from " << start << " to " << goal << ": ";
    if (dist[goal] == INT_MAX) {
        std::cout << " not found!\n";
        return path;
    }
    auto curr = goal;
    while (!curr.empty()) {
        path.push_front(curr);
        curr = parent[curr];
    }
    for (auto &nodeName: path)
        std::cout << nodeName << " ";
    std::cout << " ------------ cost = " << dist[goal];
    return path;
}

#include <cmath>

int euclideanDist(QPointF p1, QPointF p2) {
    return (int) std::sqrt(std::pow(p1.x() - p2.x(), 2) +
                           std::pow(p1.y() - p2.y(), 2));
}

std::list<std::string> GraphUtils::AStar(const Graph *graph, std::string start, std::string goal) {
    if (!graph->hasNode(start) || !graph->hasNode(goal))
        return std::list<std::string>();
    auto nodes = graph->nodeList();
    std::unordered_map<std::string, int> dist;
    std::unordered_map<std::string, int> cost;
    for (auto &node: nodes) {
        dist[node->name()] = INT_MAX;
        cost[node->name()] = INT_MAX;
    }

    std::unordered_map<std::string, bool> sptSet;
    std::unordered_map<std::string, std::string> parent;
    dist[start] = 0;
    cost[start] = 0;
    for (int count = 0; count < graph->countNodes() - 1; count++) {
        auto uname = minDistance(dist, sptSet);
        sptSet[uname] = true;
        for (auto &v: nodes) {
            double h = graph->weight(uname, v->name()) +
                       euclideanDist(graph->node(v->name())->euclidePos(), graph->node(goal)->euclidePos());
            if (!sptSet[v->name()] && graph->hasEdge(uname, v->name()) && dist[uname] != INT_MAX &&
                dist[uname] + h < dist[v->name()]) {
                dist[v->name()] = dist[uname] + h;
                cost[v->name()] = cost[uname] + graph->weight(uname, v->name());
                parent[v->name()] = uname;
            }
        }
    }
    std::list<std::string> path;
    std::cout << "A-star: Shortest path from " << start << " to " << goal << ": ";
    if (cost[goal] == INT_MAX) {
        std::cout << " not found!\n";
        return path;
    }
    auto curr = goal;
    while (!curr.empty()) {
        path.push_front(curr);
        curr = parent[curr];
    }
    for (auto &nodeName: path)
        std::cout << nodeName << " ";
    std::cout << " ------------ cost = " << cost[goal];
    return path;
}

std::string minKey(std::unordered_map<std::string, int> &key, std::unordered_map<std::string, bool> &mstSet) {
    int min = INT_MAX;
    std::string min_index;
    for (auto &it: key) {
        if (!mstSet[it.first] && it.second < min) {
            min = it.second;
            min_index = it.first;
        }
    }
    return min_index;
}

std::list<std::pair<std::string, std::string>>
PrimUtil(const Graph *graph, const std::string &source, bool &full, int &cost) {
    std::list<std::pair<std::string, std::string>> result;
    if (!graph->hasNode(source)) return result;
    std::unordered_map<std::string, std::string> parent;
    std::unordered_map<std::string, int> key;
    auto nodes = graph->nodeList();
    for (auto &node: nodes)
        key[node->name()] = INT_MAX;
    std::unordered_map<std::string, bool> mstSet;

    key[source] = 0;
    parent[source] = "";
    cost = 0;
    for (int count = 0; count < graph->countNodes(); count++) {
        auto u = minKey(key, mstSet);
        mstSet[u] = true;
        for (auto &node: nodes) {
            auto v = node->name();
            if (!mstSet[v]
                && graph->hasEdge(u, v)
                && graph->weight(u, v) < key[v]) {
                key[v] = graph->weight(u, v);
                parent[v] = u;
                result.emplace_back(std::make_pair(u, v));
                cost += graph->weight(u, v);
            }
        }
    }
    full = true;
    if (!result.empty()) {
        for (auto node: nodes) {
            if (node->name() != source && parent[node->name()].empty())
                full = false;
        }
    } else
        full = false;
    return result;
}

void
printMST(const Graph &graph, const std::list<std::pair<std::string, std::string>> &result, const std::string &source,
         int cost) {
    if (result.empty()) {
        std::cout << "Not found minimum spanning tree" << (source.empty() ? " for the whole graph" : " from source = ")
                  << source << "\n";
        return;
    }
    std::cout << "Minimum spanning tree (source = " << source << "): " << std::endl;
    std::cout << "vertex \t parent \t cost:" << std::endl;

    for (const auto &it: result) {
        std::cout << it.second << " \t " << it.first << " \t "
                  << graph.weight(it.first, it.second) << std::endl;
    }
    std::cout << "total cost: " << cost << "\n";
}

std::list<std::pair<std::string, std::string>> GraphUtils::Prim(const Graph *graph, std::string source) {
    std::list<std::pair<std::string, std::string>> result;
    const auto nodes = graph->nodeList();
    bool full{};
    int min_cost = INT_MAX;
    if (source.empty()) {
        if (!isAllWeaklyConnected(graph)) {
            std::cout << "Minimum spanning tree: not found because the graph is not connected!";
            return result;
        }
        for (auto node: nodes) {
            int cost;
            auto res = PrimUtil(graph, node->name(), full, cost);
            if (full && cost < min_cost) {
                result = res;
                min_cost = cost;
                source = node->name();
            }
        }
        printMST(*graph, result, source, min_cost);
        return result;
    }
    result = PrimUtil(graph, source, full, min_cost);
    printMST(*graph, result, source, min_cost);
    return result;
}

void weaklyFillOrder(const Graph *graph, const std::string &vname, std::unordered_map<std::string, bool> &visited,
                     std::stack<std::string> &stack) {
    visited[vname] = true;
    auto nodes = graph->nodeList();
    for (auto &node: nodes)
        if (!visited[node->name()] && (graph->hasEdge(vname, node->name()) ||
                                       graph->hasEdge(vname, node->name())))
            weaklyFillOrder(graph, node->name(), visited, stack);
    stack.push(vname);
}

void stronglyFillOrder(const Graph *graph, const std::string &vname, std::unordered_map<std::string, bool> &visited,
                       std::stack<std::string> &stack) {
    visited[vname] = true;
    auto nodes = graph->nodeList();
    for (auto &node: nodes)
        if (!visited[node->name()] && graph->hasEdge(vname, node->name()))
            weaklyFillOrder(graph, node->name(), visited, stack);
    stack.push(vname);
}

std::list<std::list<std::string>> GraphUtils::connectedComponents(const Graph *graph) {
    std::list<std::list<std::string>> result;
    std::unordered_map<std::string, bool> visited;
    std::stack<std::string> stack;

    auto nodes = graph->nodeList();
    for (auto &node: nodes)
        if (!visited[node->name()])
            stronglyFillOrder(graph, node->name(), visited, stack);

    Graph gr = graph->transpose();
    visited.clear();

    while (!stack.empty()) {
        auto vname = stack.top();
        stack.pop();

        if (!visited[vname]) {
            std::list<std::string> steps;
            DFSUtil(&gr, vname, visited, steps);
            result.push_back(steps);
        }
    }
    return result;
}

std::list<std::list<std::string>> GraphUtils::weaklyConnectedComponents(const Graph *graph) {
    std::list<std::list<std::string>> result;
    std::unordered_map<std::string, bool> visited;
    auto nodes = graph->nodeList();
    for (auto &node: nodes) {
        if (!visited[node->name()]) {
            std::list<std::string> component;
            UndirectedDFSUtil(graph, node->name(), visited, component);
            result.push_back(component);
        }
    }
    return result;
}

void bridgeUtil(const Graph *graph, const std::string &uname,
                std::unordered_map<std::string, bool> &visited,
                std::unordered_map<std::string, int> &disc,
                std::unordered_map<std::string, int> &low,
                std::unordered_map<std::string, std::string> &parent,
                std::list<std::pair<std::string, std::string>> &res, int &time) {

    visited[uname] = true;
    disc[uname] = low[uname] = ++time;
    auto nodes = graph->nodeList();
    for (auto &v: nodes) {
        if (!visited[v->name()] && graph->hasEdge(uname, v->name())) {
            parent[v->name()] = uname;
            bridgeUtil(graph, v->name(), visited, disc, low, parent, res, time);
            low[uname] = std::min(low[uname], low[v->name()]);
            if (low[v->name()] > disc[uname]) {
                res.emplace_back(uname, v->name());
            }
        } else if (v->name() != parent[uname] && graph->hasEdge(uname, v->name()))
            low[uname] = std::min(low[uname], disc[v->name()]);
    }
}

std::list<std::pair<std::string, std::string>> GraphUtils::getBridges(const Graph *graph) {
    std::unordered_map<std::string, bool> visited;
    std::unordered_map<std::string, int> disc;
    std::unordered_map<std::string, int> low;
    std::unordered_map<std::string, std::string> parent;
    std::list<std::pair<std::string, std::string>> result;

    int time = 0;
    auto nodes = graph->nodeList();
    for (auto &v: nodes) {
        if (!visited[v->name()])
            bridgeUtil(graph, v->name(), visited, disc, low, parent, result, time);
    }
    return result;
}

void APUtil(const Graph *graph, const std::string &uname,
            std::unordered_map<std::string, bool> &visited,
            std::unordered_map<std::string, int> &disc,
            std::unordered_map<std::string, int> &low,
            std::unordered_map<std::string, std::string> &parent,
            std::list<std::string> &ap) {

    static int time = 0;
    int children = 0;
    visited[uname] = true;

    disc[uname] = low[uname] = ++time;

    for (auto &v: graph->nodeList()) {
        if (graph->hasEdge(uname, v->name())) {
            if (!visited[v->name()]) {
                children++;
                parent[v->name()] = uname;
                APUtil(graph, v->name(), visited, disc, low, parent, ap);

                low[uname] = std::min(low[uname], low[v->name()]);
                if (parent[uname].empty() && children > 1)
                    ap.push_front(uname);

                if (!parent[uname].empty() && low[v->name()] >= disc[uname])
                    ap.push_front(uname);
            } else if (v->name() != parent[uname])
                low[uname] = std::min(low[uname], disc[v->name()]);
        }
    }
}

std::list<std::string> GraphUtils::getArticulationNodes(const Graph *graph) {
    std::unordered_map<std::string, bool> visited;
    std::unordered_map<std::string, int> disc;
    std::unordered_map<std::string, int> low;
    std::unordered_map<std::string, std::string> parent;
    std::list<std::string> ap;

    for (auto &node: graph->nodeList())
        if (!visited[node->name()])
            APUtil(graph, node->name(), visited, disc, low, parent, ap);
    return ap;
}

std::list<std::string> GraphUtils::displayArticulationNodes(const Graph *graph) {

    std::list<std::string> nodes = getArticulationNodes(graph);
    std::cout << "All articulation nodes: ";
    if (nodes.empty()) {
        std::cout << "not found!\n";
        return nodes;
    }
    for (auto &node: nodes)
        std::cout << node << " ";
    std::cout << "\n";
    return nodes;
}

std::list<std::pair<std::string, std::string>> GraphUtils::displayBridges(const Graph *graph) {
    std::list<std::pair<std::string, std::string>> bridges = getBridges(graph);
    std::cout << "Number of bridges: " << bridges.size() << "\n";
    for (const auto &b: bridges)
        std::cout << b.first << " " << b.second << "\n";
    return bridges;
}

std::list<std::list<std::string>> GraphUtils::displayConnectedComponents(const Graph *graph, bool strong) {
    auto nodes = graph->nodeList();
    std::list<std::list<std::string>> result;
    if (strong)
        result = connectedComponents(graph);
    else
        result = weaklyConnectedComponents(graph);
    std::cout << "Number of " << (strong ? (graph->isDirected() ? "strongly " : "") : "weakly ")
              << "connected components: " << result.size() << "\n";
    for (const auto &ri: result) {
        for (const auto &si: ri)
            std::cout << si << " ";
        std::cout << "\n";
    }
    return result;
}

bool isSafe(const Graph *graph, const std::string &vname,
            std::vector<std::string> &path, int pos) {
    if (!graph->hasEdge(path[pos - 1], vname))
        return false;
    for (int i = 0; i < pos; i++)
        if (path[i] == vname)
            return false;
    return true;
}

bool hamCycleUtil(const Graph *graph, std::vector<std::string> &path, int pos) {
    int n = graph->countNodes();
    if (pos == n)
        return graph->hasEdge(path[pos - 1], path[0]);

    for (auto &v: graph->nodeList()) {
        if (v->name() != path[0] && isSafe(graph, v->name(), path, pos)) {
            path[pos] = v->name();
            if (hamCycleUtil(graph, path, pos + 1))
                return true;
            path[pos] = "";
        }
    }
    return false;
}

std::list<std::string> GraphUtils::getHamiltonianCycle(const Graph *graph, std::string source) {
    auto nodes = graph->nodeList();
    if (source.empty())
        source = nodes.front()->name();
    std::vector<std::string> path(graph->countNodes(), "");
    std::list<std::string> result;
    path[0] = source;
    if (!hamCycleUtil(graph, path, 1))
        return result;
    for (int i = 0; i < graph->countNodes(); i++)
        result.push_back(path[i]);
    result.push_back(path[0]);
    return result;
}

std::list<std::list<std::string>> GraphUtils::displayHamiltonianCycle(const Graph *graph) {
    std::list<std::list<std::string>> result;
    if (graph->countNodes() < 3) {
        std::cout << "Hamiltonian Cycle not found: ";
        std::cout << "|V| = " << graph->countNodes() << " < 3\n";
        return result;
    }
    auto nodes = graph->nodeList();
    for (auto &source: nodes) {
        auto cycle = getHamiltonianCycle(graph, source->name());
        if (cycle.empty())
            continue;
        else
            result.push_back(cycle);

        std::cout << "Hamiltonian Cycle (source = " << source->name() << "): ";
        for (auto &node: cycle)
            std::cout << node << " ";
        std::cout << "\n";
    }
    if (result.empty())
        std::cout << "Not found any Hamiltonian cycle";
    return result;
}

bool isBridge(Graph graph, const std::string &uname, const std::string &vname) {
    if (uname == vname)
        return false;
    std::unordered_map<std::string, bool> visited;
    int count1 = DFSCount(graph, uname, visited);
    graph.removeEdge(uname, vname);
    int count2 = DFSCount(graph, vname, visited);
    return count1 <= count2;
}

std::list<std::string> GraphUtils::Hierholzer(Graph &graph, std::string source) {
    std::list<std::string> result;
    std::stack<std::string> curr_path;
    std::vector<std::string> cycle;
    auto curr_node = source;
    curr_path.push(source);
    while (!curr_path.empty()) {
        int degree = graph.isDirected() ? graph.node(curr_node)->negDegree() : graph.node(curr_node)->undirDegree();
        if (degree) {
            curr_path.push(curr_node);
            std::string next_node;
            for (auto v: graph.nodeList()) {
                if (graph.hasEdge(curr_node, v->name())) {
                    next_node = v->name();
                    break;
                }
            }
            graph.removeEdge(curr_node, next_node);
            curr_node = next_node;
        }
        else {
            cycle.push_back(curr_node);
            curr_node = curr_path.top();
            curr_path.pop();
        }
    }
    for (int i = cycle.size() - 1; i >= 0; --i)
        result.push_back(cycle[i]);
    return result;
}

std::list<std::string> GraphUtils::getEulerianCircuit(const Graph *_graph, std::string source) {
    std::list<std::string> result;
    Graph graph(*_graph);
    if (source.empty())
        source = graph.nodeList().front()->name();
    if (!graph.hasNode(source))
        return result;

    result = Hierholzer(graph, source);
    return result;
}

std::list<std::list<std::string>>
GraphUtils::displayEulerianCircuit(const Graph *graph) {
    std::list<std::list<std::string>> result;
    if (graph->isDirected()) {
        if (!isAllStronglyConnected(graph)) {
            std::cout << "Eulerian Circuit not found because the graph is not strongly connected\n";
            return result;
        }
        for (auto &node: graph->nodeList()) {
            if (node->negDegree() != node->posDegree()) {
                std::cout << "Eulerian Circuit not found because Node " << node->name() << " has deg+ != deg-\n";
                return result;
            }
        }
    }

    for (auto &node: graph->nodeList()) {
        auto cycle = getEulerianCircuit(graph, node->name());
        std::cout << "Euler Circuit (source = " << node->name() << "): ";
        if (cycle.empty())
            std::cout << "not found!\n";
        else {
            result.emplace_back(cycle);
            while (!cycle.empty()) {
                std::cout << cycle.front() << " ";
                cycle.pop_front();
            }
            std::cout << "\n";
        }
    }
    return result;
}

void topoSortUtil(const Graph *graph, const std::string &vname, std::unordered_map<std::string, bool> &visited,
                  std::stack<std::string> &stack) {
    visited[vname] = true;
    for (auto &node: graph->nodeList())
        if (graph->hasEdge(vname, node->name()) && !visited[node->name()])
            topoSortUtil(graph, node->name(), visited, stack);
    stack.push(vname);
}

bool GraphUtils::isCycle(const Graph *graph) {
    std::queue<std::string> q;
    std::unordered_map<std::string, int> in_degree;
    for (auto node: graph->nodeList()) {
        in_degree[node->name()] = node->posDegree();
        if (in_degree[node->name()] == 0)
            q.push(node->name());
    }
    int cnt = 0;
    std::vector<std::string> top_order;
    while (!q.empty()) {
        auto u = q.front();
        q.pop();
        top_order.push_back(u);
        for (auto v: graph->nodeList()) {
            if (--in_degree[v->name()] == 0)
                q.push(v->name());
            cnt++;
        }
    }
    return (cnt != graph->countNodes());
}

std::list<std::string> GraphUtils::getTopoSortResult(const Graph *graph) {
    if (!(graph->isDirected() && isCycle(graph))) {
        std::cout << "The graph is not DAG\n";
        return std::list<std::string>();
    }
    std::unordered_map<std::string, bool> visited;
    std::stack<std::string> stack;
    for (auto &node: graph->nodeList())
        if (!visited[node->name()])
            topoSortUtil(graph, node->name(), visited, stack);

    std::list<std::string> result;
    while (!stack.empty()) {
        result.push_back(stack.top());
        stack.pop();
    }
    return result;
}

std::list<std::string> GraphUtils::displayTopoSort(const Graph *graph) {
    std::list<std::string> topo_sorted;
    if (graph->isDirected()) {
        topo_sorted = getTopoSortResult(graph);
        std::cout << "Topo sorted: ";
        if (topo_sorted.empty()) {
            std::cout << "not found!\n";
            return topo_sorted;
        }
        for (const auto &v: topo_sorted)
            std::cout << v << " ";
        std::cout << "\n";
    }
    return topo_sorted;
}

std::list<std::pair<std::string, int>> GraphUtils::getColoringResult(const Graph *graph, std::string source) {
    std::list<std::pair<std::string, int>> res_list;
    if (graph->isDirected())
        return res_list;

    auto nodes = graph->nodeList();
    if (source.empty())
        source = nodes.front()->name();
    std::unordered_map<int, bool> available; // available colors
    std::unordered_map<std::string, int> result; // result: map of pairs { nodeName : _color }

    for (auto &node: nodes)
        result[node->name()] = -1;
    result[source] = 0;
    for (auto u: nodes) {
        if (u->name() != source) {
            for (auto v: nodes) {
                if (graph->hasEdge(u->name(), v->name())) {
                    if (result[v->name()] != -1)
                        available[result[v->name()]] = true;
                }
            }
            int cr;
            for (cr = 0; cr < graph->countNodes(); cr++) {
                if (!available[cr])
                    break;
            }
            result[u->name()] = cr;
            for (auto &v: nodes) {
                if (graph->hasEdge(v->name(), v->name())) {
                    if (result[v->name()] != -1)
                        available[result[v->name()]] = false;
                }
            }
        }
    }
    for (auto &node: nodes) // using for loop because it requires ordering from node 0th to node 0th and result map is unordered
        res_list.emplace_back(std::make_pair(
                node->name(),
                result[node->name()]));
    return res_list;
}

std::list<std::pair<std::string, std::string>> GraphUtils::displayColoring(const Graph *graph, std::string source) {
    if (graph->isDirected())
        return std::list<std::pair<std::string, std::string>>();
    auto result = getColoringResult(graph, std::move(source));
    std::list<std::pair<std::string, std::string>> resultToSent;
    std::cout << "Coloring of the graph: " << std::endl;
    for (auto &it: result) {
        std::cout << "Node " << it.first << " ---> Color " << it.second << std::endl;
        resultToSent.emplace_back(it.first, std::to_string(it.second));
    }
    return resultToSent;
}

std::list<std::pair<std::string, std::string>> GraphUtils::usualST(const Graph *graph, const std::string &source) {
    std::list<std::pair<std::string, std::string>> result;
    std::unordered_map<std::string, bool> visited;
    std::list<std::string> steps;
    DFSUtil(graph, source, visited, steps);
    return result;
}
