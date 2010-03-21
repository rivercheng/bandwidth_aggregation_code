#ifndef __CHUNK_H__
#define __CHUNK_H__
#include <QByteArray>
#include <QSet>
#include <QDebug>

typedef int ChunkID;
typedef int PacketID;


class FECChunk {
    QByteArray      tempPacket;
    QSet<PacketID>  expectedIds;
    bool            includedFEC;
    int             b;
    int             k;
public:
    class NotReady {};

    static QSet<PacketID> chunkID2packetIDs (ChunkID cid, \
            int b, int k) 
    {
        QSet<PacketID> expectedIDs;
        int blockId = cid/b;
        int beginId = blockId * b * k + cid - b * blockId;
        for (int i = 0; i < k; i++)
        {
            expectedIDs.insert(beginId + i*b);
        }
        return expectedIDs;
    }

    static ChunkID packetID2chunkID(PacketID pid, int b, int k) {
        int blockId = pid/(b*k);
        return blockId * b + (pid - pid / b * b);
    }

    static QByteArray do_xor(QByteArray a, QByteArray b) {
        QByteArray l = a;
        QByteArray s = b;
        if (b.size() > a.size())
        {
            l = b;
            s = a;
        }
        for (int i = 0; i < s.size(); i++) {
            l[i] = l[i] ^ s.at(i);
        }
        return l;
    }

    FECChunk(ChunkID cid, int b, int k)
        : includedFEC(false), b(b), k(k)
    {
        expectedIds = chunkID2packetIDs(cid, b, k);
    }

    void addPacket(PacketID pid, QByteArray packet)
    {
        if (expectedIds.contains(pid)) {
            tempPacket = do_xor(tempPacket, packet);
            expectedIds.remove(pid);
        } else {
            qDebug() << "unexpected packet " << pid << endl;
        }

    }

    void addFEC(QByteArray packet)
    {
        tempPacket = do_xor(tempPacket, packet);
        includedFEC = true;
    }

    bool FECReady(void) {
        return expectedIds.empty();
    }

    bool recoverReady(void) {
        return includedFEC && (expectedIds.size() == 1);
    }

    bool full(void) {
        return expectedIds.empty();
    }
    
    bool FECreceived(void) {
        return includedFEC;
    }

    int waiting(void) {
        return expectedIds.size();
    }

    QByteArray packet(void) {
        if (expectedIds.empty() || \
                ((expectedIds.size() == 1) && includedFEC))
        {
            return tempPacket;
        }
        else
        {
            throw NotReady();
        }
    }

    PacketID recoverdID (void) {
        if ((expectedIds.size() == 1) && includedFEC)
        {
            return *(expectedIds.begin());
        }
        else
        {
            throw NotReady();
        }
    }
};
#endif
