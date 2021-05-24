import time
import socket
import argparse
from pathlib import Path

HOST = "127.0.0.1"

LOG_FILE_PATH = Path(__file__).parent / "nmea0183_sample.log"

with LOG_FILE_PATH.open() as f:
    NMEA0183_SENTENCES = f.readlines()

def main():
    parser = argparse.ArgumentParser(description="Fake NMEA0183 sentence streamer over UDP")
    parser.add_argument("port", type=int, help="Port number to send to")

    args = parser.parse_args()

    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    for sentence in NMEA0183_SENTENCES:
        s.sendto(sentence.encode(),(HOST, args.port))
        time.sleep(0.01)


if __name__ == "__main__":
    main()