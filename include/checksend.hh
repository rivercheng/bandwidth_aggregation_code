#ifndef __CHECK_SEND_HH__
#define __CHECK_SEND_HH__
#include <QByteArray>
#include <QHostAddress>
#include <QUdpSocket>
#include <pcap.h>
int checkSend(QUdpSocket *sock, char* dev, QByteArray packet, QHostAddress outAddr, quint16 outPort) {
    char errbuf[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;
    char filter[256];
    bpf_u_int32 mask;
    bpf_u_int32 net;
    struct pcap_pkthdr header;
    const u_char *p;
    bool needFree = false;
    if (!dev) {
        dev = pcap_lookupdev(errbuf);
        if (!dev) {
            qDebug() << "Cannot find dev " << errbuf;
            exit(1);
        } 
        needFree = true;
    }

    sprintf(filter, "udp and dst host %s and dst port %d", (char *)outAddr.toString().data(), outPort);

    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        qDebug() << "Couldn't get netmask for device "<< dev << ": "<<errbuf;
        exit(1);
    }


    pcap_t *handle = pcap_open_live(dev, 2048, 1, 1, errbuf);
    if (handle == NULL) {
        qDebug() << "Could not open device "<< dev <<": " << errbuf;
        exit(1);
    }
    if (pcap_compile(handle, &fp, filter, 1, net) == -1) {
        qDebug() << "Could not parse filter " << filter <<": " << errbuf;
    }

    int res = sock->writeDatagram(packet, outAddr, outPort);
    if (res != -1) {
        p = pcap_next(handle, &header);
        if (p && header.len < (unsigned int)res) {
            res = header.len;
        } else if (!p) {
            res = -1;
        }
    }
    pcap_close(handle);
    return res;
}
#endif
