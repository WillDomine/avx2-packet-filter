import socket
import struct
import time

# Configuration
HOST = '127.0.0.1'
PORT = 9090

# Packet Structure:
#   Header (4 bytes)
#   Payload (28 bytes)
# Total: 32 Bytes
PACKET_FMT = '<I28x' # Little-Endian Unsigned Int, 28 pad bytes

# Mask
MASK_ADMIN = 0x80000000  # Bit 31 set

def send_packet(sock, header_val, name):
    # Pack the integer into 32 bytes (4 byte header + 28 bytes padding)
    data = struct.pack(PACKET_FMT, header_val)
    
    print(f"[{name}] Sending Header: {hex(header_val)}...")
    sock.sendall(data)
    time.sleep(0.5) # Give server time to process

def main():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((HOST, PORT))
        print(f"Connected to {HOST}:{PORT}")

        # 1. Send a GOOD Packet (Normal Player)
        # ID: 1, No Admin Bit
        send_packet(s, 0x00000001, "GOOD PACKET")

        # 2. Send a BAD Packet (Admin Bit Set)
        # ID: 1, WITH Admin Bit (0x80000000)
        # Result: 0x80000001
        bad_header = MASK_ADMIN | 1
        send_packet(s, bad_header, "BAD PACKET")

        # 3. Send another GOOD Packet to see if we are still alive
        send_packet(s, 0x00000002, "GOOD PACKET 2")

        s.close()
    except ConnectionRefusedError:
        print("Error: Could not connect. Is the server running (with sudo)?")

if __name__ == "__main__":
    main()