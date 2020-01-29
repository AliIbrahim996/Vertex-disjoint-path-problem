package project;

import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JOptionPane;

/**
 *
 * @author Ali_ibrahim
 */
public class Generate_Graph extends javax.swing.JFrame {

    String data;
    VertexDisjoinPath vdpMatrix;
    VertexDisjoinPath vdpList;

    public Generate_Graph() {
        initComponents();
        //this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        this.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
        addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent we) {
                String ObjButtons[] = {"Yes", "No"};
                int PromptResult = JOptionPane.showOptionDialog(null, "Are you sure you want to exit?", "Max-Flow project", JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE, null, ObjButtons, ObjButtons[1]);
                if (PromptResult == JOptionPane.YES_OPTION) {
                    System.exit(0);
                }
            }
        });

        this.setTitle("Max flow project");

    }

    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jPanel1 = new javax.swing.JPanel();
        jmethod1 = new javax.swing.JComboBox<>();
        jLabel5 = new javax.swing.JLabel();
        jPanel2 = new javax.swing.JPanel();
        jText1 = new javax.swing.JTextField();
        jLabel1 = new javax.swing.JLabel();
        jLabel2 = new javax.swing.JLabel();
        jText2 = new javax.swing.JTextField();
        jLabel3 = new javax.swing.JLabel();
        jText3 = new javax.swing.JTextField();
        jLabel4 = new javax.swing.JLabel();
        jText4 = new javax.swing.JTextField();
        jButton1 = new javax.swing.JButton();
        jScrollPane1 = new javax.swing.JScrollPane();
        jArea1 = new javax.swing.JTextArea();
        jButton5 = new javax.swing.JButton();
        jButton3 = new javax.swing.JButton();
        jButton4 = new javax.swing.JButton();
        jRadioButton1 = new javax.swing.JRadioButton();
        jRadioButton2 = new javax.swing.JRadioButton();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);

        jPanel1.setBackground(new java.awt.Color(204, 204, 204));
        jPanel1.setBorder(javax.swing.BorderFactory.createTitledBorder("Generate graph"));

        jmethod1.setFont(new java.awt.Font("Tempus Sans ITC", 1, 12)); // NOI18N
        jmethod1.setModel(new javax.swing.DefaultComboBoxModel<>(new String[] { "BFS", "WidestPath", "WidestPathWithLargestKey" }));

        jLabel5.setFont(new java.awt.Font("Tempus Sans ITC", 1, 12)); // NOI18N
        jLabel5.setText("path method");

        jPanel2.setBackground(new java.awt.Color(204, 204, 255));
        jPanel2.setBorder(javax.swing.BorderFactory.createEtchedBorder());

        jText1.setFont(new java.awt.Font("Tempus Sans ITC", 1, 12)); // NOI18N

        jLabel1.setFont(new java.awt.Font("Tempus Sans ITC", 1, 12)); // NOI18N
        jLabel1.setText("NumofVertices");

        jLabel2.setFont(new java.awt.Font("Tempus Sans ITC", 1, 12)); // NOI18N
        jLabel2.setText("NumofEdges");

        jText2.setFont(new java.awt.Font("Tempus Sans ITC", 1, 12)); // NOI18N

        jLabel3.setFont(new java.awt.Font("Tempus Sans ITC", 1, 12)); // NOI18N
        jLabel3.setText("Source");

        jText3.setFont(new java.awt.Font("Tempus Sans ITC", 1, 12)); // NOI18N

        jLabel4.setFont(new java.awt.Font("Tempus Sans ITC", 1, 12)); // NOI18N
        jLabel4.setText("Sink");

        jText4.setFont(new java.awt.Font("Tempus Sans ITC", 1, 12)); // NOI18N

        jButton1.setBackground(new java.awt.Color(255, 255, 255));
        jButton1.setFont(new java.awt.Font("Tempus Sans ITC", 1, 12)); // NOI18N
        jButton1.setText("Generate Graph");
        jButton1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton1ActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
        jPanel2.setLayout(jPanel2Layout);
        jPanel2Layout.setHorizontalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel2Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel2Layout.createSequentialGroup()
                        .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jLabel1)
                            .addComponent(jLabel2)
                            .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                                .addComponent(jLabel4, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                                .addComponent(jLabel3, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)))
                        .addGap(18, 18, 18)
                        .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jText1, javax.swing.GroupLayout.DEFAULT_SIZE, 83, Short.MAX_VALUE)
                            .addComponent(jText2)
                            .addComponent(jText3)
                            .addComponent(jText4))
                        .addGap(77, 77, 77))
                    .addGroup(jPanel2Layout.createSequentialGroup()
                        .addComponent(jButton1)
                        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))))
        );
        jPanel2Layout.setVerticalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel2Layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jText1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel1))
                .addGap(18, 18, 18)
                .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jText2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel2))
                .addGap(18, 18, 18)
                .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jText3, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel3))
                .addGap(22, 22, 22)
                .addGroup(jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(jText4, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel4))
                .addGap(18, 18, 18)
                .addComponent(jButton1)
                .addContainerGap(29, Short.MAX_VALUE))
        );

        jArea1.setColumns(20);
        jArea1.setFont(new java.awt.Font("Tempus Sans ITC", 1, 12)); // NOI18N
        jArea1.setRows(5);
        jArea1.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        jScrollPane1.setViewportView(jArea1);

        jButton5.setBackground(new java.awt.Color(255, 255, 255));
        jButton5.setFont(new java.awt.Font("Tempus Sans ITC", 1, 12)); // NOI18N
        jButton5.setText("Back");
        jButton5.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton5ActionPerformed(evt);
            }
        });

        jButton3.setBackground(new java.awt.Color(255, 255, 255));
        jButton3.setFont(new java.awt.Font("Tempus Sans ITC", 1, 12)); // NOI18N
        jButton3.setText("Draw Graph");
        jButton3.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton3ActionPerformed(evt);
            }
        });

        jButton4.setBackground(new java.awt.Color(255, 255, 255));
        jButton4.setFont(new java.awt.Font("Tempus Sans ITC", 1, 12)); // NOI18N
        jButton4.setText("clear");
        jButton4.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton4ActionPerformed(evt);
            }
        });

        jRadioButton1.setText("Solve vertex disjoint problem");
        jRadioButton1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jRadioButton1ActionPerformed(evt);
            }
        });

        jRadioButton2.setText("Test on normal network");
        jRadioButton2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jRadioButton2ActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel1Layout.createSequentialGroup()
                        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addGroup(jPanel1Layout.createSequentialGroup()
                                .addGap(21, 21, 21)
                                .addComponent(jPanel2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                                .addGap(35, 35, 35))
                            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel1Layout.createSequentialGroup()
                                    .addComponent(jLabel5)
                                    .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                                    .addComponent(jmethod1, javax.swing.GroupLayout.PREFERRED_SIZE, 209, javax.swing.GroupLayout.PREFERRED_SIZE))
                                .addGroup(jPanel1Layout.createSequentialGroup()
                                    .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                        .addGroup(jPanel1Layout.createSequentialGroup()
                                            .addGap(2, 2, 2)
                                            .addComponent(jRadioButton1, javax.swing.GroupLayout.PREFERRED_SIZE, 242, javax.swing.GroupLayout.PREFERRED_SIZE))
                                        .addComponent(jButton4, javax.swing.GroupLayout.PREFERRED_SIZE, 89, javax.swing.GroupLayout.PREFERRED_SIZE)
                                        .addComponent(jButton5, javax.swing.GroupLayout.PREFERRED_SIZE, 91, javax.swing.GroupLayout.PREFERRED_SIZE))
                                    .addGap(0, 0, Short.MAX_VALUE))))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 68, Short.MAX_VALUE))
                    .addGroup(jPanel1Layout.createSequentialGroup()
                        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jRadioButton2, javax.swing.GroupLayout.PREFERRED_SIZE, 244, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(jButton3))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)))
                .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, 397, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel1Layout.createSequentialGroup()
                        .addGap(11, 11, 11)
                        .addComponent(jPanel2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(jmethod1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(jLabel5, javax.swing.GroupLayout.PREFERRED_SIZE, 17, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(jRadioButton1)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(jRadioButton2)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(jButton3)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jButton4)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jButton5))
                    .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, 426, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(49, 49, 49))
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jPanel1, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void jButton5ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton5ActionPerformed

        this.setVisible(false);
        Welcom w = new Welcom();
        w.setVisible(true);
    }//GEN-LAST:event_jButton5ActionPerformed

    private void jButton3ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton3ActionPerformed

        Draw_Graph dr = new Draw_Graph();
        try {
            dr.createAndShowGui();
        } catch (IOException ex) {
            JOptionPane.showMessageDialog(this, ex.getMessage());
        }
    }//GEN-LAST:event_jButton3ActionPerformed

    private void jButton4ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton4ActionPerformed

        jText1.setText("");
        jText2.setText("");
        jText3.setText("");
        jText4.setText("");
        jArea1.setText("");
    }//GEN-LAST:event_jButton4ActionPerformed

    private void jButton1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed
        // TODO add your handling code here
        String s1 = jText1.getText();
        String s2 = jText2.getText();
        int numofv = Integer.parseInt(s1);
        int numofe = Integer.parseInt(s2);
        RandomGraph rg = new RandomGraph(numofv, numofe);
        s1 = jText3.getText();
        s2 = jText4.getText();
        int src = Integer.parseInt(s1);
        int dest = Integer.parseInt(s2);
        try {
            String d = rg.generateRandomGraph(numofv, numofe, src, dest);
            jArea1.setText("Graph generated\n" + d);
        } catch (IOException ex) {
            JOptionPane.showMessageDialog(null, ex.getMessage());
        }
    }//GEN-LAST:event_jButton1ActionPerformed

    private void jRadioButton1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jRadioButton1ActionPerformed
        // TODO add your handling code here:
        if (jRadioButton2.isSelected()) {
            jRadioButton2.setSelected(false);
        }
        String p = System.getProperty("user.dir");
        JFileChooser jFileChooser1 = new JFileChooser(p);
        data = "";
        if (jFileChooser1.showOpenDialog(null) == JFileChooser.APPROVE_OPTION) {
            File file = jFileChooser1.getSelectedFile();
            data += "Reading File " + file.getAbsolutePath() + "\n------------------\n";
            jArea1.setText(data);
            try {
                FileReader fr = new FileReader(file);
                BufferedReader br = new BufferedReader(fr);
                String s = br.readLine();
                String s1[] = s.split(",");
                int n = Integer.parseInt(s1[0]);
                String da = "";
                da = n + "";
                data += "creating graph " + "\n------------------\n";
                jArea1.setText(data);
                jText1.setText(da);
                if (s1[1].equals("1")) {
                    AdjListGraph graph = new AdjListGraph(n);
                    s = br.readLine();
                    s1 = s.split(",");
                    int source = Integer.parseInt(s1[0]);
                    da = source + "";
                    jText3.setText(da);
                    int destination = Integer.parseInt(s1[1]);
                    da = destination + "";
                    jText4.setText(da);
                    int numofe = 0;
                    while ((s = br.readLine()) != null) {

                        s1 = s.split(",");
                        int u = Integer.parseInt(s1[0]);
                        int v = Integer.parseInt(s1[1]);
                        int w = Integer.parseInt(s1[2]);
                        graph.addEdge(u, v, w);
                        numofe++;
                    }
                    da = numofe + "";
                    jText2.setText(da);
                    vdpList = new VertexDisjoinPath(graph, source, destination);
                    AdjListGraph Rgraph = vdpList.listGraph.creatResidualGraph();
                    if (jmethod1.getSelectedItem().equals("BFS")) {
                        data += "Running ford-fulkerson Algorithm " + "\n------------------\n";
                        jArea1.setText(data);
                        long startTime = System.nanoTime();
                        int max_flow = vdpList.vdp_BFS(Rgraph, source, destination);
                        long endTime = System.nanoTime();
                        double totalTime = ((endTime - startTime) / 1000000000.00);
                        System.out.println("The maximum num of disjoint paths is" + max_flow);
                        JOptionPane.showMessageDialog(this, "Time taken to run ford-fulkerson " + totalTime);
                        data += vdpList.listGraph.printFlow(n);
                        data += "The maximum num of disjont paths is " + max_flow + "\n------------------\n";
                        jArea1.setText(data);
                        JOptionPane.showMessageDialog(this, "The maximum fnum of disjoint paths is " + max_flow);
                    } else if (jmethod1.getSelectedItem().equals("WidestPath")) {
                        data += "Running ford-fulkerson Algorithm " + "\n------------------\n";
                        jArea1.setText(data);
                        long startTime = System.nanoTime();
                        int max_flow = vdpList.vdp_WidestPath(Rgraph, source, destination);
                        long endTime = System.nanoTime();
                        double totalTime = (endTime - startTime) / 1000000000.00;
                        data += vdpList.listGraph.printFlow(n);
                        data += "The maximum num of disjont paths is " + max_flow + "\n------------------\n";
                        jArea1.setText(data);
                        JOptionPane.showMessageDialog(this, "Time taken to run ford-fulkerson " + totalTime);
                        JOptionPane.showMessageDialog(this, "The maximum num of disjoint paths is " + max_flow);

                    } else if (jmethod1.getSelectedItem().equals("WidestPathWithLargestKey")) {
                        data += "Running ford-fulkerson Algorithm " + "\n------------------\n";
                        jArea1.setText(data);
                        long startTime = System.nanoTime();
                        int max_flow = vdpList.vdp_WidestPathLargestKey(Rgraph, source, destination);
                        long endTime = System.nanoTime();
                        double totalTime = (endTime - startTime) / 1000000000.00;
                        JOptionPane.showMessageDialog(this, "Time taken to run ford-fulkerson " + totalTime);
                        data += vdpList.listGraph.printFlow(n);
                        data += "The maximum num of disjont paths is " + max_flow + "\n------------------\n";
                        jArea1.setText(data);
                        JOptionPane.showMessageDialog(this, "The maximum num of disjoint paths is " + max_flow);
                    }
                } else if (s1[1].equals("2")) {
                    AdjMatrixGraph g = new AdjMatrixGraph(n);
                    s = br.readLine();
                    s1 = s.split(",");
                    int source = Integer.parseInt(s1[0]);
                    da = source + "";
                    jText3.setText(da);
                    int destination = Integer.parseInt(s1[1]);
                    da = destination + "";
                    jText4.setText(da);
                    int numofe = 0;
                    while ((s = br.readLine()) != null) {
                        s1 = s.split(",");
                        int u = Integer.parseInt(s1[0]);
                        int v = Integer.parseInt(s1[1]);
                        int w = Integer.parseInt(s1[2]);
                        g.addEdge(u, v, w);
                        numofe++;
                    }
                    da = numofe + "";
                    jText2.setText(da);
                    vdpMatrix = new VertexDisjoinPath(g, source, destination);
                    AdjMatrixGraph Rg = vdpMatrix.matrixGraph.createResduialGraph();
                    if (jmethod1.getSelectedItem().equals("BFS")) {
                        data += "Running ford-fulkerson Algorithm " + "\n------------------\n";
                        jArea1.setText(data);
                        long startTime = System.nanoTime();
                        int max1 = vdpMatrix.vdp_BFS(Rg, source, destination);
                        long endTime = System.nanoTime();
                        double totalTime = (endTime - startTime) / 1000000000.00;
                        JOptionPane.showMessageDialog(this, "Time taken to run ford-fulkerson " + totalTime);
                        int numofv = Integer.parseInt(jText1.getText());
                        data += vdpMatrix.matrixGraph.printFlow(numofv);
                        data += System.getProperty("line.separator") + "The maximum num of disjoint paths is " + max1;
                        jArea1.setText(data);
                        JOptionPane.showMessageDialog(this, "The maximum num of disjoint paths is " + max1);
                    } else if (jmethod1.getSelectedItem().equals("WidestPath")) {
                        data += "Running ford-fulkerson Algorithm " + "\n------------------\n";
                        jArea1.setText(data);
                        long startTime = System.nanoTime();
                        int max1 = vdpMatrix.vdp_WidestPath(Rg, source, destination);
                        long endTime = System.nanoTime();
                        double totalTime = (endTime - startTime) / 1000000000.00;
                        JOptionPane.showMessageDialog(this, "Time taken to run ford-fulkerson " + totalTime);
                        int numofv = Integer.parseInt(jText1.getText());
                        data += vdpMatrix.matrixGraph.printFlow(numofv);
                        data += System.getProperty("line.separator") + "The maximum num of disjoint paths is " + max1;
                        jArea1.setText(data);
                        JOptionPane.showMessageDialog(this, "The maximum num of disjoint paths is " + max1);
                    } else if (jmethod1.getSelectedItem().equals("WidestPathWithLargestKey")) {
                        data += "Running ford-fulkerson Algorithm " + "\n------------------\n";
                        jArea1.setText(data);
                        long startTime = System.nanoTime();
                        int max_flow = vdpMatrix.vdp_WidestPathLargetKey(Rg, source, destination);
                        long endTime = System.nanoTime();
                        double totalTime = (endTime - startTime) / 1000000000.00;
                        JOptionPane.showMessageDialog(this, "Time taken to run ford-fulkerson " + totalTime);
                        int numofv = Integer.parseInt(jText1.getText());
                        data += vdpMatrix.matrixGraph.printFlow(numofv);
                        data += "The maximum num of disjoint paths is " + max_flow + "\n------------------\n";
                        jArea1.setText(data);
                        JOptionPane.showMessageDialog(this, "The maximum num of disjoint paths is " + max_flow);
                    }
                }
            } catch (IOException e) {
                JOptionPane.showMessageDialog(this, e.getMessage());
            }
            jRadioButton1.setSelected(false);
        }
    }//GEN-LAST:event_jRadioButton1ActionPerformed

    private void jRadioButton2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jRadioButton2ActionPerformed
        // TODO add your handling code here:
        if (jRadioButton1.isSelected()) {
            jRadioButton1.setSelected(false);
        }
        String p = System.getProperty("user.dir");
        JFileChooser jFileChooser1 = new JFileChooser(p);
        data = "";
        if (jFileChooser1.showOpenDialog(null) == JFileChooser.APPROVE_OPTION) {
            File file = jFileChooser1.getSelectedFile();
            data += "Reading File " + file.getAbsolutePath() + "\n------------------\n";
            jArea1.setText(data);
            try {
                FileReader fr = new FileReader(file);
                BufferedReader br = new BufferedReader(fr);
                String s = br.readLine();
                String s1[] = s.split(",");
                int n = Integer.parseInt(s1[0]);
                String da = "";
                da = n + "";
                data += "creating graph " + "\n------------------\n";
                jArea1.setText(data);
                jText1.setText(da);
                if (s1[1].equals("1")) {
                    AdjListGraph graph = new AdjListGraph(n);
                    s = br.readLine();
                    s1 = s.split(",");
                    int source = Integer.parseInt(s1[0]);
                    da = source + "";
                    jText3.setText(da);
                    int destination = Integer.parseInt(s1[1]);
                    da = destination + "";
                    jText4.setText(da);
                    int numofe = 0;
                    while ((s = br.readLine()) != null) {

                        s1 = s.split(",");
                        int u = Integer.parseInt(s1[0]);
                        int v = Integer.parseInt(s1[1]);
                        int w = Integer.parseInt(s1[2]);
                        graph.addEdge(u, v, w);
                        numofe++;
                    }
                    da = numofe + "";
                    jText2.setText(da);
                    AdjListGraph Rgraph = graph.creatResidualGraph();
                    if (jmethod1.getSelectedItem().equals("BFS")) {
                        data += "Running ford-fulkerson Algorithm " + "\n------------------\n";
                        jArea1.setText(data);
                        long startTime = System.nanoTime();
                        int max_flow = graph.fordFulkersonAlgousingBFS(Rgraph, source, destination);
                        long endTime = System.nanoTime();
                        double totalTime = ((endTime - startTime) / 1000000000.00);
                        System.out.println("The maximum flow is" + max_flow);
                        JOptionPane.showMessageDialog(this, "Time taken to run ford-fulkerson " + totalTime);
                        data += graph.printFlow(n);
                        data += "The maximum flow is " + max_flow + "\n------------------\n";
                        jArea1.setText(data);
                        JOptionPane.showMessageDialog(this, "The maximum flow is " + max_flow);
                    } else if (jmethod1.getSelectedItem().equals("WidestPath")) {
                        data += "Running ford-fulkerson Algorithm " + "\n------------------\n";
                        jArea1.setText(data);
                        long startTime = System.nanoTime();
                        int max_flow = graph.fordFulkersonAlgousingWidestPath(Rgraph, source, destination);
                        long endTime = System.nanoTime();
                        double totalTime = (endTime - startTime) / 1000000000.00;
                        data += graph.printFlow(n);
                        data += "The maximum flow is " + max_flow + "\n------------------\n";
                        jArea1.setText(data);
                        JOptionPane.showMessageDialog(this, "Time taken to run ford-fulkerson " + totalTime);
                        JOptionPane.showMessageDialog(this, "The maximum flow is " + max_flow);

                    } else if (jmethod1.getSelectedItem().equals("WidestPathWithLargestKey")) {
                        data += "Running ford-fulkerson Algorithm " + "\n------------------\n";
                        jArea1.setText(data);
                        long startTime = System.nanoTime();
                        int max_flow = graph.fordFulkersonWidestPathLargestKey(Rgraph, source, destination);
                        long endTime = System.nanoTime();
                        double totalTime = (endTime - startTime) / 1000000000.00;
                        JOptionPane.showMessageDialog(this, "Time taken to run ford-fulkerson " + totalTime);
                        data += graph.printFlow(n);
                        data += "The maximum flow is " + max_flow + "\n------------------\n";
                        jArea1.setText(data);
                        JOptionPane.showMessageDialog(this, "The maximum flow is " + max_flow);
                    }
                } else if (s1[1].equals("2")) {
                    AdjMatrixGraph g = new AdjMatrixGraph(n);
                    s = br.readLine();
                    s1 = s.split(",");
                    int source = Integer.parseInt(s1[0]);
                    da = source + "";
                    jText3.setText(da);
                    int destination = Integer.parseInt(s1[1]);
                    da = destination + "";
                    jText4.setText(da);
                    int numofe = 0;
                    while ((s = br.readLine()) != null) {
                        s1 = s.split(",");
                        int u = Integer.parseInt(s1[0]);
                        int v = Integer.parseInt(s1[1]);
                        int w = Integer.parseInt(s1[2]);
                        g.addEdge(u, v, w);
                        numofe++;
                    }
                    da = numofe + "";
                    jText2.setText(da);
                    AdjMatrixGraph Rg = g.createResduialGraph();
                    if (jmethod1.getSelectedItem().equals("BFS")) {
                        data += "Running ford-fulkerson Algorithm " + "\n------------------\n";
                        jArea1.setText(data);
                        long startTime = System.nanoTime();
                        int max1 = g.fordFolkersonAlgoUsingBFS(Rg, source, destination);
                        long endTime = System.nanoTime();
                        double totalTime = (endTime - startTime) / 1000000000.00;
                        JOptionPane.showMessageDialog(this, "Time taken to run ford-fulkerson " + totalTime);
                        int numofv = Integer.parseInt(jText1.getText());
                        data += g.printFlow(numofv);
                        data += System.getProperty("line.separator") + "The maximum flow is " + max1;
                        jArea1.setText(data);
                        JOptionPane.showMessageDialog(this, "The maximum flow is " + max1);
                    } else if (jmethod1.getSelectedItem().equals("WidestPath")) {
                        data += "Running ford-fulkerson Algorithm " + "\n------------------\n";
                        jArea1.setText(data);
                        long startTime = System.nanoTime();
                        int max1 = g.fordFolkersonAlgoUsingWidestPath(Rg, source, destination);
                        long endTime = System.nanoTime();
                        double totalTime = (endTime - startTime) / 1000000000.00;
                        JOptionPane.showMessageDialog(this, "Time taken to run ford-fulkerson " + totalTime);
                        int numofv = Integer.parseInt(jText1.getText());
                        data += g.printFlow(numofv);
                        data += System.getProperty("line.separator") + "The maximum flow is " + max1;
                        jArea1.setText(data);
                        JOptionPane.showMessageDialog(this, "The maximum flow is " + max1);
                    } else if (jmethod1.getSelectedItem().equals("WidestPathWithLargestKey")) {
                        data += "Running ford-fulkerson Algorithm " + "\n------------------\n";
                        jArea1.setText(data);
                        long startTime = System.nanoTime();
                        int max_flow = g.fordFulkersonWidestPathLargestKey(Rg, source, destination);
                        long endTime = System.nanoTime();
                        double totalTime = (endTime - startTime) / 1000000000.00;
                        JOptionPane.showMessageDialog(this, "Time taken to run ford-fulkerson " + totalTime);
                        int numofv = Integer.parseInt(jText1.getText());
                        data += g.printFlow(numofv);
                        data += "The maximum flow is " + max_flow + "\n------------------\n";
                        jArea1.setText(data);
                        JOptionPane.showMessageDialog(this, "The maximum flow is " + max_flow);
                    }
                }
            } catch (IOException e) {
                JOptionPane.showMessageDialog(this, e.getMessage());
            }
            jRadioButton2.setSelected(false);
        }
    }//GEN-LAST:event_jRadioButton2ActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JTextArea jArea1;
    private javax.swing.JButton jButton1;
    private javax.swing.JButton jButton3;
    private javax.swing.JButton jButton4;
    private javax.swing.JButton jButton5;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabel4;
    private javax.swing.JLabel jLabel5;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JRadioButton jRadioButton1;
    private javax.swing.JRadioButton jRadioButton2;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JTextField jText1;
    private javax.swing.JTextField jText2;
    private javax.swing.JTextField jText3;
    private javax.swing.JTextField jText4;
    private javax.swing.JComboBox<String> jmethod1;
    // End of variables declaration//GEN-END:variables
}
