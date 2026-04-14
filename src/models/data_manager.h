#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <memory>
#include <QString>
#include <QMap>
#include <QVariantMap>
#include "strategy.h"

class DataManager {
public:
    DataManager(const QString& data_dir = "app/data");

    QMap<QString, std::shared_ptr<Strategy>> loadAllStrategies();
    void saveAllStrategies(const QMap<QString, std::shared_ptr<Strategy>>& strategies);
    void saveStrategy(const Strategy& strategy);
    void deleteStrategyFiles(const QString& name);
    
    QVariantMap loadConfig();
    void saveConfig(const QVariantMap& config);

private:
    QString data_dir;
};

#endif // DATA_MANAGER_H
