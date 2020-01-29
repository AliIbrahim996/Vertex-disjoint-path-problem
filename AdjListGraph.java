package project;

import java.util.LinkedList;
import java.util.List;
import javax.swing.JOptionPane;

/**
 *
 * @author Ali_ibrahim
 */
class Edge {

    int dest, w, Capacity;

    Edge(int dest, int w, int c) {

        this.dest = dest;
        this.w = w;
        this.Capacity = c;
    }

    @Override
    public String toString() {
        return "(" + dest + "," + w + "," + Capacity + ")";
    }
}

public class AdjListGraph {

    List<Edge> g[];
    int n;

    AdjListGraph(int n) {
        this.n = n;
        g = new LinkedList[n];
        for (int i = 0; i < this.n; i++) {
            g[i] = new LinkedList<>();
        }
    }

    public boolean cheackEdge(int u, int v) {
        for (Edge e : g[u]) {
            if (e.dest == v) {
                return true;
            }
        }
        return false;
    }

    public void addEdge(int u, int v, int w) {
        g[u].add(new Edge(v, w, 0));
        g[v].add(new Edge(u, 0, 0));

    }

    public void zeroFlow() {
        for(int i=0;i<n;i++)
        for(Edge e:g[i])
            e.Capacity=0;
    }

    public int GetCapacity(int u, int v) {
        for (Edge i : g[u]) {
            if (i.dest == v) {
                return i.w;
            }
        }
        return 0;
    }

    public void updateCapacity(Edge e, int c) {
        e.Capacity = c;
    }

    public void updateWeight(int s, int dest, int w) {
        for (Edge i : g[s]) {
            if (i.dest == dest) {
                i.w += w;
            }
        }
    }

    public void updateWeight(Edge e, int w) {
        e.w = w;
    }

    public int GetIndex(int u, int v) {
        for (Edge i : g[u]) {
            if (i.dest == v) {
                int j = g[u].indexOf(i);
                return j;
            }
        }
        return 0;
    }

    public AdjListGraph creatResidualGraph() {
        AdjListGraph Rg = new AdjListGraph(n);
        for (int i = 0; i < n; i++) {
            for (Edge e : g[i]) {
                Rg.g[i].add(new Edge(e.dest, e.w, e.Capacity));
            }
        }
        return Rg;
    }

    public boolean bfs(int src, List[] parent, int dest) {
        List<Integer> queue = new LinkedList<>();
        for (int i = 0; i < n; i++) {
            parent[i] = new LinkedList<>();
        }
        boolean status[] = new boolean[n];
        for (int i = 0; i < n; i++) {
            status[i] = false;
        }
        status[src] = true;
        queue.add(src);
        parent[src].add(-1);
        while (!queue.isEmpty()) {
            int next = queue.remove(0);
            for (Edge e : g[next]) {
                if (!status[e.dest]) {
                    if (e.w != 0) {
                        status[e.dest] = true;
                        parent[e.dest].add(next);
                        queue.add(e.dest);
                    }
                }
            }
        }
        return (status[dest] == true);
    }

    public int fordFulkersonAlgousingBFS(AdjListGraph Rgraph, int s, int t) {
        int max_flow = 0;
        List[] parent = new LinkedList[n];
        long startTime, endTime, currentTime, totalTime = 0;
        startTime = System.nanoTime();
        while (Rgraph.bfs(s, parent, t)) {
            endTime = System.nanoTime();
            currentTime = (endTime - startTime) / 1000000000;
            int min = Integer.MAX_VALUE;
            int de = 0;
            for (int cuurnode = t; cuurnode != s; cuurnode = de) {
                de = (int) parent[cuurnode].get(0);
                min = Math.min(min, Rgraph.GetCapacity(de, cuurnode));
            }
            for (int cuurnode = t; cuurnode != s; cuurnode = de) {
                de = (int) parent[cuurnode].get(0);
                int index = GetIndex(de, cuurnode);
                Edge e1 = g[de].get(index);
                Edge e2 = Rgraph.g[de].get(index);
                if (e1.w > 0) {
                    updateCapacity(e1, e1.Capacity + min);
                    Rgraph.updateWeight(e2, e2.w - min);
                    Rgraph.updateWeight(e2.dest, de, min);
                } else {
                    updateCapacity(e1, e1.Capacity - min);
                }

            }
            max_flow += min;
            totalTime += currentTime;
            startTime = System.nanoTime();
        }
        JOptionPane.showMessageDialog(null, "Total time to find augmenting path " + totalTime + " s");
        return max_flow;
    }

    public boolean widsetPath(LinkedList[] parent, int src, int dest) {
        boolean[] Status = new boolean[n];
        LinkedList<Integer> s = new LinkedList<>();
        LinkedList<Integer> wT = new LinkedList<>();
        for (int i = 0; i < n; i++) {
            parent[i] = new LinkedList<>();
        }
        for (int i = 0; i < n; i++) {
            Status[i] = false;
            parent[i].add(-1);
        }
        Status[src] = true;
        s.add(src);
        int vertex = 0;
        while (wT.size() < n) {
            int max = Integer.MIN_VALUE;
            int z = 0;
            for (int i : s) {
                for (Edge j : g[i]) {
                    if (!Status[j.dest] && j.w != 0 && max < j.w) {
                        {
                            max = j.w;
                            vertex = i;
                            z = j.dest;
                        }
                    }
                }
            }
            wT.add(max);
            if (!s.contains(z)) {
                s.add(z);
                Status[z] = true;
                parent[z].remove(0);
                parent[z].add(vertex);
            }
        }
        return Status[dest];
    }

    public int fordFulkersonAlgousingWidestPath(AdjListGraph Rgraph, int s, int t) {
        int max_flow = 0;
        LinkedList[] parent = new LinkedList[n];
        long startTime, endTime, currentTime, totalTime = 0;
        startTime = System.nanoTime();
        while (Rgraph.widsetPath(parent, s, t)) {
            endTime = System.nanoTime();
            currentTime = (endTime - startTime) / 1000000000;
            int min = Integer.MAX_VALUE;

            int de = 0;
            for (int cuurnode = t; cuurnode != s; cuurnode = de) {
                de = (int) parent[cuurnode].remove(0);
                parent[cuurnode].add(de);
                min = Math.min(min, Rgraph.GetCapacity(de, cuurnode));
            }

            for (int cuurnode = t; cuurnode != s; cuurnode = de) {
                de = (int) parent[cuurnode].get(0);
                int index = GetIndex(de, cuurnode);
                Edge e1 = g[de].get(index);
                Edge e2 = Rgraph.g[de].get(index);
                if (e1.w > 0) {
                    updateCapacity(e1, e1.Capacity + min);
                    Rgraph.updateWeight(e2, e2.w - min);
                    Rgraph.updateWeight(e2.dest, de, min);
                } else {
                    updateCapacity(e1, e1.Capacity - min);
                }
            }

            max_flow += min;
            totalTime += currentTime;
            startTime = System.nanoTime();
        }
        JOptionPane.showMessageDialog(null, "Total time to find augmenting path " + totalTime + " s");
        return max_flow;
    }

    public String mystr(List[] s) {
        String result = "";
        for (int i = 0; i < s.length; i++) {
            result += i + "-->" + s[i] + "\n";
        }
        return result;
    }

    public String printFlow(int numofv) {
        String result = "";
        for (int i = 0; i < numofv; i++) {
            for (Edge e : g[i]) {
                if (e.w == -1) {
                    for (Edge e1 : g[e.dest]) {
                        if (e1.Capacity > 0) {
                            result += i + "----" + e1.Capacity + "--->" + e1.dest + "\n";
                        }
                    }
                } else if (e.w > 0 && e.Capacity > 0) {
                    result += i + "----" + e.Capacity + "--->" + e.dest + "\n";
                }
            }
        }

        return result;
    }

    public int findMaxKey(int[] k, boolean[] Status) {
        int maxVertex = -7;
        int maxX = Integer.MIN_VALUE;
        for (int i = 0; i < n; i++) {
            if (k[i] > maxX && Status[i] == false) {
                maxX = k[i];
                maxVertex = i;
            }
        }
        return maxVertex;
    }

    boolean findWidestPath(LinkedList[] parent, int src, int dest) {
        boolean Status[] = new boolean[n];
        int X[] = new int[n];
        for (int i = 0; i < n; i++) {
            parent[i] = new LinkedList<>();
        }
        for (int i = 0; i < n; i++) {
            Status[i] = false;
            parent[i].add(-1);
            X[i] = Integer.MIN_VALUE;
        }
        X[src] = 0;
        int v = findMaxKey(X, Status);
        while (v != -7) {
            Status[v] = true;
            for (Edge e : g[v]) {
                if (e.w != 0) {
                    if (Status[e.dest] == false && e.w > X[e.dest]) {
                        X[e.dest] = e.w;
                        parent[e.dest].remove();
                        parent[e.dest].add(v);
                    }
                }
            }
            v = findMaxKey(X, Status);
        }
        return Status[dest];
    }

    public int fordFulkersonWidestPathLargestKey(AdjListGraph Rgraph, int s, int t) {
        int max_flow = 0;
        LinkedList[] parent = new LinkedList[n];
        long startTime, endTime, currentTime, totalTime = 0;
        startTime = System.nanoTime();
        while (Rgraph.findWidestPath(parent, s, t)) {
            endTime = System.nanoTime();
            currentTime = (endTime - startTime) / 1000000000;
            int min = Integer.MAX_VALUE;
            int de = 0;
            for (int cuurnode = t; cuurnode != s; cuurnode = de) {
                de = (int) parent[cuurnode].get(0);
                min = Math.min(min, Rgraph.GetCapacity(de, cuurnode));
            }

            for (int cuurnode = t; cuurnode != s; cuurnode = de) {
                de = (int) parent[cuurnode].get(0);
                int index = GetIndex(de, cuurnode);
                Edge e1 = g[de].get(index);
                Edge e2 = Rgraph.g[de].get(index);
                if (e1.w > 0) {
                    updateCapacity(e1, e1.Capacity + min);
                    Rgraph.updateWeight(e2, e2.w - min);
                    Rgraph.updateWeight(e2.dest, de, min);
                } else {
                    updateCapacity(e1, e1.Capacity - min);
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
