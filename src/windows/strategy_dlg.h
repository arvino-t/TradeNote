#ifndef STRATEGY_DLG_H
#define STRATEGY_DLG_H

#include <QDialog>
#include <QTableWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QMap>
#include "models/strategy.h"

#include <QListWidget>
#include <QStackedWidget>
#include <QToolButton>
#include <QFormLayout>

class StrategyManagerDialog : public QDialog {
    Q_OBJECT
public:
    StrategyManagerDialog(QWidget* parent, const QMap<QString, std::shared_ptr<Strategy>>& strategies, const QString& active_strategy_name);

    struct StrategyData {
        QString name; // Added to store the current name in the sidebar
        QString type;
        int deal_limit;
        QVariantMap settings;
        QString default_market;
    };
    [[nodiscard]] QMap<QString, StrategyData> getSelectedStrategies() const;
    [[nodiscard]] QString getActiveStrategyName() const;

private slots:
    void onAddStrategy();
    void onRemoveStrategy();
    void onSetActive();
    void onSelectionChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void onFieldChanged();

private:
    void setupUi();
    void loadStrategyToForm(const QString& name);
    void saveCurrentFormToData();
    void updateSidebarItemText(QListWidgetItem* item, const StrategyData& data);

    QListWidget* strategy_list;
    QPushButton* set_active_btn;
    QToolButton* add_button;
    QToolButton* remove_button;

    QWidget* details_container;
    QLineEdit* name_edit;
    QComboBox* type_combo;
    QSpinBox* limit_spin;
    QComboBox* market_combo;
    
    QString active_strategy_name;
    QMap<QString, StrategyData> strategies_data;
    QString current_selected_name; // To track which strategy is currently being edited in the form
};

#endif // STRATEGY_DLG_H
