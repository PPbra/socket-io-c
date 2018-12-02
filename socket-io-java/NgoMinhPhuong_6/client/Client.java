import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.util.Scanner;

public class Client {

    public final static int bufferSize = 1024;
    public static Scanner sc = new Scanner(System.in);
    private static Socket socket;

    public static void main(String[] args) {

        String hostAdress;
        int port;

        try {
            System.out.print("Nhap dia chi cua server: ");
            hostAdress = sc.nextLine();

            System.out.print("Nhap port cuar server: ");
            port = sc.nextInt();

            sc.nextLine();

            InetAddress address = InetAddress.getByName(hostAdress);
            socket = new Socket(address, port);

            OutputStream os = socket.getOutputStream();
            InputStream is = socket.getInputStream();
            OutputStreamWriter osw = new OutputStreamWriter(os);
            BufferedWriter bw = new BufferedWriter(osw);

            while (true) {

                System.out.print("Nhap vao ten file can tai: ");
                String fileName = sc.nextLine();

                if ("QUIT".equals(fileName)) {
                    socket.close();
                    break;
                }


                String sendMessage = fileName + "\n";
                bw.write(sendMessage);
                bw.flush();

                DataInputStream dis = new DataInputStream(is);
                long fileSize = dis.readLong();


                if (fileSize == 0) {
                    System.out.println("File not found");
                    continue;
                }

                OutputStream out = new FileOutputStream(fileName);

                byte[] bytes = new byte[bufferSize];
                long count;
                long total = 0;

                while ((count = is.read(bytes)) > 0) {
                    out.write(bytes, 0, (int) count);
                    total += count;
                    //eof
                    if (fileSize == total) {
                        break;
                    }

                }
                out.close();
                System.out.println("Nhan file thanh cong!\n");
            }

        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                socket.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

}
