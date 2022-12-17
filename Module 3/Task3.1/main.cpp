//********************************************************************************************************************//
//                                                3. Города                                                           //
// Требуется отыскать самый короткий маршрут между городами. Из города может выходить дорога,                         //
// которая возвращается в этот же город.                                                                              //
//                                                                                                                    //
// Требуемое время работы O((N + M)log N), где N – количество городов, M – известных дорог между ними.                //
// N ≤ 10000, M ≤ 250000.                                                                                             //
// Длина каждой дороги ≤ 10000.                                                                                       //
//********************************************************************************************************************//

#include <cassert>
#include <functional>
#include <iostream>
#include <vector>
#include <set>

template<typename T>
class IGraph{
public:
    virtual ~IGraph() = default;

    virtual void AddEdge(T from, T to) = 0;
    [[nodiscard]] virtual size_t VerticesCount() const  = 0;
    [[nodiscard]] virtual std::vector<T> GetNextVertices(size_t vertex) const = 0;
    [[nodiscard]] virtual std::vector<T> GetPrevVertices(size_t vertex) const = 0;
};

struct ListGraph : public IGraph<std::pair<int32_t, int32_t>> {
public:
explicit ListGraph(const size_t &size);
explicit ListGraph(const IGraph &graph);
void AddEdge(std::pair<int32_t, int32_t> from, std::pair<int32_t, int32_t> to) override;
[[nodiscard]] size_t VerticesCount() const override;
[[nodiscard]] std::vector<std::pair<int32_t, int32_t>> GetNextVertices(size_t vertex) const override;
[[nodiscard]] std::vector<std::pair<int32_t, int32_t>> GetPrevVertices(size_t vertex) const override;

~ListGraph() override = default;

private:
std::vector<std::vector<std::pair<int32_t, int32_t>>> adjacencyLists;
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

void ListGraph::AddEdge(std::pair<int32_t, int32_t> from, std::pair<int32_t, int32_t> to) {
    adjacencyLists[from.first].emplace_back(to);
}

size_t ListGraph::VerticesCount() const {
    return adjacencyLists.size();
}

std::vector<std::pair<int32_t, int32_t>> ListGraph::GetNextVertices(size_t vertex) const {
    assert(vertex >= 0 && vertex < adjacencyLists.size());
    return adjacencyLists[vertex];
}

std::vector<std::pair<int32_t, int32_t>> ListGraph::GetPrevVertices(size_t vertex) const {
    assert(vertex >= 0 && vertex < adjacencyLists.size());
    return adjacencyLists[vertex];
}

template<typename T>
int32_t GetShortestPath(const IGraph<T> &graph, int32_t from, int32_t to) {
    std::vector<int32_t> shortestPath(graph.VerticesCount(), INT32_MAX);
    std::set<std::pair<int32_t, int32_t>> priorityQueue;

    shortestPath[from] = 0;
    priorityQueue.emplace(0, from);

    for (; !priorityQueue.empty();) {
        auto vertex = *priorityQueue.begin();
        for (auto &nextVertex: graph.GetNextVertices(vertex.second)) {
            if (shortestPath[nextVertex.first] > shortestPath[vertex.second] + nextVertex.second) {
                auto vertexFromQueue = priorityQueue.find(std::pair(shortestPath[nextVertex.first], nextVertex.first));
                if (vertexFromQueue != priorityQueue.end()) {
                    priorityQueue.erase(vertexFromQueue);
                }
                shortestPath[nextVertex.first] = shortestPath[vertex.second] + nextVertex.second;
                priorityQueue.insert(std::pair<int32_t, int32_t>(shortestPath[nextVertex.first], nextVertex.first));
            }
        }
        priorityQueue.erase(priorityQueue.find(vertex));
    }

    return shortestPath[to];
}

int main() {
    size_t n_vertices = 0;
    size_t n_edges = 0;

    std::cin >> n_vertices >> n_edges;

    ListGraph graph(n_vertices);

    int32_t v = 0;
    int32_t w = 0;
    size_t weight = 0;

    for (int32_t i = 0; i < n_edges; ++i) {
        std::cin >> v >> w >> weight;
        graph.AddEdge(std::pair(v, weight), std::pair(w, weight));
        graph.AddEdge(std::pair(w, weight), std::pair(v, weight));
    }

    int32_t from = 0;
    int32_t to = 0;

    std::cin >> from >> to;
    std::cout << GetShortestPath(graph, from, to) << std::endl;
    return 0;
}
