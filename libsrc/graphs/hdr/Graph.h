#ifndef __GRAPH_H
#define __GRAPH_H

#include <assert.h>

#include <string>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>

#include "system/Debug.h"
#include "system/Returns.h"
#include "parsing/Tokenizer.h"

namespace ombt {

class Graph {
public:
    // data types
    typedef long VertexId;
    typedef long Weight;

    struct Vertex {
        Vertex(): 
            id_(-1), label_("") { }
        Vertex(VertexId id, const std::string &label): 
            id_(id), label_(label) { }
        Vertex(const Vertex &v): 
            id_(v.id_), label_(v.label_) { }
        ~Vertex() { } 
        Vertex &operator=(const Vertex &v) {
            if (this != &v) {
                id_ = v.id_;
                label_ = v.label_;
            }
            return(*this);
        }

        VertexId id_;
        std::string label_;
    };
    typedef std::map<VertexId, Vertex> Vertices;

    struct Edge {
        Edge(): exists_(false), color_(""), weight_(-1) { }
        Edge(bool e): 
            exists_(e), color_(""), weight_(-1) { }
        Edge(bool e, const std::string &c): 
            exists_(e), color_(c), weight_(-1) { }
        Edge(bool e, Weight w): 
            exists_(e), color_(""), weight_(w) { }
        Edge(bool e, const std::string &c, Weight w): 
            exists_(e), color_(c), weight_(w) { }
        Edge(const Edge &e): 
            exists_(e.exists_), color_(e.color_), weight_(e.weight_) { }
        ~Edge() { }
        Edge &operator=(const Edge &e) {
            if (this != &e) {
                exists_ = e.exists_;
                color_ = e.color_;
                weight_ = e.weight_;
            }
            return(*this);
        }

        bool exists_;
        std::string color_;
        Weight weight_;
    };
    typedef std::map<VertexId, Edge> VertexEdges;
    typedef std::map<VertexId, VertexEdges> Edges;

public:
    // ctors and dtor
    Graph();
    Graph(const Graph &src);
    Graph(const std::string &filename);
    ~Graph();

    // operations
    Graph &operator=(const Graph &rhs);
    bool P(VertexId v0, VertexId v1) const;
    std::set<VertexId> P(VertexId v1) const;
    std::set<VertexId> getFromVertices(VertexId v1) const;
    std::set<VertexId> getToVertices(VertexId v1) const;
    std::set<std::string> getFromVerticesLabels(VertexId v1) const;
    std::set<std::string> getToVerticesLabels(VertexId v1) const;
    const Vertices &getVertices() const { return(vertices_); }
    const std::string color(VertexId v0, VertexId v1) const;
    const std::string label(VertexId vid) const;
    Vertex vertex(VertexId vid) const;
    Edge edge(VertexId v0, VertexId v1) const;
    bool connected() const;
    void connected(VertexId vid, long &numvisited) const;
    void connectedViaTo(VertexId vid, long &numvisited) const;
    void connectedViaFrom(VertexId vid, long &numvisited) const;

    // build graph
    void clear();
    void insertVertex(VertexId vid, const std::string &color, bool check = true);
    void insertEdge(VertexId fromvid, VertexId tovid);
    void insertEdge(VertexId fromvid, VertexId tovid, Weight weight);
    void insertEdge(VertexId fromvid, VertexId tovid, const std::string &color);
    void insertEdge(VertexId fromvid, VertexId tovid, const std::string &color, Weight weight);

    // misc
    bool empty() const { return(vertices_.size() == 0); }
    friend std::ostream &operator<<(std::ostream &os, const Graph &g);

private:
    // utilities
    void readfile(const std::string &filename);
    void parse(const std::string &line, std::map<std::string, std::string> &nvs);
    std::set<VertexId> getVertices(VertexId vid, const Edges &edges) const;
    void connected(VertexId vid, long &numvisited, std::set<VertexId> &visited) const;
    void connectedViaTo(VertexId vid, long &numvisited, std::set<VertexId> &visited) const;
    void connectedViaFrom(VertexId vid, long &numvisited, std::set<VertexId> &visited) const;

private:
    // data
    static VertexId nextVertexId_;
    int numberOfVertices_;
    int numberOfEdges_;
    Vertices vertices_;
    Edges fromedges_;
    Edges toedges_;
};

}

#endif
