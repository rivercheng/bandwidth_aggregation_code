import sys

if __name__ == "__main__":
    recvArray = [0] * 1000000
    total = 0
    for line in open(sys.argv[1]):
            entries = line.split()
            packetId = int(entries[0])
            if packetId >= 0 :
                send_sec = int(entries[1])
                send_usec = int(entries[2])
                recv_sec = int(entries[3])
                recv_usec = int(entries[4])
                recvArray[packetId] = 1
                total = packetId + 1
            else :
                if  -packetId > total:
                    total = -packetId
    for i in range(total):
        print i, recvArray[i]

    
            

        
