#include "event.h"

// Определение конструктора
Event::Event(const QString& name, const QString& description, const QString& date, const QString& faculty)
    : eventName(name), eventDescription(description), eventDate(date), facultyName(faculty) {}

QJsonDocument Event::toJson() const
{
    QJsonObject jsonObj;
    jsonObj["event_name"] = eventName;
    jsonObj["event_description"] = eventDescription;
    jsonObj["event_date"] = eventDate;
    jsonObj["faculty_name"] = facultyName;

    return QJsonDocument(jsonObj);
}
