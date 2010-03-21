#ifndef __CODE_HEADER_H__
#define __CODE_HEADER_H__
#include <QByteArray>
#include <QDataStream>
#include "chunk.hh"
#include "gettime.hh"

inline QByteArray wrapPacket(PacketID pid, QByteArray datagram)
{
    QByteArray header;
    QDataStream dstr(&header, QIODevice::WriteOnly);
    PreciseTime t = PreciseTime::getTime();
    dstr << pid << t.sec << t.usec << (quint16)datagram.size();
    return header + datagram;
}

inline QByteArray wrapFecPacket(ChunkID cid, QByteArray datagram)
{
    return wrapPacket(static_cast<PacketID>(-cid-1), datagram);
}

inline QByteArray dewrap(QByteArray datagram)
{
    QDataStream dstr(datagram);
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
