#include "sender.hh"
#include "listAddr.hh"
#include <QUdpSocket>
#include <cassert>
Sender::Sender(const QString & devName, const QHostAddress & srcAddr, quint16 srcPort, const QHostAddress & dstAddr, quint16 dstPort, QSemaphore* readyToSend, FlowDict *dict)
    :srcAddr_(srcAddr), dstAddr_(dstAddr), dstPort_(dstPort), readyToSend_(readyToSend), \
     isAvailable_(false), sock_(0), handle_(0), datalink_type_(DLT_NULL)
{
    sock_ = new QUdpSocket(this);
    if (!sock_->bind(srcAddr, srcPort)) {
        qDebug() << "Cannot bind to port " << srcPort;
        exit(1);
    }
    initPcap(devName);
    isAvailable_ = true;
    dataflow_ = dict->value(Interface(devName, srcAddr));
}

Sender::~Sender() {
    if (handle_) {
        pcap_close(handle_);
    }
    delete sock_;
}

void Sender::run(void) {
    while(true) {
        //qDebug() << "in run";
        readyToSend_->release();
        waitingPacket_.acquire();
        while(!trySending()) {
            ;
        }
        isAvailable_ = true;
        (*dataflow_) += (packet_.size() - 14);
        //qDebug() << packet_.size() << *dataflow_;
    }
}

void Sender::send(const QByteArray & Packet) {
    //qDebug() << "in send";
    assert(isAvailable_);
    isAvailable_ = false;
    packet_ = Packet;
    id_     = packetID(packet_);
    waitingPacket_.release();
}

bool Sender::trySending() {
    int res = sock_->writeDatagram(packet_, dstAddr_, dstPort_);
    while (res == -1) {
        usleep(100);
        res = sock_->writeDatagram(packet_, dstAddr_, dstPort_);
    }
    if (datalink_type_ == DLT_EN10MB || datalink_type_ == DLT_IEEE802_11) {
        return true;
    }
    struct pcap_pkthdr header;
    const u_char *p = pcap_next(handle_, &header);
    if (p && pcapPacketID(p) == id_) {
        return(true);
    } else {
        return(false);
    }
}

void Sender::initPcap(const QString & devName) {
    //Device Name
    QByteArray devNameArray(devName.toAscii());
    const char *dev = devNameArray.constData();

    //capture filter
    QString filter_str;
    QTextStream fs(&filter_str, QIODevice::WriteOnly);
    fs << "udp and src port " << sock_->localPort() << " and dst host " << dstAddr_.toString() << " and dst port " << dstPort_;
    QByteArray filterStrArray(filter_str.toAscii());
    const char *filter = filterStrArray.constData();
    qDebug() << filter;

    //Setup the pcap handle
    char errbuf[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;
    bpf_u_int32 mask;
    bpf_u_int32 net;
    
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        qDebug() << "Couldn't get netmask for device "<< dev << ": "<<errbuf;
        exit(1);
    }

    handle_ = pcap_open_live(dev, 2048, 1, 5000, errbuf);
    if (handle_ == NULL) {
        qDebug() << "Could not open device "<< dev <<": " << errbuf;
        exit(1);
    }
    
    //Setup the filter
    if (pcap_compile(handle_, &fp, filter, 1, net) == -1) {
        qDebug() << "Could not parse filter " << filter <<": " << errbuf;
        exit(1);
    }
     
    if (pcap_setfilter(handle_, &fp) == -1) {
        qDebug() << "Could not set filter " << filter <<": " << errbuf;
        exit(1);
    }

    datalink_type_ = pcap_datalink(handle_);
}

void Sender::changeSource(const QHostAddress& srcAddr, quint16 srcPort) {
    if (!sock_->bind(srcAddr, srcPort)) {
        qDebug() << "Cannot bind to " << srcAddr << ":" << srcPort;
        exit(1);
    }
    qDebug() << "ip changed from " << srcAddr;
}









