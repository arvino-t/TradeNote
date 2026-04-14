#include "deal.h"

Deal::Deal(const QString& ticker, double profit, int count, const QString& open_by, 
           const QString& close_by, const QDate& date, const QString& currency)
    : ticker(ticker), profit(profit), count(count), open_by(open_by), 
      close_by(close_by), date(date.isValid() ? date : QDate::currentDate()), currency(currency) {}

QVariantMap Deal::toMap() const {
    QVariantMap map;
    map["ticker"] = ticker;
    map["profit"] = profit;
    map["count"] = count;
    map["open_by"] = open_by;
    map["close_by"] = close_by;
    map["date"] = date;
    map["currency"] = currency;
    return map;
}

void Deal::updateFromMap(const QVariantMap& data) {
    if (data.contains("ticker")) ticker = data["ticker"].toString();
    if (data.contains("profit")) profit = data["profit"].toDouble();
    if (data.contains("count")) count = data["count"].toInt();
    if (data.contains("open_by")) open_by = data["open_by"].toString();
    if (data.contains("close_by")) close_by = data["close_by"].toString();
    if (data.contains("date")) date = data["date"].toDate();
    if (data.contains("currency")) currency = data["currency"].toString();
}

Deal Deal::fromMap(const QVariantMap& data) {
    return Deal(
        data.value("ticker", "").toString(),
        data.value("profit", 0.0).toDouble(),
        data.value("count", 0).toInt(),
        data.value("open_by", "Order").toString(),
        data.value("close_by", "Order").toString(),
        data.value("date").toDate(),
        data.value("currency", "$").toString()
    );
}
