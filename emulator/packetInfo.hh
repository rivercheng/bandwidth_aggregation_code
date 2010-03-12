#ifndef __PACKET_INFO_H__
#define __PACKET_INFO_H__
#include <QTime>
#include <QByteArray>
#include <QList>

struct PacketInfo {
    int id;
    bool status;
    int sec;
    int usec;
    PacketInfo(int id, bool status, int sec, int usec)
        :id(id), status(status), sec(sec), usec(usec) 
        {};
    PacketInfo()
        :id(-1), status(false), sec(0), usec(0) {};
};

struct PacketToSend {
    QTime time;
    int id;
    QByteArray packet;
    PacketToSend(QTime time, int id, QByteArray packet)
        :time(time), id(id), packet(packet) {};
};

class PacketList {
    public:
        void addPacket(PacketToSend packet) {
            QList<PacketToSend>::iterator it = list.begin();
            QList<PacketToSend>::const_iterator end = list.end();
            for (; it != end; ++it)
            {
                if (it->time > packet.time) {
                    list.insert(it, packet);
                    break;
                }
            }
        }
        
        QTime earlistTime() {
            return list[0].time;
        }

        QByteArray popPacket() {
            return list.takeFirst().packet;
        }

    private:
        QList<PacketToSend> list;
};



#endif
