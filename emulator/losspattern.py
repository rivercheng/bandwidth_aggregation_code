import sys
c = [0, 0]
prev = 0
if __name__ == "__main__":
    for line in open(sys.argv[1]):
        entries = line.split()
        id = int(entries[0])
        recved = (entries[1] == "1")
        if recved != prev:
            print "%d(%d) "%(prev, c[prev])
            c[prev] = 0
            c[recved] = 1
            prev = recved
        else:
            c[prev] += 1
    print "%d(%d)" % (prev, c[prev])


    
            

        
