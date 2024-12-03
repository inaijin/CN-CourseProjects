#include <QCoreApplication>
#include <iostream>

int
main(int argv, char* argc[])
{

    QCoreApplication app(argv, argc);

    std::cout << "TEST" << std::endl;

    return app.exec();
}
