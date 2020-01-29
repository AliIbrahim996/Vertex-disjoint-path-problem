package project;

import com.mxgraph.layout.hierarchical.mxHierarchicalLayout;
import com.mxgraph.layout.mxIGraphLayout;
import com.mxgraph.swing.mxGraphComponent;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import org.jgrapht.ListenableGraph;
import org.jgrapht.ext.JGraphXAdapter;
import org.jgrapht.graph.DefaultWeightedEdge;
import org.jgrapht.graph.ListenableDirectedWeightedGraph;

public class Draw_Graph {

    public void createAndShowGui() throws IOException {
        JFrame frame = new JFrame("DemoGraph");
        frame.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);

        ListenableGraph<String, MyEdge> g = buildGraph();
        JGraphXAdapter<String, MyEdge> graphAdapter
                = new JGraphXAdapter<>(g);
        mxIGraphLayout layout = new mxHierarchicalLayout(graphAdapter);
        try {
            layout.execute(graphAdapter.getDefaultParent());
        } catch (Exception e) {
            JOptionPane.showMessageDialog(null, e.getMessage() + "\n Number of vertex is large\nCould not draw graph");
            return;
        }
        frame.add(new mxGraphComponent(graphAdapter));
        frame.pack();
        frame.setLocationByPlatform(true);
        frame.setVisible(true);
    }

    public void createAndShowGui(AdjListGraph adjGraph) throws IOException {
        JFrame frame = new JFrame("DemoGraph");
        frame.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);

        ListenableGraph<String, MyEdge> g = buildGraph(adjGraph);
        JGraphXAdapter<String, MyEdge> graphAdapter
                = new JGraphXAdapter<>(g);
        mxIGraphLayout layout = new mxHierarchicalLayout(graphAdapter);
        try {
            layout.execute(graphAdapter.getDefaultParent());
        } catch (Exception e) {
            JOptionPane.showMessageDialog(null, e.getMessage() + "\n Number of vertex is large\nCould not draw graph");
            return;
        }
        frame.add(new mxGraphComponent(graphAdapter));
        frame.pack();
        frame.setLocationByPlatform(true);
        frame.setVisible(true);
    }

        public void createAndShowGui(AdjMatrixGraph adjGraph) throws IOException {
        JFrame frame = new JFrame("DemoGraph");
        frame.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);

        ListenableGraph<String, MyEdge> g = buildGraph(adjGraph);
        JGraphXAdapter<String, MyEdge> graphAdapter
                = new JGraphXAdapter<>(g);
        mxIGraphLayout layout = new mxHierarchicalLayout(graphAdapter);
        try {
            layout.execute(graphAdapter.getDefaultParent());
        } catch (Exception e) {
            JOptionPane.showMessageDialog(null, e.getMessage() + "\n Number of vertex is large\nCould not draw graph");
            return;
        }
        frame.add(new mxGraphComponent(graphAdapter));
        frame.pack();
        frame.setLocationByPlatform(true);
        frame.setVisible(true);
    }
    public static class MyEdge extends DefaultWeightedEdge {

//        @Override
//        public String toString() {
//            return String.valueOf(getWeight());
//        }
    }

    public static ListenableGraph<String, MyEdge> buildGraph() throws FileNotFoundException, IOException {
        ListenableDirectedWeightedGraph<String, MyEdge> g = new ListenableDirectedWeightedGraph<>(MyEdge.class);
        String p = System.getProperty("user.dir");
        JFileChooser jFileChooser1 = new JFileChooser(p);
        if (jFileChooser1.showOpenDialog(null) == JFileChooser.APPROVE_OPTION) {
            File file = jFileChooser1.getSelectedFile();
            FileReader fr = new FileReader(file);
            BufferedReader br = new BufferedReader(fr);
            String s = br.readLine();
            String s1[] = s.split(",");
            int n = Integer.parseInt(s1[0]);
            for (int i = 0; i < n; i++) {
                g.addVertex(Integer.toString(i));

            }
            s = br.readLine();
            while ((s = br.readLine()) != null) {

                s1 = s.split(",");
                String u = s1[0];
                String v = s1[1];
                double w = Integer.parseInt(s1[2]);
                MyEdge e = g.addEdge(u, v);
                g.setEdgeWeight(e, w);
            }
        }
        return g;
    }

    public static ListenableGraph<String, MyEdge> buildGraph(AdjListGraph adGraph) throws FileNotFoundException, IOException {
        ListenableDirectedWeightedGraph<String, MyEdge> g = new ListenableDirectedWeightedGraph<>(MyEdge.class);
        MyEdge e;
        for (int i = 0; i < adGraph.n; i++) {
            g.addVertex(Integer.toString(i));
        }
        for (int i = 0; i < adGraph.n; i++) {
            for (Edge j : adGraph.g[i]) {
                String u = Integer.toString(i);
                String v=Integer.toString(j.dest);
                double w=j.w;
                e = g.addEdge(u, v);
                g.setEdgeWeight(e, w);
            }
        }
        return g;
    }
        public static ListenableGraph<String, MyEdge> buildGraph(AdjMatrixGraph adGraph) throws FileNotFoundException, IOException {
        ListenableDirectedWeightedGraph<String, MyEdge> g = new ListenableDirectedWeightedGraph<>(MyEdge.class);
        MyEdge e;
        for (int i = 0; i < adGraph.n; i++) {
            g.addVertex(Integer.toString(i));
        }
        for (int i = 0; i < adGraph.n; i++) {
            for (int j=0;j<adGraph.n;j++) {
                String u = Integer.toString(i);
                String v=Integer.toString(j);
                if(adGraph.graph[i][j]!=0){
                double w=adGraph.graph[i][j];
                e = g.addEdge(u, v);
                g.setEdgeWeight(e, w);
                }
            }
        }
        return g;
    }
}
