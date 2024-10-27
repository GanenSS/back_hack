#include "correlationlines.h"

correlationLines::correlationLines() {}

int correlationLines::levenshteinDistance(const QString &s1, const QString &s2) {
    int len1 = s1.length();
    int len2 = s2.length();
    QVector<QVector<int>> dp(len1 + 1, QVector<int>(len2 + 1));

    for (int i = 0; i <= len1; ++i)
        dp[i][0] = i;

    for (int j = 0; j <= len2; ++j)
        dp[0][j] = j;

    for (int i = 1; i <= len1; ++i) {
        for (int j = 1; j <= len2; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            dp[i][j] = std::min({dp[i - 1][j] + 1,
                                 dp[i][j - 1] + 1,
                                 dp[i - 1][j - 1] + cost});
        }
    }
    return dp[len1][len2];
}

QVariant correlationLines::findBestMatch(const QString &inputPhrase, const QList<QString> &keyPhrases) {
    if (keyPhrases.isEmpty()) return QVariant();

    QString bestMatch;
    int bestScore = std::numeric_limits<int>::max();

    for (const QString &keyPhrase : keyPhrases) {
        int score = levenshteinDistance(inputPhrase, keyPhrase);
        int maxLength = std::max(inputPhrase.length(), keyPhrase.length());

        if (maxLength > 0) {
            //double similarity = 1.0 - static_cast<double>(score) / maxLength;

            if (score < bestScore) {
                bestScore = score;
                bestMatch = keyPhrase;
            }
        }
    }


    if (bestScore > 0) {
        double overallSimilarity = 1.0 - static_cast<double>(bestScore) / std::max(inputPhrase.length(), bestMatch.length());
        if (overallSimilarity < 0.5) {
            return 15;
        }
    }

    return bestMatch.isEmpty() ? QVariant() : bestMatch;
}

QString correlationLines::phraseProcessing(QStringList phrases, const QList<QString> &keyPhrases) {
    QString overallBestMatch;
    int overallBestScore = std::numeric_limits<int>::max();

    if (keyPhrases.isEmpty()) return overallBestMatch;

    for (const QString &phrase : phrases) {
        QVariant result = findBestMatch(phrase.trimmed(), keyPhrases);

        if (result.isValid()) {
            if (result.toInt() == 15) {
                return "15";
            }

            QString currentBestMatch = result.toString();
            int currentScore = levenshteinDistance(phrase.trimmed(), currentBestMatch);

            if (currentScore < overallBestScore) {
                overallBestScore = currentScore;
                overallBestMatch = currentBestMatch;
            }
        }
    }

    return overallBestMatch;
}


