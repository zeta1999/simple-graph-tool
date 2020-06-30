#include "headers/Graph.h"
#include <iomanip>
#include <cmath>
#include <algorithm>
#include "headers/random.h"

using namespace GraphType;

Graph::Graph(int nodeNum) {
    init(nodeNum);
}

void Graph::init(int node_num) {
    this->clear();
    if (node_num <= 0) return;
    Random random;
    for (int i = 0; i < node_num; i++) {
        QPointF point((1 - sin((i * 6.28) / node_num)) * node_num * 80 / 2.,
            (1 - cos((i * 6.28) / node_num)) * node_num * 80 / 2.);
        addNode(Node(nextNodeName(), point));
    }
}

Graph::Graph(const Graph &obj) {
    this->clear();
    for (auto &node: obj.nodeList())
        this->addNode(node->name());
    for (auto it = obj.arcSet().begin(); it != obj.arcSet().end(); ++it) {
        this->setArc(Arc(it).u()->name(), Arc(it).v()->name(), Arc(it).weight());
    }
}

void Graph::clear() {
    this->_nodeSet.clear();
    this->_arcSet.clear();
    this->_cachedNodeList.clear();
}

void Graph::readFromFile(const std::string &file) {
    clear();
    std::ifstream in(file);
    if (!in) throw "file not existed";

    int countNodes;
    in >> countNodes;
    if (countNodes <= 0) throw "number of nodes <= 0";
    std::string name;
    qreal x, y;
    for (int i = 0; i < countNodes; i++) {
        if (in.eof()) throw "eof";
        in >> name >> x >> y, addNode(Node(name, QPointF(x, y)));
    }
    int countArcs;
    in >> countArcs;
    if (countArcs > 0) {
        std::string start, end;
        int weight;
        for (int i = 0; i < countArcs; i++) {
            if (in.eof()) throw "eof";
            in >> start >> end >> weight;
            setArc(start, end, weight);
        }
    }
    in.close();
}

void Graph::writeToFile(const std::string &file) const {
    std::ofstream out;
    out.open(file, std::ofstream::out | std::ofstream::trunc);
    out << countNodes() << "\n";
    for (const auto &node: nodeList())
        out << node->name() << " " << node->euclidePos().x() << " " << node->euclidePos().y() << "\n";
    out << countArcs() << "\n";
    for (auto it = arcSet().begin(); it != arcSet().end(); ++it) {
        out << Arc(it).u()->name() << " " << Arc(it).v()->name() << " " << Arc(it).weight() << "\n";
    }
    out.close();
}

Node *Graph::node(const std::string &node_name) const {
    if (!hasNode(node_name))
        return nullptr;
    return const_cast<Node *>(&(*_nodeSet.find(Node(node_name))));
}

bool Graph::addNode(const Node &_node) {
    if (hasNode(_node.name())) return false;
    _nodeSet.insert(_node);
    _cachedNodeList.emplace_back(node(_node.name()));
    return true;
}

std::string Graph::nextNodeName() const {
    for (int i = 0; i < countNodes(); i++) {
        auto name = std::string(1, 'a' + (i % 26)) + std::to_string(int(i / 26));
        if (!hasNode(name))
            return name;
    }
    return std::string(1, 'a' + (countNodes() % 26)) + std::to_string(int(countNodes() / 26));
}

bool Graph::addNode(std::string node_name) {
    if (node_name.empty())
        node_name = nextNodeName();
    return addNode(Node(node_name));
}

bool Graph::removeNode(Node *node) {
    if (!hasNode(node))
        return false;
    isolateNode(node);
    _nodeSet.erase(*node);
    _cachedNodeList.remove(node);
    return true;
}

bool Graph::removeNode(const std::string &name) {
    return removeNode(node(name));
}

bool Graph::isolateNode(Node *node) {
    if (!hasNode(node))
        return false;
    std::list<Arc> cachedArcs;
    for (auto it = arcSet().begin(); it != arcSet().end(); ++it) {
        if (Arc(it).u() == node || Arc(it).v() == node)
            cachedArcs.emplace_back(Arc(it));
    }
    for (auto &arc: cachedArcs)
        removeArc(arc.u(), arc.v());
    return true;
}

bool Graph::isolateNode(const std::string &name) {
    return isolateNode(node(name));
}

bool Graph::setNodeName(Node *node, const std::string &new_name) {
    if (!hasNode(node) || hasNode(new_name) || node->name() == new_name)
        return false;
    std::list<Arc> cachedArcs;
    for (auto it = arcSet().begin(); it != arcSet().end(); ++it) {
        auto arc = Arc(it);
        if (arc.u() == node || arc.v() == node)
            cachedArcs.emplace_back(arc);
    }
    addNode(Node(new_name, node->euclidePos()));
    for (auto it = arcSet().begin(); it != arcSet().end(); ++it) {
        auto arc = Arc(it);
        if (arc.u()->name() == node->name())
            setArc(new_name, arc.v()->name(), arc.weight());
        else if (arc.v()->name() == node->name())
            setArc(arc.u()->name(), new_name, arc.weight());
    }
    removeNode(node);
    return true;
}

bool Graph::setNodeName(const std::string &old_name, const std::string &new_name) {
    return setNodeName(node(old_name), new_name);
}

int Graph::weight(Node *u, Node *v) const {
    if (!hasArc(u, v))
        return INT_MAX;
    return _arcSet.at({u, v});
}

int Graph::weight(const std::string &uname, const std::string &vname) const {
    return weight(node(uname), node(vname));
}

bool Graph::setArc(Node *const u, Node *const v, int w) {
    if (u == v || !hasNode(u) || !hasNode(v) || w <= 0 || w >= INT_MAX)
        return false;
    if (hasArc(u, v))
        _arcSet.at(std::make_pair(u, v)) = w;
    else {
        _arcSet.insert({std::make_pair(u, v), w});
        u->incPositiveDeg();
        v->incNegativeDeg();
    }
    return true;
}

bool Graph::setArc(const std::string &uname, const std::string &vname, int w) {
    return setArc(node(uname), node(vname), w);
}

bool Graph::removeArc(Node *const u, Node *const v) {
    if (u == v || !hasNode(u) || !hasNode(v))
        return false;
    if (hasArc(u, v)) {
        _arcSet.erase(std::make_pair(u, v));
        u->decPositiveDeg();
        v->decNegativeDeg();
        return true;
    }
    return false;
}

bool Graph::removeArc(const std::string &uname, const std::string &vname) {
    return removeArc(node(uname), node(vname));
}

Graph Graph::transpose() const {
    Graph transposed_graph;
    for (const auto &node: this->nodeList())
        transposed_graph.addNode(node->name());
    for (auto it = arcSet().begin(); it != arcSet().end(); ++it)
        transposed_graph.setArc(Arc(it).v()->name(), Arc(it).u()->name(), Arc(it).weight());
    return transposed_graph;
}

std::ostream &operator<<(std::ostream &os, const Graph &graph) {
    if (graph.nodeList().empty()) {
        if (!graph.nodeList().empty())
            throw "Error in clearing graph: nodeSet is empty but arcSet";
        else {
            os << "";
            return os;
        }
    }
    os << "\n";
    auto adj = graph.adjMatrix();
    auto nodeNum = adj.nodes().size();
    auto maxLength = 3;

    for (auto &node: adj.nodes())
        maxLength = std::max(maxLength, (int) node->name().length());
    for (auto it = graph.arcSet().begin(); it != graph.arcSet().end(); ++it)
        maxLength = std::max(maxLength, (int) std::to_string(Arc(it).weight()).length());

    os << std::string(maxLength + 1, ' ');
    for (auto &node: adj.nodes())
        os << std::left << std::setw(maxLength) << node->name() << " ";
    os << "\n";
    for (int i = 0; i < nodeNum; i++) {
        os << std::left << std::setw(maxLength) << adj.node(i)->name() << " ";
        for (int j = 0; j < nodeNum; j++) {
            if (adj.weight(i, j) != INT_MAX)
                os << std::left << std::setw(maxLength) << adj.weight(i, j) << " ";
            else
                os << std::left << std::setw(maxLength) << "INF" << " ";
        }
        os << "\n";
    }
    return os;
}

bool Graph::hasNode(Node *node) const {
    if (!node) return false;
    return _nodeSet.find(*node) != _nodeSet.end();
}

