#include "assistserver.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    AssistServer server; // Создаем экземпляр сервера
    server.listen(QHostAddress::Any, 8080); // Слушаем на всех адресах, порт 8080

    return a.exec();
}
