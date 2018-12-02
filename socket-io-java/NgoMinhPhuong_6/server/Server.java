import java.util.Scanner;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;

public class Server {

    public final static int bufferSize = 1024;
    public static int serverPort = 3001;
    public static Scanner sc = new Scanner(System.in);
    public static int nFiles = 0;

    public static void main(String[] args) {
        try {

            System.out.print("Nhap port cuar server: ");
            Server.serverPort = sc.nextInt();
            sc.nextLine();

            ServerSocket serverSocket = new ServerSocket(serverPort);

            while (true) {
                Socket socket = serverSocket.accept();
                
                Thread multipleThread = new HandleThread(socket);

                System.out.println("Thong tin Client: " + socket.getInetAddress() + ":" + socket.getPort());
                multipleThread.start();
            }
        } catch (Exception e) {
            System.out.println(e);
        }
    }

}

class HandleThread extends Thread {

    public final int bufferSize = 1024;
    private Socket socket;

    public HandleThread(Socket socket) {
        this.socket = socket;
    }

    public synchronized void getFilesInfo() throws Exception {
        System.out.println("Tong file da gui la: " + ++Server.nFiles);
    }

    public void run() {

        InputStream  inputStr= null;

        try {
             inputStr= this.socket.getInputStream();
        } catch (IOException ex) {
            System.out.println(ex);
        }
        
        InputStreamReader inputStrReader = new InputStreamReader(inputStr);
        BufferedReader bufferReader= new BufferedReader(inputStrReader);

        while (true) {
            String fileName = null;
            OutputStream outStr = null;
            try {
                fileName = bufferReader.readLine();
                outStr = socket.getOutputStream();
            } catch (IOException ex) {
                ex.printStackTrace();
            }

            if (fileName == null) {
                break;
            }
            System.out.println("File name: " + fileName);
            DataOutputStream dataOutStr= new DataOutputStream(outStr);
            long fileSize = 0;

            try {

                File file = null;
                file = new File(fileName);

                // Lay thong tin do lon cua file


                fileSize = file.length();
                dataOutStr.writeLong((long) fileSize);

                //kiem tra file 
                
                if (fileSize <= 0) {
                    System.out.println("File not found");
                    continue;
                }

                FileInputStream fileInStr = new FileInputStream(file);

                byte fileStrContent[] = new byte[bufferSize];


                //gui file
                int count;
                while ((count = fileInStr.read(fileStrContent)) > 0) {
                    outStr.write(fileStrContent, 0, count);
                }

                //clear 
                outStr.flush();
                fileInStr.close();

                getFilesInfo();
            } catch (Exception e) {
                System.out.println(e);
            }
        }

        try {
            socket.close();
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }

}
