package project;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.LinkedList;
import java.util.Random;
/**
 *
 * @author Ali_ibrahim
 */
public class RandomGraph {

    LinkedList<Integer>[] edge;

    public RandomGraph(int numofv, int numofe) {
        edge = new LinkedList[numofv];
        for (int i = 0; i < numofv; i++) {
            edge[i] = new LinkedList<>();
        }
    }

    public String generateRandomGraph(int numofv, int numofe, int src, int dest) throws IOException {
        Random index = new Random();
        int i = 0;
        while (i < numofe) {
            int source = index.nextInt(numofv);
            int destination = index.nextInt(numofv);
            if (source == destination) {
                i++;
            } else if (edge[source].contains(destination)) {
                 System.out.println("no adding");
            } else if (edge[destination].contains(source)) {
                 System.out.println("no adding");
            } else {
                edge[source].add(destination);
                System.out.println("Edge added");
            }
            i++;
        }
        String data = printfile(edge, src, dest, numofv,i);
        return data;
    }

    private String printfile(LinkedList<Integer>[] edge, int src, int dest, int numofv,int numofe) throws IOException {
        Random r = new Random();
        //int filenum = r.nextInt(100);
        String d = "";
        String filename1 = "list\\" + "test_" + numofv + "_list"+numofe/numofv + ".txt";
        String filename2 = "matrix\\" + "test_" + numofv + "_matrix"+numofe/numofv + ".txt";
        Random w = new Random();
        File f1 = new File(filename1);
        File f2 = new File(filename2);
        if (f1.exists()) {
            System.out.println("File is alreade created Updating it");

            String newLine = System.getProperty("line.separator");
            FileWriter fw1 = new FileWriter(f1);
            FileWriter fw2 = new FileWriter(f2);
            String data = numofv + "," + "1" + newLine;
            fw1.write(data);
            data = numofv + "," + "2" + newLine;
            fw2.write(data);
            data = src + "," + dest + newLine;
            fw1.write(data);
            fw2.write(data);
            for (int i = 0; i < numofv; i++) {
                for (Integer e : edge[i]) {
                    boolean b = true;
                    while (b) {
                        int weight = w.nextInt(250);
                        if (weight > 0) {
                            data = i + "," + e + "," + weight + newLine;
                            fw1.write(data);
                            fw2.write(data);
                            b = false;
                        }
                    }
                }
            }
            fw1.close();
            fw2.close();
            d = "File " + filename1 + " written successful\n" + "File " + filename2 + " written successful";
            System.out.println("File " + filename1 + " written successful");
            System.out.println("File " + filename2 + " written successful");

        } else {
            String newLine = System.getProperty("line.separator");
            FileWriter fw1 = new FileWriter(f1);
            FileWriter fw2 = new FileWriter(f2);
            String data = numofv + "," + "1" + newLine;
            fw1.write(data);
            data = numofv + "," + "2" + newLine;
            fw2.write(data);
            data = src + "," + dest + newLine;
            fw1.write(data);
            fw2.write(data);
            for (int i = 0; i < numofv; i++) {
                for (Integer e : edge[i]) {
                    boolean b = true;
                    while (b) {
                        int weight = w.nextInt(250);
                        if (weight > 0) {
                            data = i + "," + e + "," + weight + newLine;
                            fw1.write(data);
                            fw2.write(data);
                            b = false;
                        }
                    }
                }
            }
            fw1.close();
            fw2.close();
            d = "File " + filename1 + " written successful\n" + "File " + filename2 + " written successful";
            System.out.println("File " + filename1 + " written successful");
            System.out.println("File " + filename2 + " written successful");
        }
        return d;
    }
}
