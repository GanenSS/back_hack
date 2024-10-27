#ifndef CORRELATIONLINES_H
#define CORRELATIONLINES_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QMap>

class correlationLines : public QObject
{
    Q_OBJECT
public:
    correlationLines();

    QString phraseProcessing(QStringList phrases, const QList<QString> &keyPhrases);
    QVariant findBestMatch(const QString &inputPhrase, const QList<QString> &keyPhrases);
    int levenshteinDistance(const QString &s1, const QString &s2);

private:
    QList<QString> smalListText;

    QString bestMatch = "";
};

#endif // CORRELATIONLINES_H
