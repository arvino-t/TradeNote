#ifndef TRADE_DIALOG_H
#define TRADE_DIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <memory>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCompleter>
#include "models/strategy.h"

class TradeDialog : public QDialog {
    Q_OBJECT
public:
    TradeDialog(QWidget* parent = nullptr, const QString& last_ticker = "", 
                const QVariantMap& edit_data = {}, const QStringList& strategies = {},
                const QString& default_strategy = "", const QMap<QString, std::shared_ptr<Strategy>>& strategy_objs = {});

    QVariantMap getData() const;

private slots:
    void onMarketChanged();
    void onTickerChanged();
    void fetchMoexTickers();
    void fetchTickerDetails();

private:
    QComboBox* market_combo;
    QLineEdit* ticker_edit;
    QDoubleSpinBox* profit_spin;
    QSpinBox* count_spin;
    QComboBox* strategy_combo;
    QComboBox* open_by_combo;
    QComboBox* close_by_combo;
    QDialogButtonBox* button_box;
    
    std::unique_ptr<QNetworkAccessManager> network_manager;
    QStringList tickers;
    QVariantMap edit_data;
    QMap<QString, std::shared_ptr<Strategy>> strategy_objs;
};

#endif // TRADE_DIALOG_H
