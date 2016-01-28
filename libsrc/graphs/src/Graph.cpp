
#include "hdr/Graph.h"

namespace ombt {

// sample graph file
// v: id:1 label:a
// v: id:2 label:b
// v: id:3 label:c
// v: id:4 label:e
// v: id:5 label:f
// e: fromid:1 toid:2 color:blue weight:10
// e: fromid:2 toid:3 color:blue
// e: fromid:3 toid:4 weight:10
// e: fromid:4 toid:5 

Graph::VertexId Graph::nextVertexId_ = 0;

Graph::Graph():
    numberOfVertices_(0),
    numberOfEdges_(0),
    vertices_(),
    fromedges_(),
    toedges_()
{
}

Graph::Graph(const Graph &src):
    numberOfVertices_(src.numberOfVertices_),
    numberOfEdges_(src.numberOfEdges_),
    vertices_(src.vertices_),
    fromedges_(src.fromedges_),
    toedges_(src.toedges_)
{
}

Graph::Graph(const std::string &filename):
    numberOfVertices_(0),
    numberOfEdges_(0),
    vertices_(),
    fromedges_(),
    toedges_()
{
    readfile(filename);
}

Graph::~Graph()
{
}

Graph &
Graph::operator=(const Graph &rhs)
{
    if (this != &rhs)
    {
        numberOfVertices_ = rhs.numberOfVertices_;
        numberOfEdges_ = rhs.numberOfEdges_;
        vertices_ = rhs.vertices_;
        fromedges_ = rhs.fromedges_;
        toedges_ = rhs.toedges_;
    }
    return(*this);
}

void
Graph::parse(const std::string &line, 
             std::map<std::string, std::string> &nvs)
{
    nvs.clear();

    Tokenizer tokenizer;
    std::vector<std::string> tokens;
    tokenizer(line, tokens, " ");

    for (int i=0; i<tokens.size(); ++i)
    {
        std::vector<std::string> pair;
        tokenizer(tokens[i], pair, ":");
        if (pair.size() == 1)
        {
            nvs[pair[0]] = "";
        }
        else if (pair.size() == 2)
        {
            nvs[pair[0]] = pair[1];
        }
    }
}

void 
Graph::readfile(const std::string &filename)
{
    std::cout << std::endl;
    std::cout << "Reading file: " << filename << std::endl;
    std::ifstream infile(filename.c_str());
    assert(infile.is_open());

    std::cout << std::endl;
    Tokenizer tokenizer;
    while (infile.good())
    {
        std::string line;
        std::getline(infile, line);

        std::map<std::string, std::string> nvs;
        parse(line, nvs);

        if (nvs.find("#") != nvs.end())
        {
            // skip comments
            continue;
        }
        else if (nvs.find("v") != nvs.end())
        {
            std::cout << "Vertex: " << line << std::endl;
            VertexId vid;
            std::istringstream(nvs["id"]) >> vid;
            insertVertex(vid, nvs["label"]);
        }
        else if (nvs.find("e") != nvs.end())
        {
            std::cout << "Edge: " << line << std::endl;
            VertexId toid, fromid;
            std::istringstream(nvs["fromid"]) >> fromid;
            std::istringstream(nvs["toid"]) >> toid;
            bool havecolor = (nvs.find("color") != nvs.end());
            bool haveweight = (nvs.find("weight") != nvs.end());
            if (haveweight)
            {
                Weight weight;
                std::istringstream(nvs["weight"]) >> weight;
                if (havecolor)
                    insertEdge(fromid, toid, nvs["color"], weight);
                else
                    insertEdge(fromid, toid, weight);
            }
            else if (havecolor)
            {
                insertEdge(fromid, toid, nvs["color"]);
            }
            else 
            {
                insertEdge(fromid, toid);
            }
        }
    }
    infile.close();
}

std::ostream &operator<<(std::ostream &os, const Graph &g)
{
    // v: id:1 label:a
    os << std::endl;
    Graph::Vertices::const_iterator vit = g.vertices_.begin();
    Graph::Vertices::const_iterator vitend = g.vertices_.end();
    for ( ; vit!=vitend; ++vit)
    {
        os << "v:";
        os << " id: " << vit->second.id_;
        os << " label:" << vit->second.label_;
        os << std::endl;
    }

    // e: fromid:1 toid:2 color:blue
    Graph::Edges::const_iterator feit = g.fromedges_.begin();
    Graph::Edges::const_iterator feitend = g.fromedges_.end();
    for ( ; feit!=feitend; ++feit)
    {
        Graph::VertexEdges::const_iterator veit = feit->second.begin();
        Graph::VertexEdges::const_iterator veitend = feit->second.end();
        for ( ; veit!=veitend; ++veit)
        {
            os << "e:";
            os << " fromid:" << feit->first;
            os << " toid:" << veit->first;
            os << " color:" << veit->second.color_;
            os << " weight:" << veit->second.weight_;
            os << std::endl;
        }
    }
    return(os);
}

void 
Graph::insertVertex(Graph::VertexId vid, const std::string &label, bool check)
{
    if (check)
    {
        Vertices::iterator vit = vertices_.find(vid);
        Vertices::iterator vitend = vertices_.end();
        if (vit != vitend) abort();
    }
    vertices_[vid].id_ = vid;
    vertices_[vid].label_ = label;
}

void 
Graph::insertEdge(Graph::VertexId v0, Graph::VertexId v1, const std::string &color)
{
    // v0 ==>> v1 (transition *from* v0)
    fromedges_[v0].insert(
        std::pair<VertexId, Edge>(v1, Edge(true, color)));
    // v0 ==>> v1 (transition *to* v1)
    toedges_[v1].insert(
        std::pair<VertexId, Edge>(v0, Edge(true, color)));
}

void 
Graph::insertEdge(Graph::VertexId v0, Graph::VertexId v1)
{
    // v0 ==>> v1 (transition *from* v0)
    fromedges_[v0].insert(
        std::pair<VertexId, Edge>(v1, Edge(true)));
    // v0 ==>> v1 (transition *to* v1)
    toedges_[v1].insert(
        std::pair<VertexId, Edge>(v0, Edge(true)));
}

void 
Graph::insertEdge(Graph::VertexId v0, Graph::VertexId v1, Graph::Weight weight)
{
    // v0 ==>> v1 (transition *from* v0)
    fromedges_[v0].insert(
        std::pair<VertexId, Edge>(v1, Edge(true, weight)));
    // v0 ==>> v1 (transition *to* v1)
    toedges_[v1].insert(
        std::pair<VertexId, Edge>(v0, Edge(true, weight)));
}

void 
Graph::insertEdge(Graph::VertexId v0, Graph::VertexId v1, const std::string &color, Graph::Weight weight)
{
    // v0 ==>> v1 (transition *from* v0)
    fromedges_[v0].insert(
        std::pair<VertexId, Edge>(v1, Edge(true, color, weight)));
    // v0 ==>> v1 (transition *to* v1)
    toedges_[v1].insert(
        std::pair<VertexId, Edge>(v0, Edge(true, color, weight)));
}

bool 
Graph::P(VertexId v0, VertexId v1) const
{
    Edges::const_iterator eit = fromedges_.find(v0);
    if (eit == fromedges_.end()) return(false);
    VertexEdges::const_iterator veit = eit->second.find(v1);;
    if (veit == eit->second.end()) return(false);
    return(veit->second.exists_);
}

std::set<Graph::VertexId> 
Graph::getVertices(Graph::VertexId vid, const Graph::Edges &edges) const
{
    Edges::const_iterator eit = edges.find(vid);
    if (eit == edges.end()) 
    {
        return(std::set<Graph::VertexId>());
    }
    std::set<Graph::VertexId> vertices;
    VertexEdges::const_iterator veit = eit->second.begin();
    VertexEdges::const_iterator veitend = eit->second.end();
    for ( ; veit!=veitend; ++veit)
    {
        vertices.insert(veit->first);
    }
    return(vertices);
}

std::set<Graph::VertexId> 
Graph::P(Graph::VertexId v1) const
{
    return(getVertices(v1, toedges_));
}

std::set<Graph::VertexId> 
Graph::getFromVertices(VertexId vid) const
{
    return(getVertices(vid, fromedges_));
}

std::set<Graph::VertexId> 
Graph::getToVertices(VertexId vid) const
{
    return(getVertices(vid, toedges_));
}

const std::string
Graph::color(Graph::VertexId v0, Graph::VertexId v1) const
{
    Edges::const_iterator eit = fromedges_.find(v0);
    if (eit == fromedges_.end()) return(std::string(""));
    VertexEdges::const_iterator veit = eit->second.find(v1);;
    if (veit == eit->second.end()) return(std::string(""));
    return(veit->second.color_);
}

const std::string
Graph::label(Graph::VertexId vid) const
{
    Vertices::const_iterator vit = vertices_.find(vid);
    if (vit != vertices_.end())
        return(vit->second.label_);
    else
        return("");
}

Graph::Vertex
Graph::vertex(Graph::VertexId vid) const
{
    Vertices::const_iterator vit = vertices_.find(vid);
    if (vit != vertices_.end())
        return(vit->second);
    else
        return(Graph::Vertex());
}

Graph::Edge
Graph::edge(Graph::VertexId v0, Graph::VertexId v1) const
{
    Edges::const_iterator eit = fromedges_.find(v0);
    if (eit == fromedges_.end()) return(Graph::Edge());
    VertexEdges::const_iterator veit = eit->second.find(v1);
    if (veit == eit->second.end()) return(Graph::Edge());
    return(veit->second);
}

void
Graph::clear()
{
    vertices_.clear();
    fromedges_.clear();
    toedges_.clear();
}

void
Graph::connected(Graph::VertexId vid, long &numvisited,
                 std::set<Graph::VertexId> &visited) const
{
    numvisited += 1;
    visited.insert(vid);

    std::set<Graph::VertexId> toset = getToVertices(vid);
    std::set<Graph::VertexId>::iterator tosetit = toset.begin();
    std::set<Graph::VertexId>::iterator tosetitend = toset.end();
    for ( ; tosetit!=tosetitend; ++tosetit)
    {
        Graph::VertexId tosetvid = *tosetit;
        if (visited.find(tosetvid) == visited.end())
        {
            connected(tosetvid, numvisited, visited);
        }
    }
    std::set<Graph::VertexId> fromset = getFromVertices(vid);
    std::set<Graph::VertexId>::iterator fromsetit = fromset.begin();
    std::set<Graph::VertexId>::iterator fromsetitend = fromset.end();
    for ( ; fromsetit!=fromsetitend; ++fromsetit)
    {
        Graph::VertexId fromsetvid = *fromsetit;
        if (visited.find(fromsetvid) == visited.end())
        {
            connected(fromsetvid, numvisited, visited);
        }
    }
}

void
Graph::connectedViaTo(Graph::VertexId vid, long &numvisited, 
                      std::set<Graph::VertexId> &visited) const
{
    std::set<Graph::VertexId> toset = getToVertices(vid);
    std::set<Graph::VertexId>::iterator tosetit = toset.begin();
    std::set<Graph::VertexId>::iterator tosetitend = toset.end();
    for ( ; tosetit!=tosetitend; ++tosetit)
    {
        Graph::VertexId tosetvid = *tosetit;
        if (visited.find(tosetvid) == visited.end())
        {
            numvisited += 1;
            visited.insert(tosetvid);
            connectedViaTo(tosetvid, numvisited, visited);
        }
    }
}

void
Graph::connectedViaFrom(Graph::VertexId vid, long &numvisited, 
                        std::set<Graph::VertexId> &visited) const
{
    std::set<Graph::VertexId> fromset = getFromVertices(vid);
    std::set<Graph::VertexId>::iterator fromsetit = fromset.begin();
    std::set<Graph::VertexId>::iterator fromsetitend = fromset.end();
    for ( ; fromsetit!=fromsetitend; ++fromsetit)
    {
        Graph::VertexId fromsetvid = *fromsetit;
        if (visited.find(fromsetvid) == visited.end())
        {
            numvisited += 1;
            visited.insert(fromsetvid);
            connectedViaFrom(fromsetvid, numvisited, visited);
        }
    }
}

void
Graph::connected(Graph::VertexId vid, long &numvisited) const
{
    std::set<Graph::VertexId> visited;
    connected(vid, numvisited, visited);
}

void
Graph::connectedViaTo(Graph::VertexId vid, long &numvisited) const
{
    std::set<Graph::VertexId> visited;
    connectedViaTo(vid, numvisited, visited);
}

void
Graph::connectedViaFrom(Graph::VertexId vid, long &numvisited) const
{
    std::set<Graph::VertexId> visited;
    connectedViaFrom(vid, numvisited, visited);
}

bool
Graph::connected() const
{
    Vertices::const_iterator vit = vertices_.begin();
    if (vit == vertices_.end()) return(false);
    long visited = 0;
    connected(vit->first, visited);
    return(visited == vertices_.size());
}

std::set<std::string> 
Graph::getFromVerticesLabels(VertexId vid) const
{
    std::set<std::string> labels;
    std::set<Graph::VertexId> fvset = getFromVertices(vid);
    std::set<Graph::VertexId>::const_iterator it = fvset.begin();
    std::set<Graph::VertexId>::const_iterator itend = fvset.end();
    for ( ; it!=itend; ++it)
    {
        labels.insert(vertex(*it).label_);
    }
    return(labels);
}

std::set<std::string> 
Graph::getToVerticesLabels(VertexId vid) const
{
    std::set<std::string> labels;
    std::set<Graph::VertexId> tvset = getToVertices(vid);
    std::set<Graph::VertexId>::const_iterator it = tvset.begin();
    std::set<Graph::VertexId>::const_iterator itend = tvset.end();
    for ( ; it!=itend; ++it)
    {
        labels.insert(vertex(*it).label_);
    }
    return(labels);
}

}
