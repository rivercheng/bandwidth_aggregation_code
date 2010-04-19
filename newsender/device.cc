#include <pcap.h>
#include "device.hh"
#include "header.hh"
#include "checksocket.hh"
//Device::Device(QString name, quint16 inPort, QHostAddress outAddr, quint16 outPort)
Device::Device(QString name, CheckSocket *sock, QHostAddress outAddr, quint16 outPort)
    :sock_(sock), name_(name.toAscii()), handle(0)
{
    QString filter_str;
    QTextStream fs(&filter_str, QIODevice::WriteOnly);
    fs << "udp and src port " << sock->localPort() << " and dst host " << outAddr.toString() << " and dst port " << outPort;
    filter_ = filter_str.toAscii();
    init();
}

void Device::init() {
    char errbuf[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;
    bpf_u_int32 mask;
    bpf_u_int32 net;
    const char *dev = name_.constData();
    const char *filter = filter_.constData();

    qDebug() << "dev: " << dev;
    
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        qDebug() << "Couldn't get netmask for device "<< dev << ": "<<errbuf;
        exit(1);
    }

    handle = pcap_open_live(dev, 2048, 1, 300, errbuf);
    if (handle == NULL) {
        qDebug() << "Could not open device "<< dev <<": " << errbuf;
        exit(1);
    }

    qDebug() << filter;

    if (pcap_compile(handle, &fp, filter, 1, net) == -1) {
        qDebug() << "Could not parse filter " << filter <<": " << errbuf;
        exit(1);
    }
     
    if (pcap_setfilter(handle, &fp) == -1) {
        qDebug() << "Could not set filter " << filter <<": " << errbuf;
        exit(1);
    }
}

void Device::capture() {
    struct pcap_pkthdr  header;
    const u_char *p;
    p=pcap_next(handle, &header);
    if (p) {
        //QByteArray pdata((const char*)p, 14);
        //PacketID id = packetInfo(pdata).id;
        //qDebug() << "captured " << id;
        emit received();
    } else {
        emit resend();
    }
}





    

