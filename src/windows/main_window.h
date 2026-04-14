#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QLabel>
#include <QMap>
#include <QPushButton>
#include <QMenu>
#include <memory>
#include "models/strategy.h"
#include "models/data_manager.h"
#include "windows/deals_window.h"
#include "windows/statistics_tab.h"
#include "windows/journal_tab.h"
#include "windows/todo_tab.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();

private slots:
    void onNewDealClicked();
    void onNewStrategyClicked();
    void onSaveStrategyClicked();
    void onOpenStrategyClicked();
    void onStrategyManagerClicked();
    void onSettingsClicked();
    void onTerminalClicked();
    void onTabChanged(int index) const;
    void onItemDoubleClicked(QTableWidgetItem* item);

private:
    void createMenuBar();
    void createStatusBar();
    void updateMainTable();
    void updateStatistics();
    void saveDeals();
    void loadDeals();
    void setActiveStrategy(const QString& name);
    static void applyTheme(const QString& theme);
    void updateUIState() const;
    QList<std::pair<Deal, Strategy>> getAllDealsWithStrategies();

    DataManager data_manager;
    QMap<QString, std::shared_ptr<Strategy>> strategies;
    QString active_strategy_name;
    QString last_ticker;
    
    QString current_theme = "Light";
    QString current_language = "English";
    QFont current_font;

    std::unique_ptr<QTabWidget> tabs;
    std::unique_ptr<QTableWidget> table;
    std::unique_ptr<QLabel> title_label;
    std::unique_ptr<QPushButton> add_deal_btn;
    
    std::unique_ptr<DealsWindow> deals_window;
    std::unique_ptr<StatisticsTab> statistics_tab;
    std::unique_ptr<JournalTab> journal_tab;
    std::unique_ptr<TodoTab> todo_tab;

    std::unique_ptr<QLabel> winrate_label;
    std::unique_ptr<QLabel> money_earned_label;
    std::unique_ptr<QLabel> total_period_profit_label;

    QMenu* tools_menu;
};

#endif // MAIN_WINDOW_H
