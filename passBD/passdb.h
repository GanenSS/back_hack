#ifndef LISTDB_H
#define LISTDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDebug>
#include <QList>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>


#include "correlationlines.h"
#include "event.h"

// class buff
// {
// public:
//     QVector<Event> data;
// };


class passDB : public QObject
{
    Q_OBJECT
public:
    explicit passDB(QObject *parent = nullptr);
    ~passDB();

    QSqlDatabase    db;
    QSqlQuery       query;
    QSqlTableModel  *modelKeyWords;
    QSqlTableModel  *modelUser;
    QSqlTableModel  *modelEvents;

private:

    QList<QString> listKeyWords;
    QList<QString> splittingText;

    const QString dbType        = "QPSQL";
    const QString dbUserName    = "postgres";
    const QString dbName        = "back_Assistent";
    const QString dbPassword    = "1+Qwertis+1";

    const QString accessKey = "20033110";

    const QString dbTableKeyWords   = "keywords";
        const int IDKeyWord         = 0;
        const int key               = 1;
        const int responseID        = 2;

    const QString dbTableUsers      = "users";
        const int IDUsers           = 0;
        const int userName          = 1;
        const int hashedpassword    = 2;
        const int department        = 3;
        const int accessrights      = 4;

    const QString dbTableEvents     = "events";
        const int IDEvent           = 0;
        const int event_name        = 1;
        const int event_description = 2;
        const int event_date        = 3;
        const int faculty_name      = 4;

    correlationLines *correlation = new correlationLines;

public slots:
    bool createConnection();
    void addRowKeyResponse(QString keyWords, QString response);
    QString getResponse(QString keyWords);
    void filingListKeyWords(QList<QString> &listKeyWords);
    QString findDescript(QString keyText);

    QJsonDocument addRowUsers(QString userNameAddRow, QString hashedpasswordAddRow, QString departmentAddRow);
    QJsonDocument enterInAccount(QString login, QString password);

    void addRowEvents(QString eventName, QString eventDescription, QString eventDate, QString facultyName);
    QJsonDocument sendEvents(QString department);

    QJsonDocument validateAccessKey(QString accessKeyUser, QString login);

    void splitText(QList<QString> &splittingText, QString text);

    QString hashPassword(const QString &password);

};

#endif // LISTDB_H
