#include "assistserver.h"

AssistServer::AssistServer(QObject *parent) : QTcpServer(parent)
{
    connect(this, &QTcpServer::newConnection, this, &AssistServer::onNewConnection);
}

void AssistServer::onNewConnection()
{
    qDebug () << "СОЕДИНЕНИЕ";
    QTcpSocket *socket = nextPendingConnection();

    connect(socket, &QTcpSocket::readyRead, [this, socket]() {
        processRequest(socket);
    });
}

void AssistServer::processRequest(QTcpSocket *socket)
{
    QByteArray requestData = socket->readAll();
    int pos = requestData.indexOf("\r\n\r\n");

    if (pos != -1) {
        QByteArray jsonData = requestData.mid(pos + 4);

        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (doc.isObject()) {
            QJsonObject jsonObj = doc.object();

            if (jsonObj.contains("username") && jsonObj["username"].isString() &&
                jsonObj.contains("hashedpassword") && jsonObj["hashedpassword"].isString() &&
                jsonObj.contains("department") && jsonObj["department"].isString()) {

                QString userName = jsonObj["username"].toString();
                QString hashedPassword = jsonObj["hashedpassword"].toString();
                QString department = jsonObj["department"].toString();

                QJsonDocument responseDoc = DB.addRowUsers(userName, hashedPassword, department);
                if (responseDoc.isNull()) {
                    QByteArray errorResponse = "Invalid login or password.";
                    socket->write("HTTP/1.1 403 Unauthorized\r\nContent-Type: text/plain\r\nContent-Length: "
                                  + QByteArray::number(errorResponse.size()) + "\r\n\r\n"
                                  + errorResponse);
                } else {
                    QJsonObject responseObj;
                    responseObj["status"] = "User added successfully";
                    QJsonDocument responseDoc(responseObj);
                    QByteArray responseData = responseDoc.toJson();

                    socket->write("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: "
                                  + QByteArray::number(responseData.size()) + "\r\n\r\n"
                                  + responseData);
                }

            }
            else if (jsonObj.contains("username") && jsonObj["username"].isString() &&
                     jsonObj.contains("hashedpassword") && jsonObj["hashedpassword"].isString()) {

                QString userName = jsonObj["username"].toString();
                QString password = jsonObj["hashedpassword"].toString();

                QJsonDocument responseDoc = DB.enterInAccount(userName, password);
                if (responseDoc.isNull()) {

                    QByteArray errorResponse = "Invalid login or password.";
                    socket->write("HTTP/1.1 402 Unauthorized\r\nContent-Type: text/plain\r\nContent-Length: "
                                  + QByteArray::number(errorResponse.size()) + "\r\n\r\n"
                                  + errorResponse);
                } else {
                    QByteArray responseData = responseDoc.toJson();
                    socket->write("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: "
                                  + QByteArray::number(responseData.size()) + "\r\n\r\n"
                                  + responseData);
                }

            }
            else if(jsonObj.contains("key") && jsonObj["key"].isString() &&
                    jsonObj.contains("username") && jsonObj["username"].isString())
            {
                QString key = jsonObj["key"].toString();
                QString username = jsonObj["username"].toString();

                QByteArray responseData = DB.validateAccessKey(key, username).toJson();

                socket->write("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: "
                              + QByteArray::number(responseData.size()) + "\r\n\r\n"
                              + responseData);

            }else if(jsonObj.contains("name") && jsonObj["name"].isString() &&
                     jsonObj.contains("date") && jsonObj["date"].isString() &&
                     jsonObj.contains("description") && jsonObj["description"].isString() &&
                     jsonObj.contains("department") && jsonObj["department"].isString())
            {
                QString name = jsonObj["name"].toString();
                QString date = jsonObj["date"].toString();
                QString description = jsonObj["description"].toString();
                QString department = jsonObj["department"].toString();

                DB.addRowEvents(name, description, date, department);

                QByteArray responseData;
                responseData.append("dwa");

                socket->write("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: "
                              + QByteArray::number(responseData.size()) + "\r\n\r\n"
                              + responseData);

            }else if (jsonObj.contains("department") && jsonObj["department"].isString()) {
                QString department = jsonObj["department"].toString();

                QByteArray responseData = DB.sendEvents(department).toJson();

                socket->write("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: "
                              + QByteArray::number(responseData.size()) + "\r\n\r\n"
                              + responseData);
            }
            else if (jsonObj.contains("description") && jsonObj["description"].isString()) {
                QString description = jsonObj["description"].toString();

                QJsonObject responseObj;
                responseObj["description"] = DB.getResponse(description);
                QJsonDocument responseDoc(responseObj);
                QByteArray responseData = responseDoc.toJson();

                socket->write("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: "
                              + QByteArray::number(responseData.size()) + "\r\n\r\n"
                              + responseData);
            }
            else{

                QByteArray errorResponse = "{\"error\": \"Invalid fields in request.\"}";
                socket->write("HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\nContent-Length: "
                              + QByteArray::number(errorResponse.size()) + "\r\n\r\n"
                              + errorResponse);
            }
        } else {
            QByteArray errorResponse = "{\"error\": \"Invalid JSON.\"}";
            socket->write("HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\nContent-Length: "
                          + QByteArray::number(errorResponse.size()) + "\r\n\r\n"
                          + errorResponse);
        }
    } else {
        QByteArray errorResponse = "{\"error\": \"No JSON data found.\"}";
        socket->write("HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\nContent-Length: "
                      + QByteArray::number(errorResponse.size()) + "\r\n\r\n"
                      + errorResponse);
    }

    socket->close();
}

/* void AssistServer::threadConnection()
{
    QThread* thread = new QThread;
    this->moveToThread(thread);

    connect(thread, &QThread::started, this, &AssistServer::onNewConnection);
    connect(this, &AssistServer::destroyed, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}*/


