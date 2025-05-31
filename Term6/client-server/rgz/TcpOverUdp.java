import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;

public class TcpOverUdp implements /*Auto*/ Closeable {
  private static final int MAX_PACKET_SIZE = 65507;
  private static final int SOCKET_TIMEOUT = 3000;

  private DatagramSocket socket;
  private byte expectedSeq, sendSeq;
  private byte[] packetBuffer;

  private byte[] receiveBuffer;
  private int availableBytes = 0;
  private int availableBytesOffset = 0;

  // TODO arraycopy
  @Override
  public void close() {
    socket.close();
  }

  public TcpOverUdp() throws SocketException { this(new DatagramSocket(), (byte)1); }

  private TcpOverUdp(DatagramSocket socket, byte expectSeq) {
    this.socket = socket;
    sendSeq = 1;
    expectedSeq = expectSeq;
    packetBuffer = new byte[MAX_PACKET_SIZE];
    receiveBuffer = new byte[MAX_PACKET_SIZE];
  }

  public void connect(InetAddress server, int port) throws IOException {
    if (socket.isConnected())
      throw new IOException();
    socket.connect(server, port);
    send("connect".getBytes(StandardCharsets.UTF_8));
  }

  public void send(byte[] payload) throws IOException {
    send(payload, payload.length);
  }
  public void send(byte[] payload, int length) throws IOException {
    int toSend = length;
    while (toSend > 0) {
      int len = 1;
      packetBuffer[0] = sendSeq;
      while (len < packetBuffer.length && toSend > 0) {
        packetBuffer[len] = payload[len - 1];
        len++;
        toSend--;
      }
      for (int i = 0; i < 4; i++) {
        if (i == 3)
          throw new IOException();
        DatagramPacket packet = new DatagramPacket(
            packetBuffer, len, socket.getRemoteSocketAddress());
        socket.send(packet);
        byte[] ackBuffer = new byte[4];
        DatagramPacket ackPacket =
            new DatagramPacket(ackBuffer, ackBuffer.length);
        try {
          socket.setSoTimeout(SOCKET_TIMEOUT);
          socket.receive(ackPacket);
          int recvLen = ackPacket.getLength();
          if (recvLen == 4 &&
              "ack".compareTo(
                  new String(ackBuffer, 1, 3, StandardCharsets.UTF_8)) == 0 &&
              ackBuffer[0] == sendSeq)
            break;

        } catch (SocketTimeoutException e) {}
      }
      sendSeq++;
    }
  }

  public void receive(byte[] buffer, int len) throws IOException {
    receive(buffer, len, 0);
  }
  public void receive(byte[] buffer, int len, int timeout) throws IOException {
    while (len > 0) {
      if (availableBytes > 0) {
        if (availableBytes >= len) {
          for (int i = 0; i < len; i++) {
            buffer[i] = receiveBuffer[availableBytesOffset++];
            availableBytes--;
          }
          return;
        } else {
          for (int i = 0; i < availableBytes; i++) {
            buffer[i] = receiveBuffer[availableBytesOffset++];
            len--;
          }
          availableBytes = 0;
        }
      }

      while (true) {
        DatagramPacket packet =
            new DatagramPacket(receiveBuffer, receiveBuffer.length);
        socket.setSoTimeout(timeout);
        socket.receive(packet);
        byte seq = receiveBuffer[0];
        byte[] ackBuffer = new byte[4];
        ackBuffer[0] = seq;
        System.arraycopy("ack".getBytes(), 0, ackBuffer, 1, 3);
        DatagramPacket ackPacket = new DatagramPacket(
            ackBuffer, ackBuffer.length, socket.getRemoteSocketAddress());
        socket.send(ackPacket);
        if (seq == expectedSeq) {
          availableBytes = packet.getLength() - 1;
          availableBytesOffset = 1;
          expectedSeq++;
          break;
        }
      }
    }
  }

  public int readInt() throws IOException {
    byte[] intBytes = new byte[Integer.BYTES];
    receive(intBytes, intBytes.length);
    int result;
    try (ByteArrayInputStream arrayStream = new ByteArrayInputStream(intBytes);
         DataInputStream dataStream = new DataInputStream(arrayStream)) {
      result = dataStream.readInt();
    }
    return result;
  }

  public long readLong() throws IOException {
    byte[] longBytes = new byte[Long.BYTES];
    receive(longBytes, longBytes.length);
    long result;
    try (ByteArrayInputStream arrayStream = new ByteArrayInputStream(longBytes);
         DataInputStream dataStream = new DataInputStream(arrayStream)) {
      result = dataStream.readLong();
    }
    return result;
  }

  public void writeInt(int value) throws IOException {
    byte[] intBytes;
    ByteArrayOutputStream arrayStream =
        new ByteArrayOutputStream(Integer.BYTES);
    try (DataOutputStream dataStream = new DataOutputStream(arrayStream)) {
      dataStream.writeInt(value);
    }
    intBytes = arrayStream.toByteArray();
    send(intBytes);
  }

  public void writeLong(long value) throws IOException {
    byte[] longBytes;
    ByteArrayOutputStream arrayStream = new ByteArrayOutputStream(Long.BYTES);
    try (DataOutputStream dataStream = new DataOutputStream(arrayStream)) {
      dataStream.writeLong(value);
    }
    longBytes = arrayStream.toByteArray();
    send(longBytes);
  }

  public static TcpOverUdp accept(int port)
      throws SocketException, IOException {
    DatagramSocket socket = new DatagramSocket(port);

    byte[] buffer = new byte[8];
    DatagramPacket packet = new DatagramPacket(buffer, buffer.length);

    while (true) {
      socket.receive(packet);
      if ("connect".compareTo(
              new String(buffer, 1, 7, StandardCharsets.UTF_8)) == 0) {
        byte seq = buffer[0];
        byte[] ackBuffer = new byte[4];
        ackBuffer[0] = seq;
        System.arraycopy("ack".getBytes(), 0, ackBuffer, 1, 3);
        socket.connect(packet.getSocketAddress());
        DatagramPacket ackPacket = new DatagramPacket(
            ackBuffer, ackBuffer.length, socket.getRemoteSocketAddress());
        socket.send(ackPacket);
        packet = new DatagramPacket(buffer, 4);
        TcpOverUdp result = new TcpOverUdp(socket, (byte)2);
        return result;
      }
    }
  }
}
