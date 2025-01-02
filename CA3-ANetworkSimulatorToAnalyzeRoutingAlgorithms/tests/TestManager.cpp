#include <QtTest/QtTest>
#include "DataGeneratorTests.cpp"
#include "DataLinkHeaderTests.cpp"
#include "IPHeaderTests.cpp"
#include "MACAddressTests.cpp"
#include "PacketTests.cpp"
#include "PortTests.cpp"
#include "RouterRegistryTests.cpp"
#include "TCPHeaderTests.cpp"

int main(int argc, char *argv[]) {
    int status = 0;

    {
        DataGeneratorTests dataGeneratorTests;
        status |= QTest::qExec(&dataGeneratorTests, argc, argv);
    }

    {
        DataLinkHeaderTests dataLinkHeaderTests;
        status |= QTest::qExec(&dataLinkHeaderTests, argc, argv);
    }

    {
        IPHeaderTests ipHeaderTests;
        status |= QTest::qExec(&ipHeaderTests, argc, argv);
    }

    {
        MACAddressTests macAddressTests;
        status |= QTest::qExec(&macAddressTests, argc, argv);
    }

    {
        PacketTests packetTests;
        status |= QTest::qExec(&packetTests, argc, argv);
    }

    {
        PortTests portTests;
        status |= QTest::qExec(&portTests, argc, argv);
    }

    {
        RouterRegistryTests routerRegistryTests;
        status |= QTest::qExec(&routerRegistryTests, argc, argv);
    }

    {
        TCPHeaderTests tcpHeaderTests;
        status |= QTest::qExec(&tcpHeaderTests, argc, argv);
    }

    return status;
}
