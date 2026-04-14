#include "main_window.h"
#include "resources/icons.h"
#include "resources/themes.h"
#include "windows/trade_dialog.h"
#include "windows/strategy_dlg.h"
#include "windows/settings_dlg.h"
#include "windows/terminal_window.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QMenuBar>
#include <QStatusBar>
#include <QHeaderView>
#include <QFileDialog>
#include <QApplication>
#include <QStandardPaths>
#include "version.h"

MainWindow::MainWindow() : data_manager() {
    setWindowTitle(QString("%1 %2").arg(PROJECT_NAME).arg(PROJECT_VERSION));
    setWindowIcon(get_app_icon());
    resize(900, 600);

    createMenuBar();
    createStatusBar();

    tabs = std::make_unique<QTabWidget>(this);
    setCentralWidget(tabs.get());

    // Tab 1: Trading
    auto* trading_widget = new QWidget(this);
    auto* trading_layout = new QVBoxLayout(trading_widget);
    title_label = std::make_unique<QLabel>("No Active Strategy", this);
    title_label->setStyleSheet("font-size: 24px; font-weight: bold;");
    
    table = std::make_unique<QTableWidget>(this);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Ticker", "Profit", "Count", "Open By", "Close By"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(table.get(), &QTableWidget::itemDoubleClicked, this, &MainWindow::onItemDoubleClicked);

    add_deal_btn = std::make_unique<QPushButton>("Add Deal", this);
    connect(add_deal_btn.get(), &QPushButton::clicked, this, &MainWindow::onNewDealClicked);

    trading_layout->addWidget(title_label.get());
    trading_layout->addWidget(table.get());
    trading_layout->addWidget(add_deal_btn.get());
    tabs->addTab(trading_widget, "Trading");

    // Tab 2: Deals History
    deals_window = std::make_unique<DealsWindow>([this]() { return getAllDealsWithStrategies(); }, this);
    tabs->addTab(deals_window.get(), "Deals History");

    // Tab 3: Statistics
    statistics_tab = std::make_unique<StatisticsTab>([this]() { return getAllDealsWithStrategies(); }, this);
    tabs->addTab(statistics_tab.get(), "Statistics");

    // Tab 4: Journal
    journal_tab = std::make_unique<JournalTab>(
        [this]() { 
            if (!active_strategy_name.isEmpty() && strategies.contains(active_strategy_name)) {
                return strategies[active_strategy_name]->journal_entries;
            }
            return QList<JournalEntry>();
        },
        [this](const JournalEntry& entry) {
            if (!active_strategy_name.isEmpty() && strategies.contains(active_strategy_name)) {
                strategies[active_strategy_name]->addJournalEntry(entry);
                saveDeals();
            }
        },
        [this](int index) {
            if (!active_strategy_name.isEmpty() && strategies.contains(active_strategy_name)) {
                strategies[active_strategy_name]->removeJournalEntry(index);
                saveDeals();
            }
        },
        this
    );
    tabs->addTab(journal_tab.get(), "Journal");

    // Tab 5: TODO
    todo_tab = std::make_unique<TodoTab>(
        [this]() {
            if (!active_strategy_name.isEmpty() && strategies.contains(active_strategy_name)) {
                return strategies[active_strategy_name]->todo_items;
            }
            return QList<TodoItem>();
        },
        [this](const TodoItem& item) {
            if (!active_strategy_name.isEmpty() && strategies.contains(active_strategy_name)) {
                strategies[active_strategy_name]->addTodoItem(item);
                saveDeals();
            }
        },
        [this](int index, bool completed) {
            if (!active_strategy_name.isEmpty() && strategies.contains(active_strategy_name)) {
                if (index >= 0 && index < strategies[active_strategy_name]->todo_items.size()) {
                    strategies[active_strategy_name]->todo_items[index].completed = completed;
                    saveDeals();
                }
            }
        },
        [this](int index) {
            if (!active_strategy_name.isEmpty() && strategies.contains(active_strategy_name)) {
                strategies[active_strategy_name]->removeTodoItem(index);
                saveDeals();
            }
        },
        this
    );
    tabs->addTab(todo_tab.get(), "TODO");

    connect(tabs.get(), &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    
    loadDeals();
}

void MainWindow::createMenuBar() {
    QMenu* file_menu = menuBar()->addMenu("&File");
    
    QAction* strat_mgr_action = file_menu->addAction("&Strategy Manager");
    connect(strat_mgr_action, &QAction::triggered, this, &MainWindow::onStrategyManagerClicked);
    
    file_menu->addSeparator();
    QAction* exit_action = file_menu->addAction("&Exit");
    connect(exit_action, &QAction::triggered, this, &MainWindow::close);

    QMenu* settings_menu = menuBar()->addMenu("&Settings");
    QAction* gen_settings_action = settings_menu->addAction("&General Settings");
    connect(gen_settings_action, &QAction::triggered, this, &MainWindow::onSettingsClicked);

    tools_menu = menuBar()->addMenu("&Tools");
    QAction* terminal_action = tools_menu->addAction("&Terminal");
    terminal_action->setShortcut(QKeySequence("Ctrl+T"));
    connect(terminal_action, &QAction::triggered, this, &MainWindow::onTerminalClicked);

    QMenu* help_menu = menuBar()->addMenu("&Help");
    help_menu->addAction("&About", [this]() {
        QString about_text = QString("%1 v%2\n\nAuthor: Anton Rubtsov")
            .arg(PROJECT_NAME)
            .arg(PROJECT_VERSION);
        QMessageBox::about(this, "About", about_text);
    });
}

void MainWindow::createStatusBar() {
    winrate_label = std::make_unique<QLabel>("Daily Winrate: 0%", this);
    money_earned_label = std::make_unique<QLabel>("Daily Earned: $0.00", this);
    total_period_profit_label = std::make_unique<QLabel>("Total Profit: $0.00", this);

    statusBar()->addPermanentWidget(winrate_label.get());
    statusBar()->addPermanentWidget(money_earned_label.get());
    statusBar()->addPermanentWidget(total_period_profit_label.get());
}

QList<std::pair<Deal, Strategy>> MainWindow::getAllDealsWithStrategies() {
    QList<std::pair<Deal, Strategy>> result;
    for (auto it = strategies.begin(); it != strategies.end(); ++it) {
        for (const auto& deal : it.value()->deals) {
            result.append({deal, *it.value()});
        }
    }
    return result;
}

void MainWindow::onNewDealClicked() {
    if (active_strategy_name.isEmpty() || !strategies.contains(active_strategy_name)) {
        QMessageBox::warning(this, "No Active Strategy", "Please select or create an active strategy first.");
        return;
    }
    QStringList strat_names = strategies.keys();
    std::sort(strat_names.begin(), strat_names.end());
    
    TradeDialog dialog(this, last_ticker, {}, strat_names, active_strategy_name, strategies);
    if (dialog.exec() == QDialog::Accepted) {
        QVariantMap data = dialog.getData();
        QString strategy_name = data.value("strategy").toString();
        
        if (strategies.contains(strategy_name)) {
            if (strategies[strategy_name]->isDealLimitReached()) {
                QMessageBox::warning(this, "Limit Reached", QString("Daily deal limit reached for '%1'!").arg(strategy_name));
                return;
            }
        } else {
            strategies.insert(strategy_name, std::make_shared<Strategy>(strategy_name));
        }

        Deal deal = Deal::fromMap(data);
        last_ticker = deal.ticker;
        strategies[strategy_name]->addDeal(deal);
        
        updateMainTable();
        updateStatistics();
        saveDeals();
    }
}

void MainWindow::updateMainTable() {
    table->setRowCount(0);
    if (active_strategy_name.isEmpty() || !strategies.contains(active_strategy_name)) return;

    QDate today = QDate::currentDate();
    auto deals = strategies[active_strategy_name]->deals;
    
    QList<Deal> daily_deals;
    for (const auto& d : deals) if (d.date == today) daily_deals.append(d);
    
    std::ranges::sort(daily_deals, [](const auto& a, const auto& b) { return a.date > b.date; });

    for (const auto& deal : daily_deals) {
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(deal.ticker));
        
        QString profit_str = deal.currency.startsWith("$") ? 
            QString("%1%2").arg(deal.currency).arg(deal.profit, 0, 'f', 2) :
            QString("%1%2").arg(deal.profit, 0, 'f', 2).arg(deal.currency);
        
        auto* p_item = new QTableWidgetItem(profit_str);
        if (deal.profit > 0) p_item->setForeground(Qt::green);
        else if (deal.profit < 0) p_item->setForeground(Qt::red);
        table->setItem(row, 1, p_item);
        
        table->setItem(row, 2, new QTableWidgetItem(QString::number(deal.count)));
        table->setItem(row, 3, new QTableWidgetItem(deal.open_by));
        table->setItem(row, 4, new QTableWidgetItem(deal.close_by));
    }
}

void MainWindow::updateStatistics() {
    if (active_strategy_name.isEmpty() || !strategies.contains(active_strategy_name)) return;
    
    const auto& strat = *strategies[active_strategy_name];
    double daily_profit = strat.dailyProfit();
    double winrate = strat.dailyWinrate();
    double total_profit = strat.totalProfit();
    
    winrate_label->setText(QString("Daily Winrate: %1%").arg(winrate, 0, 'f', 1));
    
    QString currency = strat.deals.isEmpty() ? "$" : strat.deals.last().currency;
    QString earned_str = currency.startsWith("$") ? QString("%1%2").arg(currency).arg(daily_profit, 0, 'f', 2) : QString("%1%2").arg(daily_profit, 0, 'f', 2).arg(currency);
    money_earned_label->setText(QString("Daily Earned: %1").arg(earned_str));
    
    QString total_str = currency.startsWith("$") ? QString("%1%2").arg(currency).arg(total_profit, 0, 'f', 2) : QString("%1%2").arg(total_profit, 0, 'f', 2).arg(currency);
    total_period_profit_label->setText(QString("Total Profit: %1").arg(total_str));
}

void MainWindow::onTabChanged(int index) const
{
    QString text = tabs->tabText(index);
    if (text == "Deals History") deals_window->refreshData();
    else if (text == "Statistics") statistics_tab->refreshData();
    else if (text == "Journal") journal_tab->refreshData();
    else if (text == "TODO") todo_tab->refreshData();
}

void MainWindow::onItemDoubleClicked(QTableWidgetItem* item) {
    // Basic implementation for double click edit
}

void MainWindow::onStrategyManagerClicked() {
    StrategyManagerDialog dialog(this, strategies, active_strategy_name);
    if (dialog.exec() == QDialog::Accepted) {
        auto results = dialog.getSelectedStrategies();
        QString new_active = dialog.getActiveStrategyName();
        
        QMap<QString, std::shared_ptr<Strategy>> new_strategies;
        for (auto it = results.begin(); it != results.end(); ++it) {
            QString name = it.key();
            QString type = it.value().type;
            int limit = it.value().deal_limit;
            QString market = it.value().default_market;
            
            bool found = false;
            for (auto s_it = strategies.begin(); s_it != strategies.end(); ++s_it) {
                if (s_it.key() == name) {
                    std::shared_ptr<Strategy> s = s_it.value();
                    s->strategy_type = type;
                    s->deal_limit = limit;
                    s->default_market = market;
                    new_strategies.insert(name, s);
                    found = true;
                    break;
                }
            }
            if (!found) {
                new_strategies.insert(name, std::make_shared<Strategy>(name, type, limit, QVariantMap{}, market));
            }
        }
        
        // Handle deletions
        for (auto it = strategies.begin(); it != strategies.end(); ++it) {
            if (!new_strategies.contains(it.key())) data_manager.deleteStrategyFiles(it.key());
        }
        
        strategies = new_strategies;
        setActiveStrategy(new_active);
        saveDeals();
    }
}

void MainWindow::setActiveStrategy(const QString& name) {
    active_strategy_name = name;
    if (!active_strategy_name.isEmpty()) title_label->setText(active_strategy_name);
    else title_label->setText("No Active Strategy");
    updateMainTable();
    updateStatistics();
    updateUIState();
}

void MainWindow::applyTheme(const QString& theme) {
    if (theme == "JetBrains Dark") {
        qApp->setStyleSheet(themes::JETBRAINS_DARK_STYLESHEET);
    } else if (theme == "Dracula") {
        qApp->setStyleSheet(themes::DRACULA_STYLESHEET);
    } else if (theme == "Solarized Dark") {
        qApp->setStyleSheet(themes::SOLARIZED_DARK_STYLESHEET);
    } else if (theme == "Gruvbox") {
        qApp->setStyleSheet(themes::GRUVBOX_STYLESHEET);
    } else {
        qApp->setStyleSheet("");
    }
}

void MainWindow::updateUIState() const
{
    bool has_active_strategy = !strategies.isEmpty() && !active_strategy_name.isEmpty();
    add_deal_btn->setEnabled(has_active_strategy);
    tools_menu->setEnabled(has_active_strategy);
    
    // Disable all tabs except the first one (Trading) if no active strategy
    for (int i = 1; i < tabs->count(); ++i) {
        tabs->setTabEnabled(i, has_active_strategy);
    }
}

void MainWindow::onSettingsClicked() {
    SettingsDialog dialog(this);
    dialog.setSelectedSettings(current_theme, current_font, current_language, data_manager.getDataDir());
    if (dialog.exec() == QDialog::Accepted) {
        auto settings = dialog.getSelectedSettings();
        current_theme = settings["theme"].toString();
        current_language = settings["language"].toString();
        current_font = QFont(settings["font"].toString());
        QString new_data_dir = settings["data_dir"].toString();
        
        setFont(current_font);
        applyTheme(current_theme);
        
        if (new_data_dir != data_manager.getDataDir()) {
            data_manager.setDataDir(new_data_dir);
            loadDeals(); // Re-load everything from new directory
        } else {
            saveDeals();
        }
    }
}

void MainWindow::onTerminalClicked() {
    if (strategies.isEmpty() || active_strategy_name.isEmpty()) {
        QMessageBox::warning(this, "No Active Strategy", "Please select or create an active strategy first.");
        return;
    }
    TerminalWindow dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        auto deals = dialog.getImportedDeals();
        if (deals.isEmpty() || active_strategy_name.isEmpty()) return;

        auto& strat = *strategies[active_strategy_name];
        int imported = 0;
        for (const auto& dVar : deals) {
            if (strat.isDealLimitReached()) break;
            QVariantMap dMap = dVar.toMap();
            Deal deal(dMap["ticker"].toString(), dMap["profit"].toDouble(), dMap["count"].toInt(), "Terminal", "Terminal", QDate::fromString(dMap["date"].toString(), Qt::ISODate), "₽");
            strat.addDeal(deal);
            imported++;
        }
        updateMainTable();
        updateStatistics();
        saveDeals();
        QMessageBox::information(this, "Import", QString("Imported %1 deals").arg(imported));
    }
}

void MainWindow::saveDeals() {
    data_manager.saveAllStrategies(strategies);
    QVariantMap config;
    config["active_strategy"] = active_strategy_name;
    config["theme"] = current_theme;
    config["language"] = current_language;
    config["font"] = current_font.toString();
    config["data_dir"] = data_manager.getDataDir();
    data_manager.saveConfig(config);
}

void MainWindow::loadDeals() {
    strategies = data_manager.loadAllStrategies();
    QVariantMap config = data_manager.loadConfig();
    if (!config.isEmpty()) {
        active_strategy_name = config["active_strategy"].toString();
        current_theme = config["theme"].toString();
        current_language = config["language"].toString();
        if (config.contains("font")) current_font.fromString(config["font"].toString());
        
        setFont(current_font);
        applyTheme(current_theme);
    }
    setActiveStrategy(active_strategy_name);

    if (strategies.isEmpty()) {
        QMessageBox::information(this, "No Strategies", "No strategies found. Please create a new strategy to begin.");
        onStrategyManagerClicked();
    }
}

// Stubs for missing methods if any
void MainWindow::onNewStrategyClicked() {}
void MainWindow::onSaveStrategyClicked() { saveDeals(); }
void MainWindow::onOpenStrategyClicked() {}
