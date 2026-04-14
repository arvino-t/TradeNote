#ifndef DEAL_H
#define DEAL_H

#include <QString>
#include <QDate>
#include <QJsonObject>
#include <QVariantMap>

class Deal {
public:
    Deal(const QString& ticker, double profit, int count, const QString& open_by, 
         const QString& close_by, const QDate& date = QDate::currentDate(), const QString& currency = "$");

    QVariantMap toMap() const;
    void updateFromMap(const QVariantMap& data);
    static Deal fromMap(const QVariantMap& data);

    QString ticker;
    double profit;
    int count;
    QString open_by;
    QString close_by;
    QDate date;
    QString currency;
};

#endif // DEAL_H
