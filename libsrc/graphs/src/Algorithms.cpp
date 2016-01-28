
#include <stack>
#include <queue>
#include "hdr/Graph.h"
#include "hdr/Algorithms.h"

namespace ombt {

void 
depthFirstTraversal(const Graph &graph, bool verbose)
{
    std::set<Graph::VertexId> visited;

    const Graph::Vertices vertices = graph.getVertices();
    Graph::Vertices::const_iterator vit = vertices.begin();
    Graph::Vertices::const_iterator vitend = vertices.end();
    for ( ; vit!=vitend; ++vit)
    {
        Graph::VertexId vid = vit->first;
        if (visited.count(vid) != 0)
        {
            // already visited, skip it.
            continue;
        }

        std::stack<Graph::VertexId> stack;
        stack.push(vid);
        while (!stack.empty())
        {
            Graph::VertexId vid2 = stack.top();
            stack.pop();
            if (visited.count(vid2) != 0)
            {
                // already visited, skip it.
                continue;
            }
            if (verbose) std::cout << "Visited: (" << graph.vertex(vid2).id_ << "," << graph.vertex(vid2).label_ << ")" << std::endl;
            visited.insert(vid2);

            const std::set<Graph::VertexId> fromvid2set = graph.getFromVertices(vid2);
            std::set<Graph::VertexId>::const_iterator vit2 = fromvid2set.begin();
            std::set<Graph::VertexId>::const_iterator vit2end = fromvid2set.end();
            for ( ; vit2!=vit2end; ++vit2)
            {
                Graph::VertexId vid3 = *vit2;
                if (visited.count(vid3) == 0)
                {
                    // still need to visit.
                    stack.push(vid3);
                }
            }
        }
    }
}

void 
depthFirstTraversal(const Graph &graph, Graph::VertexId vid, bool verbose)
{
    std::set<Graph::VertexId> visited;
    std::stack<Graph::VertexId> stack;
    stack.push(vid);
    while (!stack.empty())
    {
        Graph::VertexId vid2 = stack.top();
        stack.pop();
        if (visited.count(vid2) != 0)
        {
            // already visited, skip it.
            continue;
        }
        if (verbose) std::cout << "Visited: (" << graph.vertex(vid2).id_ << "," << graph.vertex(vid2).label_ << ")" << std::endl;
        visited.insert(vid2);

        const std::set<Graph::VertexId> fromvid2set = graph.getFromVertices(vid2);
        std::set<Graph::VertexId>::const_iterator vit2 = fromvid2set.begin();
        std::set<Graph::VertexId>::const_iterator vit2end = fromvid2set.end();
        for ( ; vit2!=vit2end; ++vit2)
        {
            Graph::VertexId vid3 = *vit2;
            if (visited.count(vid3) == 0)
            {
                // still need to visit.
                stack.push(vid3);
            }
        }
    }
}

void 
depthFirstTraversal2(const Graph &graph, bool verbose)
{
    std::set<Graph::VertexId> visited;

    const Graph::Vertices vertices = graph.getVertices();
    Graph::Vertices::const_iterator vit = vertices.begin();
    Graph::Vertices::const_iterator vitend = vertices.end();
    for ( ; vit!=vitend; ++vit)
    {
        std::stack<Graph::VertexId> stack;
        stack.push(vit->first);
        while (!stack.empty())
        {
            Graph::VertexId vid2 = stack.top();
            stack.pop();
            if (visited.count(vid2) != 0)
            {
                // already visited, skip it.
                continue;
            }
            if (verbose) std::cout << "Visited: (" << graph.vertex(vid2).id_ << "," << graph.vertex(vid2).label_ << ")" << std::endl;
            visited.insert(vid2);

            const std::set<Graph::VertexId> fromvid2set = graph.getFromVertices(vid2);
            std::set<Graph::VertexId>::const_iterator vit2 = fromvid2set.begin();
            std::set<Graph::VertexId>::const_iterator vit2end = fromvid2set.end();
            for ( ; vit2!=vit2end; ++vit2)
            {
                stack.push(*vit2);
            }
        }
    }
}

void 
breadthFirstTraversal(const Graph &graph, bool verbose)
{
    std::set<Graph::VertexId> visited;

    const Graph::Vertices vertices = graph.getVertices();
    Graph::Vertices::const_iterator vit = vertices.begin();
    Graph::Vertices::const_iterator vitend = vertices.end();
    for ( ; vit!=vitend; ++vit)
    {
        Graph::VertexId vid = vit->first;
        if (visited.count(vid) != 0)
        {
            // already visited, skip it.
            continue;
        }

        std::queue<Graph::VertexId> queue;
        queue.push(vid);
        while (!queue.empty())
        {
            Graph::VertexId vid2 = queue.front();
            queue.pop();
            if (visited.count(vid2) != 0)
            {
                // already visited, skip it.
                continue;
            }
            if (verbose) std::cout << "Visited: (" << graph.vertex(vid2).id_ << "," << graph.vertex(vid2).label_ << ")" << std::endl;
            visited.insert(vid2);

            const std::set<Graph::VertexId> fromvid2set = graph.getFromVertices(vid2);
            std::set<Graph::VertexId>::const_iterator vit2 = fromvid2set.begin();
            std::set<Graph::VertexId>::const_iterator vit2end = fromvid2set.end();
            for ( ; vit2!=vit2end; ++vit2)
            {
                Graph::VertexId vid3 = *vit2;
                if (visited.count(vid3) == 0)
                {
                    // still need to visit.
                    queue.push(vid3);
                }
            }
        }
    }
}

void 
breadthFirstTraversal(const Graph &graph, Graph::VertexId vid, bool verbose)
{
    std::set<Graph::VertexId> visited;
    std::queue<Graph::VertexId> queue;
    queue.push(vid);
    while (!queue.empty())
    {
        Graph::VertexId vid2 = queue.front();
        queue.pop();
        if (visited.count(vid2) != 0)
        {
            // already visited, skip it.
            continue;
        }
        if (verbose) std::cout << "Visited: (" << graph.vertex(vid2).id_ << "," << graph.vertex(vid2).label_ << ")" << std::endl;
        visited.insert(vid2);

        const std::set<Graph::VertexId> fromvid2set = graph.getFromVertices(vid2);
        std::set<Graph::VertexId>::const_iterator vit2 = fromvid2set.begin();
        std::set<Graph::VertexId>::const_iterator vit2end = fromvid2set.end();
        for ( ; vit2!=vit2end; ++vit2)
        {
            Graph::VertexId vid3 = *vit2;
            if (visited.count(vid3) == 0)
            {
                // still need to visit.
                queue.push(vid3);
            }
        }
    }
}

void 
breadthFirstTraversal2(const Graph &graph, bool verbose)
{
    std::set<Graph::VertexId> visited;

    const Graph::Vertices vertices = graph.getVertices();
    Graph::Vertices::const_iterator vit = vertices.begin();
    Graph::Vertices::const_iterator vitend = vertices.end();
    for ( ; vit!=vitend; ++vit)
    {
        std::queue<Graph::VertexId> queue;
        queue.push(vit->first);
        while (!queue.empty())
        {
            Graph::VertexId vid2 = queue.front();
            queue.pop();
            if (visited.count(vid2) != 0)
            {
                // already visited, skip it.
                continue;
            }
            if (verbose) std::cout << "Visited: (" << graph.vertex(vid2).id_ << "," << graph.vertex(vid2).label_ << ")" << std::endl;
            visited.insert(vid2);

            const std::set<Graph::VertexId> fromvid2set = graph.getFromVertices(vid2);
            std::set<Graph::VertexId>::const_iterator vit2 = fromvid2set.begin();
            std::set<Graph::VertexId>::const_iterator vit2end = fromvid2set.end();
            for ( ; vit2!=vit2end; ++vit2)
            {
                queue.push(*vit2);
            }
        }
    }
}

}
