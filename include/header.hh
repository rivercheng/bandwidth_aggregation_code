#ifndef __CODE_HEADER_H__
#define __CODE_HEADER_H__
#include <QByteArray>
#include <QDataStream>
#include "chunk.hh"
#include "gettime.hh"
struct PacketInfo {
    int id;
    unsigned int sec;
    unsigned int usec;
    quint16 len;
};


inline QByteArray wrapPacket(PacketID pid, QByteArray datagram)
{
    QByteArray header;
    QDataStream dstr(&header, QIODevice::WriteOnly);
    PreciseTime t = PreciseTime::getTime();
    dstr << pid << t.sec << t.usec << (quint16)datagram.size();
    return header + datagram;
}

inline PacketInfo packetInfo(QByteArray datagram)
{
    QDataStream dstr(&datagram, QIODevice::ReadOnly);
    PacketInfo info;
    dstr >> info.id >> info.sec >> info.usec >> info.len;
    return info;
}

inline QByteArray wrapFecPacket(ChunkID cid, QByteArray datagram)
{
    return wrapPacket(static_cast<PacketID>(-cid-1), datagram);
}

inline QByteArray dewrap(QByteArray datagram)
{
    QDataStream dstr(&datagram, QIODevice::ReadOnly);
    int id;
    quint16 len;
    unsigned int sec;
    unsigned int usec;
    int length = sizeof id + sizeof len + sizeof sec + sizeof usec;
    dstr >> id >> sec >> usec >> len;
    datagram.remove(0, length);
    datagram.resize(len);
    return datagram;
}
#endif
