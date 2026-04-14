#include "strategy_dlg.h"
#include "resources/icons.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QHeaderView>
#include <QSplitter>
#include <QGroupBox>

StrategyManagerDialog::StrategyManagerDialog(QWidget* parent, const QMap<QString, std::shared_ptr<Strategy>>& strategies, const QString& active_name)
    : QDialog(parent), active_strategy_name(active_name) {
    
    setWindowTitle("Strategy Manager");
    setWindowIcon(get_app_icon());
    resize(700, 500);
    setWindowModality(Qt::ApplicationModal);

    // Initial load of data
    for (auto it = strategies.begin(); it != strategies.end(); ++it) {
        StrategyData sd;
        sd.name = it.key();
        sd.type = it.value()->strategy_type;
        sd.deal_limit = it.value()->deal_limit;
        sd.settings = it.value()->settings;
        sd.default_market = it.value()->default_market;
        strategies_data.insert(it.key(), sd);
    }

    setupUi();

    // Populate list
    for (const auto& name : strategies_data.keys()) {
        QListWidgetItem* item = new QListWidgetItem(name, strategy_list);
        if (name == active_strategy_name) {
            item->setFont(QFont(item->font().family(), -1, QFont::Bold));
        }
    }

    if (strategy_list->count() > 0) {
        // Find active strategy in list
        auto items = strategy_list->findItems(active_strategy_name, Qt::MatchExactly);
        if (!items.isEmpty()) {
            strategy_list->setCurrentItem(items.first());
        } else {
            strategy_list->setCurrentRow(0);
        }
    } else {
        details_container->setEnabled(false);
    }
}

void StrategyManagerDialog::setupUi() {
    QVBoxLayout* main_layout = new QVBoxLayout(this);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

    // Sidebar
    QWidget* sidebar = new QWidget(this);
    QVBoxLayout* sidebar_layout = new QVBoxLayout(sidebar);
    sidebar_layout->setContentsMargins(0, 0, 0, 0);

    strategy_list = new QListWidget(this);
    connect(strategy_list, &QListWidget::currentItemChanged, this, &StrategyManagerDialog::onSelectionChanged);

    QHBoxLayout* toolbar_layout = new QHBoxLayout();
    toolbar_layout->setSpacing(2);
    add_button = new QToolButton(this);
    add_button->setText("+");
    connect(add_button, &QToolButton::clicked, this, &StrategyManagerDialog::onAddStrategy);
    
    remove_button = new QToolButton(this);
    remove_button->setText("-");
    connect(remove_button, &QToolButton::clicked, this, &StrategyManagerDialog::onRemoveStrategy);

    toolbar_layout->addWidget(add_button);
    toolbar_layout->addWidget(remove_button);
    toolbar_layout->addStretch();

    sidebar_layout->addWidget(strategy_list);
    sidebar_layout->addLayout(toolbar_layout);

    // Details Area
    details_container = new QWidget(this);
    QVBoxLayout* details_layout = new QVBoxLayout(details_container);

    QHBoxLayout* header_layout = new QHBoxLayout();
    set_active_btn = new QPushButton("Set as Active", this);
    connect(set_active_btn, &QPushButton::clicked, this, &StrategyManagerDialog::onSetActive);
    header_layout->addStretch();
    header_layout->addWidget(set_active_btn);
    details_layout->addLayout(header_layout);

    QFormLayout* form_layout = new QFormLayout();
    name_edit = new QLineEdit(this);
    connect(name_edit, &QLineEdit::textEdited, this, &StrategyManagerDialog::onFieldChanged);

    type_combo = new QComboBox(this);
    type_combo->addItems({"Scalping", "Day Trading", "Swing Trading", "Position Trading"});
    connect(type_combo, &QComboBox::currentTextChanged, this, &StrategyManagerDialog::onFieldChanged);

    limit_spin = new QSpinBox(this);
    limit_spin->setRange(1, 100);
    connect(limit_spin, QOverload<int>::of(&QSpinBox::valueChanged), this, &StrategyManagerDialog::onFieldChanged);

    market_combo = new QComboBox(this);
    market_combo->addItems({"MOEX Shares", "FORTS (Futures)"});
    connect(market_combo, &QComboBox::currentTextChanged, this, &StrategyManagerDialog::onFieldChanged);

    form_layout->addRow("Name:", name_edit);
    form_layout->addRow("Type:", type_combo);
    form_layout->addRow("Daily Limit:", limit_spin);
    form_layout->addRow("Default Market:", market_combo);

    details_layout->addLayout(form_layout);
    details_layout->addStretch();

    splitter->addWidget(sidebar);
    splitter->addWidget(details_container);
    splitter->setStretchFactor(1, 1);

    main_layout->addWidget(splitter);

    QDialogButtonBox* button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    main_layout->addWidget(button_box);
}

void StrategyManagerDialog::onSelectionChanged(QListWidgetItem* current, QListWidgetItem* previous) {
    if (current) {
        details_container->setEnabled(true);
        current_selected_name = current->text();
        loadStrategyToForm(current_selected_name);
        set_active_btn->setEnabled(current_selected_name != active_strategy_name);
    } else {
        details_container->setEnabled(false);
    }
}

void StrategyManagerDialog::loadStrategyToForm(const QString& name) {
    if (!strategies_data.contains(name)) return;
    
    const auto& data = strategies_data[name];
    
    // Block signals to prevent onFieldChanged being called while loading
    name_edit->blockSignals(true);
    type_combo->blockSignals(true);
    limit_spin->blockSignals(true);
    market_combo->blockSignals(true);

    name_edit->setText(data.name);
    type_combo->setCurrentText(data.type);
    limit_spin->setValue(data.deal_limit);
    market_combo->setCurrentText(data.default_market);

    name_edit->blockSignals(false);
    type_combo->blockSignals(false);
    limit_spin->blockSignals(false);
    market_combo->blockSignals(false);
}

void StrategyManagerDialog::onFieldChanged() {
    if (current_selected_name.isEmpty() || !strategies_data.contains(current_selected_name)) return;

    QString new_name = name_edit->text().trimmed();
    if (new_name.isEmpty()) return;

    // Check if name changed and if it's unique
    if (new_name != current_selected_name) {
        if (!strategies_data.contains(new_name)) {
            // Update strategies_data with new name
            auto data = strategies_data.take(current_selected_name);
            data.name = new_name;
            strategies_data.insert(new_name, data);
            
            // Update sidebar item
            strategy_list->currentItem()->setText(new_name);
            
            // Update active_strategy_name if it was this one
            if (current_selected_name == active_strategy_name) {
                active_strategy_name = new_name;
            }
            
            current_selected_name = new_name;
        }
    }

    auto& data = strategies_data[current_selected_name];
    data.type = type_combo->currentText();
    data.deal_limit = limit_spin->value();
    data.default_market = market_combo->currentText();
}

void StrategyManagerDialog::onSetActive() {
    QListWidgetItem* current = strategy_list->currentItem();
    if (!current) return;

    // Remove bold from old active item
    for (int i = 0; i < strategy_list->count(); ++i) {
        QListWidgetItem* item = strategy_list->item(i);
        item->setFont(QFont(item->font().family(), -1, QFont::Normal));
    }

    active_strategy_name = current->text();
    current->setFont(QFont(current->font().family(), -1, QFont::Bold));
    set_active_btn->setEnabled(false);
}

void StrategyManagerDialog::onAddStrategy() {
    QString base_name = "New Strategy";
    QString name = base_name;
    int counter = 1;
    while (strategies_data.contains(name)) {
        name = QString("%1 %2").arg(base_name).arg(counter++);
    }

    StrategyData sd;
    sd.name = name;
    sd.type = "Scalping";
    sd.deal_limit = 3;
    sd.default_market = "MOEX Shares";
    
    strategies_data.insert(name, sd);
    
    QListWidgetItem* item = new QListWidgetItem(name, strategy_list);
    strategy_list->setCurrentItem(item);
    name_edit->setFocus();
    name_edit->selectAll();
}

void StrategyManagerDialog::onRemoveStrategy() {
    QListWidgetItem* item = strategy_list->currentItem();
    if (!item) return;

    QString name = item->text();
    auto res = QMessageBox::question(this, "Confirm Delete", QString("Are you sure you want to delete '%1'?").arg(name));
    if (res == QMessageBox::Yes) {
        strategies_data.remove(name);
        delete item;
        if (name == active_strategy_name) {
            active_strategy_name = strategy_list->count() > 0 ? strategy_list->item(0)->text() : "";
            if (!active_strategy_name.isEmpty()) {
                 strategy_list->item(0)->setFont(QFont(strategy_list->item(0)->font().family(), -1, QFont::Bold));
            }
        }
    }
}

QMap<QString, StrategyManagerDialog::StrategyData> StrategyManagerDialog::getSelectedStrategies() const {
    return strategies_data;
}

QString StrategyManagerDialog::getActiveStrategyName() const {
    return active_strategy_name;
}
