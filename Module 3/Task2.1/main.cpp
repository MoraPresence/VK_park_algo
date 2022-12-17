//********************************************************************************************************************//
//                                                2. Количество различных путей                                       //
// Дан невзвешенный неориентированный граф. В графе может быть несколько кратчайших путей между какими-то вершинами.  //
// Найдите количество различных кратчайших путей между заданными вершинами. Требуемая сложность O(V+E).               //
//********************************************************************************************************************//

#include <cassert>
#include <functional>
#include <iostream>
#include <queue>
#include <vector>

template<typename T>
class IGraph{
public:
    virtual ~IGraph() = default;

    virtual void AddEdge(T from, T to) = 0;
    [[nodiscard]] virtual size_t VerticesCount() const  = 0;
    virtual std::vector<T> GetNextVertices(T vertex) const = 0;
    virtual std::vector<T> GetPrevVertices(T vertex) const = 0;
};

template<typename T>
void BFS(const IGraph<T> &graph, size_t vertex, std::vector<bool> &visited, const std::function<void(T)> &func) {
    std::queue<T> qu;
    qu.push(vertex);
    visited[vertex] = true;

    while (!qu.empty()) {
        auto currentVertex = qu.front();
        qu.pop();

        func(currentVertex);

        for (auto nextVertex: graph.GetNextVertices(currentVertex)) {
            if (!visited[nextVertex]) {
                visited[nextVertex] = true;
                qu.push(nextVertex);
            }
        }
    }
}

template<typename T>
void mainBFS(const IGraph<T> &graph, const std::function<void(size_t)> &func) {
    std::vector<bool> visited(graph.VerticesCount(), false);

    for (size_t i = 0; i < graph.VerticesCount(); ++i) {
        if (!visited[i]) {
            BFS(graph, i, visited, func);
        }
    }
}

template<typename T>
size_t getShortestPath(const IGraph<T> &graph, size_t& from,  size_t& to) {
    if(from > to){
        std::swap(from, to);
    }
    std::vector<size_t> shortestPath(graph.VerticesCount(), UINT64_MAX);
    std::vector<size_t> countShortestPath(graph.VerticesCount(), 0);

    countShortestPath[from] = 1;


    mainBFS(graph, [&graph, &shortestPath, &countShortestPath](size_t vertex){
        for(auto &it : graph.GetNextVertices(vertex)){
            if(shortestPath[vertex] + 1 < shortestPath[it]){
                shortestPath[it] = shortestPath[vertex] + 1;
                countShortestPath[it] = countShortestPath[vertex];
            }else if(shortestPath[vertex] + 1 == shortestPath[it]){
                countShortestPath[it] += countShortestPath[vertex];
            }
        }
    });

    return countShortestPath[to];
}

struct ListGraph : public IGraph<size_t> {
public:
    explicit ListGraph(const size_t &size);
    explicit ListGraph(const IGraph &graph);
    void AddEdge(size_t from, size_t to) override;
    [[nodiscard]] size_t VerticesCount() const override;
    [[nodiscard]] std::vector<size_t> GetNextVertices(size_t vertex) const override;
    [[nodiscard]] std::vector<size_t> GetPrevVertices(size_t vertex) const override;

    ~ListGraph() override = default;

private:
    std::vector<std::vector<size_t>> adjacencyLists;
};

ListGraph::ListGraph(const size_t &size)
        : adjacencyLists(size) {
}

ListGraph::ListGraph(const IGraph &graph)
        : adjacencyLists(graph.VerticesCount()) {
    for (size_t i = 0; i < graph.VerticesCount(); ++i) {
        adjacencyLists[i] = graph.GetNextVertices(i);
    }
}

void ListGraph::AddEdge(size_t from, size_t to) {
    assert(from >= 0 && from < adjacencyLists.size());
    assert(to >= 0 && to < adjacencyLists.size());
    adjacencyLists[from].push_back(to);
}

size_t ListGraph::VerticesCount() const {
    return adjacencyLists.size();
}

std::vector<size_t> ListGraph::GetNextVertices(size_t vertex) const {
    assert(vertex >= 0 && vertex < adjacencyLists.size());
    return adjacencyLists[vertex];
}

std::vector<size_t> ListGraph::GetPrevVertices(size_t vertex) const {
    assert(vertex >= 0 && vertex < adjacencyLists.size());
    std::vector<size_t> prevVertices;

    for (size_t from = 0; from < adjacencyLists.size(); ++from) {
        for (auto &to: adjacencyLists[from]) {
            if (to == vertex) {
                prevVertices.push_back(from);
            }
        }
    }

    return prevVertices;
}

int main(){
    size_t n_vertices = 0;
    size_t n_edges = 0;

    std::cin >> n_vertices >> n_edges;

    ListGraph graph(n_vertices);

    size_t v = 0;
    size_t w = 0;

    for (size_t i = 0; i < n_edges; ++i) {
        std::cin >> v >> w;
        graph.AddEdge(v, w);
        graph.AddEdge(w, v);
    }

    size_t from = 0;
    size_t to = 0;

    std::cin >> from >> to;

    std::cout << getShortestPath(graph, from, to) << std::endl;
}
