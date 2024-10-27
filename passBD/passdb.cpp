#include "passdb.h"

passDB::passDB(QObject *parent)
    : QObject{parent}
{
    createConnection();

    filingListKeyWords(listKeyWords);
}

passDB::~passDB()
{
    delete modelKeyWords;
}

bool passDB::createConnection()
{
    db = QSqlDatabase::addDatabase(dbType);
    db.setHostName("localhost");
    db.setDatabaseName(dbName);
    db.setUserName(dbUserName);
    db.setPassword(dbPassword);

    if (!db.open())
    {
        qDebug() << "Ошибка БД " + db.lastError().text();
        return false;
    }
    else
    {
        qDebug() << "Соединение с БД установлено!";

        query = QSqlQuery(db);

        modelKeyWords = new QSqlTableModel(this, db);
        modelKeyWords->setTable(dbTableKeyWords);
        modelKeyWords->select();

        modelUser = new QSqlTableModel(this, db);
        modelUser->setTable(dbTableUsers);
        modelUser->select();

        modelEvents = new QSqlTableModel(this, db);
        modelEvents->setTable(dbTableEvents);
        modelEvents->select();

        return true;
    }
}

void passDB::addRowKeyResponse(QString keyWords, QString response)
{
    int row = modelKeyWords->rowCount();
    modelKeyWords->insertRow(row);

    QModelIndex index = modelKeyWords->index(row, IDKeyWord);
    modelKeyWords->setData(index, row);

    index = modelKeyWords->index(row, key);
    modelKeyWords->setData(index, keyWords);

    index = modelKeyWords->index(row, responseID);
    modelKeyWords->setData(index, response);

    modelKeyWords->submitAll();
}

QString passDB::getResponse(QString keyWords)
{
    qDebug() << "keyWords: " +  keyWords;
    QStringList phrases = keyWords.split(", ");
    qDebug() << "correlation->phraseProcessing(phrases, listKeyWords): " << correlation->phraseProcessing(phrases, listKeyWords);

    QString str = findDescript(correlation->phraseProcessing(phrases, listKeyWords));
    phrases.clear();
    return str;
}

void passDB::filingListKeyWords(QList<QString> &listKeyWords)
{
    QModelIndex index;
    for (int i = 0; i < modelKeyWords->rowCount(); i++)
    {
        index = modelKeyWords->index(i, key);
        listKeyWords.push_back(modelKeyWords->data(index).toString());
    }
}

QString passDB::findDescript(QString keyText)
{
    if(keyText == "15")
        return "Попробуй задать другой вопрос";

    QModelIndex index;

    for(int i = 0; i < modelKeyWords->rowCount(); i++)
    {
        index = modelKeyWords->index(i, key);
        if(modelKeyWords->data(index).toString() == keyText)
        {
            index = modelKeyWords->index(i, responseID);
            return modelKeyWords->data(index).toString();
        }
    }
    return "Попробуйте ещё";
}

QJsonDocument passDB::addRowUsers(QString userNameAddRow, QString hashedpasswordAddRow, QString departmentAddRow)
{

    QModelIndex index;
    for(int i = 0; i < modelUser->rowCount(); i++)
    {
        index = modelUser->index(i, userName);
        if(modelUser->data(index) == userNameAddRow)
        {
            return QJsonDocument();
        }
    }

    int row = modelUser->rowCount();
    modelUser->insertRow(row);

    index = modelUser->index(row, IDUsers);
    modelUser->setData(index, row);

    index = modelUser->index(row, userName);
    modelUser->setData(index, userNameAddRow);

    index = modelUser->index(row, hashedpassword);
    modelUser->setData(index, hashPassword(hashedpasswordAddRow));

    index = modelUser->index(row, department);
    modelUser->setData(index, departmentAddRow);

    index = modelUser->index(row, accessrights);
    modelUser->setData(index, false);

    modelUser->submitAll();
    qDebug() << "Пользователь добавлен";

    QJsonObject mess;
    mess["status"] = "mess";
    mess["message"] = "Invalid";
    return QJsonDocument(mess);
}

QJsonDocument passDB::enterInAccount(QString login, QString password)
{
    QModelIndex index;

    for (int i = 0; i < modelUser->rowCount(); i++)
    {
        index = modelUser->index(i, userName);
        if (modelUser->data(index).toString() == login)
        {
            index = modelUser->index(i, hashedpassword);
            if (modelUser->data(index).toString() == hashPassword(password))
            {

                QJsonObject userInfo;


                userInfo["username"] = modelUser->data(modelUser->index(i, userName)).toString();
                userInfo["department"] = modelUser->data(modelUser->index(i, department)).toString();
                userInfo["accessRights"] = modelUser->data(modelUser->index(i, accessrights)).toBool();


                qDebug() << "Вход в аккаунт";
                qDebug() << QJsonDocument(userInfo);
                return QJsonDocument(userInfo);
            }
        }
    }


    return QJsonDocument();
}

void passDB::addRowEvents(QString eventName, QString eventDescription, QString eventDate, QString facultyName)
{
    int row = modelEvents->rowCount();
    modelEvents->insertRow(row);

    QModelIndex index= modelEvents->index(row, IDEvent);
    modelEvents->setData(index, row);

    index = modelEvents->index(row, event_name);
    modelEvents->setData(index, eventName);

    index = modelEvents->index(row, event_description);
    modelEvents->setData(index, eventDescription);

    index = modelEvents->index(row, event_date);
    modelEvents->setData(index, eventDate);

    index = modelEvents->index(row, faculty_name);
    modelEvents->setData(index, facultyName);

    qDebug() << "Добавлен ивент";
    modelEvents->submitAll();
}

QJsonDocument passDB::sendEvents(QString department)
{
    QJsonArray jsonArray;
    QModelIndex indexName, indexDesc, indexDate, indexDepart;
    QVector<Event> events;


    for (int i = 0; i < modelEvents->rowCount(); i++)
    {
        indexDepart = modelEvents->index(i, faculty_name);
        if (modelEvents->data(indexDepart).toString() == department)
        {
            indexName = modelEvents->index(i, event_name);
            indexDesc = modelEvents->index(i, event_description);
            indexDate = modelEvents->index(i, event_date);


            events.append(Event(
                modelEvents->data(indexName).toString(),
                modelEvents->data(indexDesc).toString(),
                modelEvents->data(indexDate).toString(),
                modelEvents->data(indexDepart).toString()));
        }
    }


    for (const Event& event : events) {
        jsonArray.append(event.toJson().object());
    }


    QJsonObject dataObject;
    dataObject["data"] = jsonArray;


    qDebug() << "Получение ивентов";
    return QJsonDocument(dataObject);
}

QJsonDocument passDB::validateAccessKey(QString accessKeyUser, QString login)
{
    QModelIndex index;
    QJsonObject response;

    if(accessKeyUser == accessKey)
    {
        for (int i = 0; i < modelUser->rowCount(); i++)
        {
            index = modelUser->index(i, userName);
            if (modelUser->data(index).toString() == login)
            {
                index = modelUser->index(i, accessrights);
                modelUser->setData(index, true);

                modelUser->submitAll();


                qDebug() << "true";
                response["username"] = true;
                return QJsonDocument(response);
            }
        }
    }

    response["username"] = false;
    qDebug() << "false";

    return QJsonDocument(response);
}

void passDB::splitText(QList<QString> &splittingText, QString text)
{
    QStringList parts = text.split(", ");
    for (const QString &part : parts)
    {
        if (!part.isEmpty())
        {
            splittingText.push_back(part);
        }
    }
}


QString passDB::hashPassword(const QString &password)
{
    QByteArray passwordBytes = password.toUtf8();
    QByteArray hashedBytes = QCryptographicHash::hash(passwordBytes, QCryptographicHash::Sha256);
    return hashedBytes.toHex();
}


