package project;

import java.io.FileWriter;
import java.io.IOException;
import java.util.LinkedList;
import java.util.List;
import java.util.Random;
import javax.swing.JOptionPane;

public class VertexDisjoinPath {

    AdjMatrixGraph matrixGraph;
    AdjListGraph listGraph;
    int n;

    public VertexDisjoinPath() {
    }

    public VertexDisjoinPath(AdjMatrixGraph g, int src, int dest) {
        n = g.n;
        matrixGraph = new AdjMatrixGraph(2 * n - 2);
        for (int i = 0; i < n; i++) {
            System.arraycopy(g.graph[i], 0, matrixGraph.graph[i], 0, n);
        }
        int j = 0;
        int div = matrixGraph.n - n;
        for (int i = 0; i < matrixGraph.n; i++) {
            if (i != src && i != dest) {
                if (j < div) {
                    for (int k = 0; k < matrixGraph.n; k++) {
                        matrixGraph.graph[n + j][k] = matrixGraph.graph[i][k];
                        matrixGraph.graph[i][k] = 0;
                    }
                    matrixGraph.graph[i][n + j] = -1;
                    j++;
                } else {
                    break;
                }
            }
        }
    }

    public VertexDisjoinPath(AdjListGraph graph, int src, int dest) {
        listGraph = new AdjListGraph(2 * graph.n - 2);
        n = graph.n;
        for (int i = 0; i < graph.n; i++) {
            for (Edge e : graph.g[i]) {
                listGraph.g[i].add(new Edge(e.dest, e.w, e.Capacity));
            }
        }
        int j = 0;
        int div = listGraph.n - graph.n;
        for (int i = 0; i < listGraph.n; i++) {
            if (i != src && i != dest) {
                if (j < div) {
                    for (Edge e : listGraph.g[i]) {
                        listGraph.g[n + j].add(new Edge(e.dest, e.w, e.Capacity));
                        e.w = 0;
                    }
                    listGraph.g[i].add(new Edge(n + j, -1, 0));
                    j++;
                } else {
                    break;
                }
            }
        }
    }

    public int vdp_BFS(AdjListGraph Rgraph, int s, int t) {
        int max_flow = 0;
        int n = Rgraph.n;
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
            if (min == -1) {
                min *= -1;
            }
            for (int cuurnode = t; cuurnode != s; cuurnode = de) {
                de = (int) parent[cuurnode].get(0);
                int index = listGraph.GetIndex(de, cuurnode);
                Edge e1 = listGraph.g[de].get(index);
                Edge e2 = Rgraph.g[de].get(index);
                if (e1.w != -1) {
                    if (e1.w > 0) {
                        listGraph.updateCapacity(e1, e1.Capacity + min);
                        Rgraph.updateWeight(e2, e2.w - min);
                        Rgraph.updateWeight(e2.dest, de, min);
                    } else {
                        listGraph.updateCapacity(e1, e1.Capacity - min);
                    }
                }
            }
            max_flow += min;
            totalTime += currentTime;
            startTime = System.nanoTime();
        }
        JOptionPane.showMessageDialog(null, "Total time to find augmenting path " + totalTime + " s");
        return max_flow;
    }

    public int vdp_WidestPath(AdjListGraph Rgraph, int s, int t) {
        int max_flow = 0;
        LinkedList[] parent = new LinkedList[listGraph.n];
        long startTime, endTime, currentTime, totalTime = 0;
        startTime = System.nanoTime();
        while (Rgraph.widsetPath(parent, s, t)) {
            endTime = System.nanoTime();
            currentTime = (endTime - startTime) / 1000000000;
            int min = Integer.MAX_VALUE;
            int de = 0;
            for (int cuurnode = t; cuurnode != s; cuurnode = de) {
                de = (int) parent[cuurnode].get(0);
                
                min = Math.min(min, Rgraph.GetCapacity(de, cuurnode));
            }
            if (min == -1) {
                min *= -1;
            }
            for (int cuurnode = t; cuurnode != s; cuurnode = de) {
                de = (int) parent[cuurnode].get(0);
                int index = listGraph.GetIndex(de, cuurnode);
                Edge e1 = listGraph.g[de].get(index);
                Edge e2 = Rgraph.g[de].get(index);
                if (e1.w != -1) {
                    if (e1.w > 0) {
                        listGraph.updateCapacity(e1, e1.Capacity + min);
                        Rgraph.updateWeight(e2, e2.w - min);
                        Rgraph.updateWeight(e2.dest, de, min);
                    } else {
                        listGraph.updateCapacity(e1, e1.Capacity - min);
                    }
                }

            }

            max_flow += min;
            totalTime += currentTime;
            startTime = System.nanoTime();
        }
        JOptionPane.showMessageDialog(null, "Total time to find augmenting path " + totalTime + " s");
        return max_flow;
    }

    public int vdp_BFS(AdjMatrixGraph Rg, int s, int t) {
        int max_flow = 0;
        int[] parent = new int[matrixGraph.n];
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
            if (min == -1) {
                min *= -1;
            }

            for (int currNode = t; currNode != s; currNode = parent[currNode]) {
                i = parent[currNode];
                if (matrixGraph.checkEdge(i, currNode)) {
                    if (matrixGraph.graph[i][currNode] != -1) {
                        matrixGraph.flow[i][currNode] += min;
                        Rg.graph[currNode][i] += min;
                        Rg.graph[i][currNode] -= min;
                    } else {
                        matrixGraph.flow[i][currNode] -= min;
                    }
                }
            }
            max_flow += min;
            totalTime += currentTime;
            startTime = System.nanoTime();

        }
        JOptionPane.showMessageDialog(null, "Total time to find augmenting path " + totalTime + " s");
        return max_flow;
    }

    public int vdp_WidestPath(AdjMatrixGraph Rg, int s, int t) {
        int max_flow = 0;
        int[] parent = new int[matrixGraph.n];
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
            if (min == -1) {
                min *= -1;
            }

            for (int currNode = t; currNode != s; currNode = parent[currNode]) {
                i = parent[currNode];
                if (matrixGraph.graph[i][currNode] != -1) {
                    if (matrixGraph.checkEdge(i, currNode)) {
                        {
                            matrixGraph.flow[i][currNode] += min;
                            Rg.graph[currNode][i] += min;
                            Rg.graph[i][currNode] -= min;
                        }
                    } else {
                        matrixGraph.flow[i][currNode] -= min;
                    }
                }
            }
            max_flow += min;
            totalTime += currentTime;
            startTime = System.nanoTime();

        }
        JOptionPane.showMessageDialog(null, "Total time to find augmenting path " + totalTime + " s");
        return max_flow;
    }

    public int vdp_WidestPathLargetKey(AdjMatrixGraph Rg, int s, int t) {
        int max_flow = 0;

        int[] parent = new int[matrixGraph.n];
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
            if (min == -1) {
                min *= -1;
            }

            for (int currNode = t; currNode != s; currNode = parent[currNode]) {
                i = parent[currNode];
                if (matrixGraph.graph[i][currNode] != -1) {
                    if (matrixGraph.checkEdge(i, currNode)) {
                        {
                            matrixGraph.flow[i][currNode] += min;
                            Rg.graph[currNode][i] += min;
                            Rg.graph[i][currNode] -= min;
                        }
                    } else {
                        matrixGraph.flow[i][currNode] -= min;
                    }
                }
            }
            max_flow += min;
            totalTime += currentTime;
            startTime = System.nanoTime();
        }
        JOptionPane.showMessageDialog(null, "Total time to find augmenting path " + totalTime + " s");
        return max_flow;
    }

    public int vdp_WidestPathLargestKey(AdjListGraph Rgraph, int s, int t) {
        int max_flow = 0;
        LinkedList[] parent = new LinkedList[listGraph.n];
        long startTime, endTime, currentTime, totalTime = 0;
        startTime = System.nanoTime();
        while (Rgraph.findWidestPath(parent, s, t)) {
            endTime = System.nanoTime();
            currentTime = (endTime - startTime) / 1000000000;
            int min = Integer.MAX_VALUE;
            int de = 0;
            for (int cuurnode = t; cuurnode != s; cuurnode = de) {
                de = (int) parent[cuurnode].remove(0);
                parent[cuurnode].add(de);
                min = Math.min(min, Rgraph.GetCapacity(de, cuurnode));
            }
            if (min == -1) {
                min *= -1;
            }

            for (int cuurnode = t; cuurnode != s; cuurnode = de) {
                de = (int) parent[cuurnode].get(0);
                int index = listGraph.GetIndex(de, cuurnode);
                Edge e1 = listGraph.g[de].get(index);
                Edge e2 = Rgraph.g[de].get(index);
                if (e1.w != -1) {
                    if (e1.w > 0) {
                        listGraph.updateCapacity(e1, e1.Capacity + min);
                        Rgraph.updateWeight(e2, e2.w - min);
                        Rgraph.updateWeight(e2.dest, de, min);
                    } else {
                        listGraph.updateCapacity(e1, e1.Capacity - min);
                    }
                }

            }

            max_flow += min;
            totalTime += currentTime;
            startTime = System.nanoTime();

        }
        JOptionPane.showMessageDialog(null, "Total time to find augmenting path " + totalTime + " s");
        return max_flow;
    }

    public void printfile(int src, int dest) throws IOException {
        Random r = new Random();
        int filenum = r.nextInt(100);
        int numofv = n;
        String filename1 = "list\\" + "test_" + filenum + "_list_vdp" + ".txt";
        String filename2 = "matrix\\" + "test_" + filenum + "_matrix_vdp" + ".txt";
        Random w = new Random();
        String newLine = System.getProperty("line.separator");
        FileWriter fw1 = new FileWriter(filename1);
        FileWriter fw2 = new FileWriter(filename2);

        String data = numofv + "," + "-1" + newLine;
        fw1.write(data);
        data = numofv + "," + "-2" + newLine;
        fw2.write(data);
        data = src + "," + dest + newLine;
        fw1.write(data);
        fw2.write(data);
        for (int i = 0; i < numofv; i++) {
            for (Edge e : listGraph.g[i]) {
                if (e.w != 0) {
                    data = i + "," + e.dest + "," + e.w + newLine;
                    fw1.write(data);
                    fw2.write(data);
                }

            }
        }
        fw1.close();
        fw2.close();
        System.out.println("File " + filename1 + " written successful");
        System.out.println("File " + filename2 + " written successful");

    }
}
