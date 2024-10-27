#ifndef EVENT_H
#define EVENT_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>


class Event
{
public:
    // Конструктор для инициализации значений
    Event(const QString& name, const QString& description, const QString& date, const QString& faculty);

    // Метод для преобразования в JSON
    QJsonDocument toJson() const;

private:
    QString eventName;
    QString eventDescription;
    QString eventDate;
    QString facultyName;
};

#endif // EVENT_H
