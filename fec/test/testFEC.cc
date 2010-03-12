#include <QtTest>
#include <QObject>
#include "../chunk.hh"
#include "../udpRelay.hh"

class TestChunk : public QObject {
    Q_OBJECT
private slots:
    void pid2cid();
    void pid2cid_data();
    void cid2pid();
    //void cid2pid_data();
    void do_xor();
    void testwrapPacket();
    void testwrapFECPacket();
    void wrapDewrap();
    void testFECpacket();
    void testFECdecode();
};

void TestChunk::pid2cid_data()
{
    QTest::addColumn<PacketID>("pid");
    QTest::addColumn<ChunkID>("cid");

    QTest::newRow("first line, 0") << 0 << 0;
    QTest::newRow("first line, 4") << 4 << 4;
    QTest::newRow("second line, 5") << 5 << 0;
    QTest::newRow("second line, 9") << 9 << 4;
    QTest::newRow("second block, 20") << 20 << 5;
    QTest::newRow("second block, 24") << 24 << 9;
    QTest::newRow("second block, 25") << 25 << 5;
    QTest::newRow("second block, 29") << 29 << 9;
}

void TestChunk::pid2cid()
{
    QFETCH(PacketID, pid);
    QFETCH(ChunkID, cid);
    QCOMPARE(FECChunk::packetID2chunkID(pid, 5, 4), cid);
}

/*void TestChunk::cid2pid_data()
{
    QTest::addColumn<ChunkID>("cid");
    QTest::addColumn< QSet<PacketID> >("pids");

    QSet<PacketID> s;
    s << 0 << 5 << 10 << 15;
    QTest::newRow("cid=0") << 0 << s;
    s.clear();
    s << 1 << 6 << 11 << 16;
    QTest::newRow("cid=1") << 1 << s;
    s.clear();
    s << 20 << 25 << 30 << 35;
    QTest::newRow("cid=5") << 5 << s;
    s.clear();
    s << 22 << 27 << 32 << 37;
    QTest::newRow("cid=7") << 7 << s;
}*/

void TestChunk::cid2pid()
{
    QSet<PacketID> s;
    s << 0 << 5 << 10 << 15;
    QCOMPARE(FECChunk::chunkID2packetIDs(0, 5, 4), s);

    s.clear();
    s << 1 << 6 << 11 << 16;
    QCOMPARE(FECChunk::chunkID2packetIDs(1, 5, 4), s);
    
    s.clear();
    s << 20 << 25 << 30 << 35;
    QCOMPARE(FECChunk::chunkID2packetIDs(5, 5, 4), s);
    
    s.clear();
    s << 22 << 27 << 32 << 37;
    QCOMPARE(FECChunk::chunkID2packetIDs(7, 5, 4), s);
}

void TestChunk::do_xor() {
    QByteArray arr1 = QByteArray::fromHex("0000");
    QByteArray arr2 = QByteArray::fromHex("1111");
    QCOMPARE(FECChunk::do_xor(arr1, arr2), QByteArray::fromHex("1111"));

    arr1.resize(0);
    QCOMPARE(FECChunk::do_xor(arr1, arr2), QByteArray::fromHex("1111"));

    arr1 = QByteArray::fromHex("001011");
    QCOMPARE(FECChunk::do_xor(arr1, arr2), QByteArray::fromHex("110111"));
}

void TestChunk::testwrapPacket() {
    QByteArray arr1(10, 0);
    QByteArray arr2 = wrapPacket(101, arr1);
    QDataStream data(arr2);
    int id;
    quint16 len;
    data >>  id  >> len;
    QCOMPARE(id, 101);
    QCOMPARE(len, (unsigned short)10);
}

void TestChunk::testwrapFECPacket() {
    QByteArray arr1(10, 0);
    QByteArray arr2 = wrapFecPacket(101, arr1);
    QDataStream data(arr2);
    int id;
    quint16 len;
    data >>  id  >> len;
    QCOMPARE(id, -102);
    QCOMPARE(len, (unsigned short)10);
}
void TestChunk::wrapDewrap() {
    QByteArray arr1(10, 0);
    QByteArray arr2 = wrapFecPacket(101, arr1);
    QByteArray arr3 = dewrap(arr2);
    QCOMPARE(arr1, arr3);
}

void TestChunk::testFECpacket() {
    QByteArray arr1 = QByteArray::fromHex("1111");
    QByteArray arr2 = QByteArray::fromHex("2222");
    QByteArray arr3 = QByteArray::fromHex("3333");
    FECChunk chunk(0, 1, 2);
    chunk.addPacket(0, arr1);
    chunk.addPacket(1, arr2);
    QVERIFY(chunk.FECReady());
    QCOMPARE(chunk.packet(), arr3); 
}

void TestChunk::testFECdecode() {
    QByteArray arr1 = QByteArray::fromHex("1111");
    QByteArray arr2 = QByteArray::fromHex("2222");
    QByteArray arr3 = QByteArray::fromHex("3333");
    FECChunk chunk(0, 1, 2);
    chunk.addPacket(0, arr1);
    chunk.addFEC(arr3);
    QVERIFY(chunk.recoverReady());
    QCOMPARE(chunk.packet(), arr2); 
}





QTEST_MAIN(TestChunk)
#include "testFEC.moc"
