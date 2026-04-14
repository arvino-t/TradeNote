#include "data_manager.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>
#include <QDebug>

DataManager::DataManager(const QString& data_dir) : data_dir(data_dir) {
    QDir dir(data_dir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

QMap<QString, std::shared_ptr<Strategy>> DataManager::loadAllStrategies() {
    QMap<QString, std::shared_ptr<Strategy>> strategies;
    QDir dir(data_dir);
    
    // 1. Load from individual .json files
    QStringList jsonFiles = dir.entryList({"*.json"}, QDir::Files);
    for (const QString& filename : jsonFiles) {
        if (filename == "config.json") continue;
        
        QFile file(dir.filePath(filename));
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            QJsonObject obj = doc.object();
            
            QString name = obj.value("name").toString(filename.left(filename.length() - 5));
            QString type = obj.value("type").toString("Scalping");
            int limit = obj.value("deal_limit").toInt(3);
            QString market = obj.value("default_market").toString("MOEX Shares");
            QVariantMap settings = obj.value("settings").toObject().toVariantMap();
            
            strategies.insert(name, std::make_shared<Strategy>(name, type, limit, settings, market));
        }
    }

    // 2. Load deals from CSVs
    QStringList csvFiles = dir.entryList({"*.csv"}, QDir::Files);
    for (const QString& filename : csvFiles) {
        if (filename.endsWith("_journal.csv") || filename.endsWith("_todo.csv")) continue;
        
        QString strategy_name = filename.left(filename.length() - 4);
        if (!strategies.contains(strategy_name)) {
            strategies.insert(strategy_name, std::make_shared<Strategy>(strategy_name));
        }
        
        QFile file(dir.filePath(filename));
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);
            QString headerLine = in.readLine();
            QStringList headers = headerLine.split(",");
            
            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList values = line.split(",");
                if (values.size() < headers.size()) continue;
                
                QVariantMap row;
                for (int i = 0; i < headers.size(); ++i) {
                    QString key = headers[i].trimmed();
                    QString val = values[i].trimmed();
                    
                    if (key == "date") row[key] = QDate::fromString(val, Qt::ISODate);
                    else if (key == "profit") row[key] = val.toDouble();
                    else if (key == "count") row[key] = val.toInt();
                    else row[key] = val;
                }
                
                strategies[strategy_name]->addDeal(Deal::fromMap(row));
            }
        }

        // 3. Load journal from CSVs
        QFile journalFile(dir.filePath(strategy_name + "_journal.csv"));
        if (journalFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&journalFile);
            QString headerLine = in.readLine();
            QStringList headers = headerLine.split(",");
            
            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList values = line.split(",");
                if (values.size() < headers.size()) continue;
                
                QVariantMap row;
                for (int i = 0; i < headers.size(); ++i) {
                    QString key = headers[i].trimmed();
                    QString val = values[i].trimmed();
                    row[key] = val;
                }
                strategies[strategy_name]->addJournalEntry(JournalEntry::fromMap(row));
            }
        }

        // 4. Load TODO from CSVs
        QFile todoFile(dir.filePath(strategy_name + "_todo.csv"));
        if (todoFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&todoFile);
            QString headerLine = in.readLine();
            QStringList headers = headerLine.split(",");
            
            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList values = line.split(",");
                if (values.size() < headers.size()) continue;
                
                QVariantMap row;
                for (int i = 0; i < headers.size(); ++i) {
                    QString key = headers[i].trimmed();
                    QString val = values[i].trimmed();
                    if (key == "completed") row[key] = (val == "1");
                    else row[key] = val;
                }
                strategies[strategy_name]->addTodoItem(TodoItem::fromMap(row));
            }
        }
    }
    
    return strategies;
}

void DataManager::saveAllStrategies(const QMap<QString, std::shared_ptr<Strategy>>& strategies) {
    for (const auto& strategy : strategies) {
        saveStrategy(*strategy);
    }
}

void DataManager::saveStrategy(const Strategy& strategy) {
    QDir dir(data_dir);
    
    // Save deals to CSV
    QFile csvFile(dir.filePath(strategy.name + ".csv"));
    if (csvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&csvFile);
        out << "date,ticker,profit,currency,count,open_by,close_by\n";
        for (const auto& deal : strategy.deals) {
            out << deal.date.toString(Qt::ISODate) << ","
                << deal.ticker << ","
                << deal.profit << ","
                << deal.currency << ","
                << deal.count << ","
                << deal.open_by << ","
                << deal.close_by << "\n";
        }
    }

    // Save journal to CSV
    QFile journalFile(dir.filePath(strategy.name + "_journal.csv"));
    if (journalFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&journalFile);
        out << "timestamp,text,priority\n";
        for (const auto& entry : strategy.journal_entries) {
            QString escapedText = entry.text;
            escapedText.replace(",", ";");
            out << entry.timestamp.toString(Qt::ISODate) << ","
                << escapedText << ","
                << static_cast<int>(entry.priority) << "\n";
        }
    }

    // Save TODO to CSV
    QFile todoFile(dir.filePath(strategy.name + "_todo.csv"));
    if (todoFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&todoFile);
        out << "text,completed\n";
        for (const auto& item : strategy.todo_items) {
            QString escapedText = item.text;
            escapedText.replace(",", ";");
            out << escapedText << ","
                << (item.completed ? "1" : "0") << "\n";
        }
    }

    // Save strategy properties to JSON
    QFile jsonFile(dir.filePath(strategy.name + ".json"));
    if (jsonFile.open(QIODevice::WriteOnly)) {
        QJsonObject obj;
        obj["name"] = strategy.name;
        obj["type"] = strategy.strategy_type;
        obj["deal_limit"] = strategy.deal_limit;
        obj["default_market"] = strategy.default_market;
        obj["settings"] = QJsonObject::fromVariantMap(strategy.settings);
        
        QJsonDocument doc(obj);
        jsonFile.write(doc.toJson());
    }
}

void DataManager::deleteStrategyFiles(const QString& name) {
    QDir dir(data_dir);
    QFile::remove(dir.filePath(name + ".csv"));
    QFile::remove(dir.filePath(name + ".json"));
    QFile::remove(dir.filePath(name + "_journal.csv"));
    QFile::remove(dir.filePath(name + "_todo.csv"));
}

QVariantMap DataManager::loadConfig() {
    QFile file(QDir(data_dir).filePath("config.json"));
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        return doc.object().toVariantMap();
    }
    return {};
}

void DataManager::saveConfig(const QVariantMap& config) {
    QFile file(QDir(data_dir).filePath("config.json"));
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(QJsonObject::fromVariantMap(config));
        file.write(doc.toJson());
    }
}
