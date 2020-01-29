package project;

import java.util.LinkedList;
import java.util.List;
import javax.swing.JOptionPane;

public class AdjMatrixGraph {

    int n;
    int[][] graph;
    int[][] flow;

    public AdjMatrixGraph(int n) {
        this.n = n;
        graph = new int[n][n];
        flow = new int[n][n];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                this.flow[i][j] = 0;
                this.graph[i][j] = 0;
            }
        }
    }

    public boolean checkEdge(int src, int dest) {
        return graph[src][dest] != 0;

    }

    public String Str(int[][] matrix) {
        String result = " |\t";
        for (int i = 0; i < matrix.length; i++) {
            result += i + "\t";
        }
        result += "\n";
        for (int i = 0; i < matrix.length; i++) {
            result += i + "|\t";
            for (int j = 0; j < matrix.length; j++) {
                result += matrix[i][j] + "\t";
            }
            result += "\n";
        }
        return result;
    }

    public void zeroFlow() {
          for (int i = 0; i < n; i++) 
            for (int j = 0; j < n; j++) {
                this.flow[i][j] = 0;
            }
    }

    public String printFlow(int numofv) {
        String result = "";
        for (int i = 0; i < numofv; i++) {
            for (int j = 0; j < n; j++) {
                if (checkEdge(i, j)) {
                    if (graph[i][j] == -1) {
                        for (int k = 0; k < n; k++) {
                            if (checkEdge(j, k)) {
                                if (flow[j][k] > 0) {
                                    result += i + "----" + flow[j][k] + "-->" + k + "\n";
                                }
                            }
                        }
                    } else if (flow[i][j] > 0) {
                        result += i + "----" + flow[i][j] + "-->" + j + "\n";
                    }
                }
            }
        }
        return result;
    }

    void addEdge(int u, int v, int w) {
        {
            this.graph[u][v] = w;
        }
    }

    public boolean BFS(int src, int[] parent, int dest) {
        boolean[] Status = new boolean[n];
        List<Integer> queue = new LinkedList<>();
        for (int i = 0; i < n; i++) {
            Status[i] = false;
            parent[i] = -1;
        }
        Status[src] = true;
        parent[src] = -1;
        queue.add(src);
        while (!queue.isEmpty()) {
            int next = queue.remove(0);
            for (int i = 0; i < n; i++) {
                if (checkEdge(next, i) && Status[i] == false) {
                    queue.add(i);
                    Status[i] = true;
                    parent[i] = next;
                }
            }
        }
        return (Status[dest] == true);
    }

    public AdjMatrixGraph createResduialGraph() {
        AdjMatrixGraph Rg = new AdjMatrixGraph(n);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                Rg.graph[i][j] = graph[i][j];
            }
        }
        return Rg;
    }

    public int fordFolkersonAlgoUsingBFS(AdjMatrixGraph Rg, int s, int t) {
        int max_flow = 0;
        int[] parent = new int[n];
        int i;
        long startTime, endTime, currentTime, totalTime = 0;
        startTime = System.nanoTime();
        while (Rg.BFS(s, parent, t)) {
            endTime = System.nanoTime();
            currentTime = (endTime - startTime) / 1000000000;
            int min = Integer.MAX_VALUE;
            for (int currnode = t; currnode != s; currnode = parent[currnode]) {
                i = parent[currnode];
                min = Math.min(min, Rg.graph[i][currnode]);
            }

            for (int currNode = t; currNode != s; currNode = parent[currNode]) {
                i = parent[currNode];
                if (checkEdge(i, currNode)) {
                    flow[i][currNode] += min;
                    Rg.graph[currNode][i] += min;
                    Rg.graph[i][currNode] -= min;
                } else {
                    flow[i][currNode] -= min;
                }
            }
            max_flow += min;
            totalTime += currentTime;
            startTime = System.nanoTime();
        }
        JOptionPane.showMessageDialog(null, "Total time to find augmenting path " + totalTime + " s");
        return max_flow;
    }

    public boolean widestPath(int[] parent, int src, int dest) {
        boolean[] Status = new boolean[n];
        LinkedList<Integer> s = new LinkedList<>();
        LinkedList<Integer> v = new LinkedList<>();
        LinkedList<Integer> wT = new LinkedList<>();
        for (int i = 0; i < n; i++) {
            Status[i] = false;
            parent[i] = -1;
            v.add(i);
        }
        Status[src] = true;
        parent[src] = -1;
        s.add(src);
        v.remove(src);
        int vertex = 0;
        int parentVertex = -1;
        while (wT.size() < n) {
            int max = Integer.MIN_VALUE;
            for (int i : s) {
                for (int j : v) {
                    if (!Status[j]) {
                        if (graph[i][j] != 0) {
                            if (graph[i][j] > max) {
                                max = graph[i][j];
                                parentVertex = i;
                                vertex = j;
                            }
                        }
                    }
                }
            }
            wT.add(max);
            if (!s.contains((Object) vertex)) {
                s.add(vertex);
                Status[vertex] = true;
                v.remove((Object) vertex);
                parent[vertex] = parentVertex;
            }
        }
        return Status[dest] == true;

    }

    public int fordFolkersonAlgoUsingWidestPath(AdjMatrixGraph Rg, int s, int t) {
        int max_flow = 0;
        int[] parent = new int[n];
        int i;
        long startTime, endTime, currentTime, totalTime = 0;
        startTime = System.nanoTime();
        while (Rg.widestPath(parent, s, t)) {
            endTime = System.nanoTime();
            currentTime = (endTime - startTime) / 1000000000;
            int min = Integer.MAX_VALUE;
            for (int currnode = t; currnode != s; currnode = parent[currnode]) {
                i = parent[currnode];
                min = Math.min(min, Rg.graph[i][currnode]);
            }

            for (int currNode = t; currNode != s; currNode = parent[currNode]) {
                i = parent[currNode];

                if (checkEdge(i, currNode)) {
                    flow[i][currNode] += min;
                    Rg.graph[currNode][i] += min;
                    Rg.graph[i][currNode] -= min;
                } else {
                    flow[i][currNode] -= min;
                }
            }
            max_flow += min;
            totalTime += currentTime;
            startTime = System.nanoTime();
        }
        JOptionPane.showMessageDialog(null, "Total time to find augmenting path " + totalTime + " s");
        return max_flow;
    }

    public int findMaxK(int[] K, boolean[] Status) {
        int maxVertex = -7;
        int maxX = Integer.MIN_VALUE;
        for (int i = 0; i < n; i++) {
            if (K[i] > maxX && Status[i] == false) {
                maxX = K[i];
                maxVertex = i;
            }
        }
        return maxVertex;
    }

    boolean findWidestPath(int[] parent, int src, int dest) {
        boolean Status[] = new boolean[n];
        int X[] = new int[n];
        for (int i = 0; i < n; i++) {
            Status[i] = false;
            parent[i] = -1;
            X[i] = Integer.MIN_VALUE;
        }
        X[src] = 0;
        int v = findMaxK(X, Status);
        while (v != -7) {

            Status[v] = true;
            for (int i = 0; i < n; i++) {
                if (checkEdge(v, i) && Status[i] == false && graph[v][i] > X[i]) {
                    X[i] = graph[v][i];
                    parent[i] = v;
                }
            }
            v = findMaxK(X, Status);
        }
        return Status[dest];
    }

    public int fordFulkersonWidestPathLargestKey(AdjMatrixGraph Rg, int s, int t) {
        int max_flow = 0;
        int[] parent = new int[n];
        int i;
        long startTime, endTime, currentTime, totalTime = 0;
        startTime = System.nanoTime();
        while (Rg.findWidestPath(parent, s, t)) {
            endTime = System.nanoTime();
            currentTime = (endTime - startTime) / 1000000000;
            int min = Integer.MAX_VALUE;
            for (int currnode = t; currnode != s; currnode = parent[currnode]) {
                i = parent[currnode];
                min = Math.min(min, Rg.graph[i][currnode]);
            }

            for (int currNode = t; currNode != s; currNode = parent[currNode]) {
                i = parent[currNode];

                if (checkEdge(i, currNode)) {
                    flow[i][currNode] += min;
                    Rg.graph[currNode][i] += min;
                    Rg.graph[i][currNode] -= min;
                } else {
                    flow[i][currNode] -= min;
                }
            }
            max_flow += min;
            totalTime += currentTime;
            startTime = System.nanoTime();
        }
        JOptionPane.showMessageDialog(null, "Total time to find augmenting path " + totalTime + " s");
        return max_flow;
    }
}
