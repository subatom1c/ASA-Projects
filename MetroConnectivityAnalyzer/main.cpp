#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <tuple>

using namespace std;

struct Edge {
    int to, line;
};

typedef tuple<int, int, int> State;

struct Compare {
    bool operator()(const State& a, const State& b){
        return get<0>(a) > get<0> (b);
    }
};

const int INF = INT_MAX;

void print_lines_graph(const vector<vector<int>>& lines_graph, int total_lines) {
    cout << "Lines Graph:" << endl;
    for (int i = 1; i <= total_lines; i++) { // Start from line 1 (1-based indexing)
        cout << "Line " << i << " connects to: ";
        for (int neighbor : lines_graph[i]) {
            cout << neighbor << " "; // Print each neighboring line
        }
        cout << endl; // Newline after printing all neighbors for this line
    }
}


int bfs_line_changes(int start_line, const vector<vector<int>>& lines_graph, int total_lines) {
    
    vector<int> dist(total_lines + 1, -1);
    vector<int> visited(total_lines + 1, -1); // Distance from start_line (-1 means not visited)
    queue<int> q;

    // Initialize BFS
    q.push(start_line);
    visited[start_line] = 0;
    dist[start_line] = 0;

  
   
    while (!q.empty()) {
        int current_line = q.front();
       
        q.pop();

        // Traverse all neighboring lines
        for (int neighbor_line : lines_graph[current_line]) {
            if (visited[neighbor_line] == -1) { // If not visited
              
                visited[neighbor_line] = 1; // Is visited
                dist[neighbor_line] = dist[current_line] + 1;
                q.push(neighbor_line);
            }
        }
    }

    int max_dist = 0;
    // Check if all lines are reachable
    for (int i = 1; i <= total_lines; i++) {
        if (dist[i] == -1) return -1;
        max_dist = max(max_dist, dist[i]);
    }
    return max_dist; // Return the maximum line changes
}

// Procura linear
bool find_in_vector(vector<int>& lines_graph, int value){
    int len = lines_graph.size();
    for (int i = 0; i < len; i++){
        if (lines_graph[i]  == value){
            return true;
        }
    }
    return false;
}

bool is_all_points_connected(const vector<vector<Edge>>& graph, int n) {
    vector<bool> visited(n + 1, false);
    queue<int> q;
    
    q.push(1); // Start from node 1 (or any arbitrary point)
    visited[1] = true;

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        for (const auto& edge : graph[current]) {
            int neighbor = edge.to;
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }

    // Check if all points are visited
    for (int i = 1; i <= n; ++i) {
        if (!visited[i]) {
            return false; // If any point is not reachable, return false
        }
    }

    return true; // All points are connected
}

bool is_singular_line(int l, int n, const vector<vector<Edge>>& graph) {
    bool found_single_line = false;

    for (int line = 1; line <= l; ++line) { // O(l)
        vector<bool> visited(n + 1, false);
        queue<int> q;

        // Find the first station that uses this line
        for (int i = 1; i <= n; ++i) { // O(n * l)
            for (const Edge& edge : graph[i]) { // O(m * n * l)
                if (edge.line == line) {
                    q.push(i);
                    visited[i] = true;
                    break; // Only need one starting point
                }
            }
        }

        // BFS to check if all stations are reachable via this line
        while (!q.empty()) { // O(n * l)
            int station = q.front();
            q.pop();

            for (const Edge& edge : graph[station]) { // O(m * n * l)
                if (edge.line == line && !visited[edge.to]) {
                    visited[edge.to] = true;
                    q.push(edge.to);
                }
            }
        }

        // Check if all stations are visited
        bool all_visited = true;
        for (int i = 1; i <= n; ++i) {
            if (!visited[i]) {
                all_visited = false;
                break;
            }
        }

        if (all_visited) {
            found_single_line = true;
            break;
        }
    }

    if (found_single_line) {
        return false;
    }

    return true;
}

int main() {
    int n, m, l;
    cin >> n >> m >> l;

    vector<vector<Edge>> graph(n + 1);
    vector<vector<int>> lines_graph(l + 1);

    // Input the graph
    for (int i = 0; i < m; ++i) {
        int u, v, line;
        cin >> u >> v >> line;
        graph[u].push_back(Edge{v, line});
        graph[v].push_back(Edge{u, line}); // Undirected graph
    }

    

    if (!is_all_points_connected(graph, n)){
        cout << -1 << endl;
        return 0;
    }

    if (!is_singular_line(l, n, graph)){
        cout << 0 << endl;
        return 0;
    }

    // Creation of lines graph
    // Idea: Start in a certain point and check its adjacencies
    for (int i = 1 ; i <= n; i++){ 
        for (size_t j = 0; j < graph[i].size(); j++){ //
            for (size_t k = j + 1; k < graph[i].size(); k++) { 
                int line1 = graph[i][j].line;
                int line2 = graph[i][k].line;
                if (line1 != line2) {
                if (!find_in_vector(lines_graph[line1], line2)){ //
                    lines_graph[line1].push_back(line2);
                }
                if (!find_in_vector(lines_graph[line2], line1)){
                    lines_graph[line2].push_back(line1);
                }
                }
            }
        }
    }
    
    

        int result = 0;

    // Run BFS for each node to determine the global maximum line changes
    for (int i = 1; i <= l; ++i) {
        int max_changes = bfs_line_changes(i, lines_graph, l);
        if (max_changes == -1) {
            cout << -1 << endl;
            return 0;
        }
        result = max(result, max_changes);
    }
    cout << result << endl;
    return 0;
}
