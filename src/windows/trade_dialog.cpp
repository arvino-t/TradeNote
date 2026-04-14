#include "trade_dialog.h"
#include "resources/icons.h"
#include <QUrl>
#include <QNetworkRequest>

TradeDialog::TradeDialog(QWidget* parent, const QString& last_ticker, 
                         const QVariantMap& edit_data, const QStringList& strategies,
                         const QString& default_strategy, const QMap<QString, std::shared_ptr<Strategy>>& strategy_objs)
    : QDialog(parent), edit_data(edit_data), strategy_objs(strategy_objs) {
    
    setWindowTitle(edit_data.isEmpty() ? "Add New Deal" : "Edit Deal");
    setWindowIcon(get_app_icon());
    setFixedSize(300, 270);
    setWindowModality(Qt::ApplicationModal);

    QFormLayout* layout = new QFormLayout(this);

    market_combo = new QComboBox(this);
    market_combo->addItems({"MOEX Shares", "FORTS (Futures)"});
    layout->addRow("Market:", market_combo);

    ticker_edit = new QLineEdit(this);
    ticker_edit->setPlaceholderText("Enter Ticker (e.g. SBER)");
    QString initial_ticker = edit_data.value("ticker", last_ticker).toString();
    ticker_edit->setText(initial_ticker);
    layout->addRow("Ticker:", ticker_edit);

    profit_spin = new QDoubleSpinBox(this);
    profit_spin->setRange(-1000000000, 1000000000);
    
    if (!edit_data.isEmpty()) {
        profit_spin->setValue(edit_data.value("profit", 0.0).toDouble());
        QString currency = edit_data.value("currency", "$").toString();
        if (currency == " ₽") {
            profit_spin->setPrefix("");
            profit_spin->setSuffix(" ₽");
        } else {
            profit_spin->setPrefix(currency.contains(" ") || currency == "$" ? currency : currency + " ");
            profit_spin->setSuffix("");
        }
    } else {
        profit_spin->setPrefix("$");
    }
    layout->addRow("Profit:", profit_spin);

    count_spin = new QSpinBox(this);
    count_spin->setRange(1, 1000000000);
    if (!edit_data.isEmpty()) {
        count_spin->setValue(edit_data.value("count", 1).toInt());
    }
    layout->addRow("Count:", count_spin);

    strategy_combo = new QComboBox(this);
    strategy_combo->addItems(strategies);
    if (!edit_data.isEmpty()) {
        strategy_combo->setCurrentText(edit_data.value("strategy", "").toString());
    } else if (!default_strategy.isEmpty()) {
        strategy_combo->setCurrentText(default_strategy);
    }
    layout->addRow("Strategy:", strategy_combo);

    open_by_combo = new QComboBox(this);
    open_by_combo->addItems({"Order", "Market"});
    if (!edit_data.isEmpty()) {
        open_by_combo->setCurrentText(edit_data.value("open_by", "Order").toString());
    }
    layout->addRow("Open By:", open_by_combo);

    close_by_combo = new QComboBox(this);
    close_by_combo->addItems({"Order", "Market"});
    if (!edit_data.isEmpty()) {
        close_by_combo->setCurrentText(edit_data.value("close_by", "Order").toString());
    }
    layout->addRow("Close By:", close_by_combo);

    button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addRow(button_box);

    network_manager = std::make_unique<QNetworkAccessManager>(this);
    
    auto update_market = [this](const QString& strategy_name) {
        if (this->strategy_objs.contains(strategy_name)) {
            QString def_market = this->strategy_objs[strategy_name]->default_market;
            if (!def_market.isEmpty()) {
                market_combo->setCurrentText(def_market);
            }
        }
    };

    if (edit_data.isEmpty()) {
        update_market(strategy_combo->currentText());
    }

    connect(strategy_combo, &QComboBox::currentTextChanged, this, update_market);
    connect(market_combo, &QComboBox::currentIndexChanged, this, &TradeDialog::onMarketChanged);
    connect(ticker_edit, &QLineEdit::editingFinished, this, &TradeDialog::onTickerChanged);

    fetchMoexTickers();
}

QVariantMap TradeDialog::getData() const {
    QVariantMap data;
    data["ticker"] = ticker_edit->text().toUpper();
    data["profit"] = profit_spin->value();
    data["currency"] = !profit_spin->prefix().isEmpty() ? profit_spin->prefix() : profit_spin->suffix();
    data["strategy"] = strategy_combo->currentText();
    data["count"] = count_spin->value();
    data["open_by"] = open_by_combo->currentText();
    data["close_by"] = close_by_combo->currentText();
    return data;
}

void TradeDialog::onMarketChanged() {
    fetchMoexTickers();
    onTickerChanged();
}

void TradeDialog::onTickerChanged() {
    QString ticker = ticker_edit->text().toUpper();
    if (ticker.isEmpty()) return;
    fetchTickerDetails();
}

void TradeDialog::fetchMoexTickers() {
    QString urlStr;
    if (market_combo->currentText() == "MOEX Shares") {
        urlStr = "https://iss.moex.com/iss/engines/stock/markets/shares/boards/TQBR/securities.json?iss.meta=off&iss.only=securities&securities.columns=SECID";
    } else {
        urlStr = "https://iss.moex.com/iss/engines/futures/markets/forts/boards/RFUD/securities.json?iss.meta=off&iss.only=securities&securities.columns=SECID";
    }

    QNetworkRequest request{QUrl(urlStr)};
    QNetworkReply* reply = network_manager->get(request);
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonArray rows = doc.object()["securities"].toObject()["data"].toArray();
            tickers.clear();
            for (const auto& rowValue : rows) {
                tickers << rowValue.toArray()[0].toString();
            }
            QCompleter* completer = new QCompleter(tickers, this);
            completer->setCaseSensitivity(Qt::CaseInsensitive);
            completer->setFilterMode(Qt::MatchContains);
            ticker_edit->setCompleter(completer);
        }
        reply->deleteLater();
    });
}

void TradeDialog::fetchTickerDetails() {
    QString ticker = ticker_edit->text().toUpper();
    QString urlStr;
    if (market_combo->currentText() == "MOEX Shares") {
        urlStr = QString("https://iss.moex.com/iss/engines/stock/markets/shares/boards/TQBR/securities/%1.json?iss.meta=off&iss.only=securities,marketdata").arg(ticker);
    } else {
        urlStr = QString("https://iss.moex.com/iss/engines/futures/markets/forts/boards/RFUD/securities/%1.json?iss.meta=off&iss.only=securities,marketdata").arg(ticker);
    }

    QNetworkRequest request{QUrl(urlStr)};
    QNetworkReply* reply = network_manager->get(request);
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject obj = doc.object();
            
            QJsonObject secObj = obj["securities"].toObject();
            QJsonArray secCols = secObj["columns"].toArray();
            QJsonArray secRows = secObj["data"].toArray();
            
            if (!secRows.isEmpty()) {
                int currIdx = -1;
                for (int i = 0; i < secCols.size(); ++i) {
                    if (secCols[i].toString() == "CURRENCYID") {
                        currIdx = i;
                        break;
                    }
                }
                if (currIdx != -1) {
                    QString currency = secRows[0].toArray()[currIdx].toString();
                    if (currency == "SUR") {
                        profit_spin->setPrefix("");
                        profit_spin->setSuffix(" ₽");
                    } else {
                        QString prefix = (currency == "USD") ? "$" : currency + " ";
                        profit_spin->setPrefix(prefix);
                        profit_spin->setSuffix("");
                    }
                }
            }
        }
        reply->deleteLater();
    });
}
