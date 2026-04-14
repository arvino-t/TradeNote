#include "terminal_window.h"
#include "resources/icons.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

TerminalWindow::TerminalWindow(QWidget* parent) : QDialog(parent) {
    setWindowTitle("T-Bank Terminal (Browser not available)");
    setWindowIcon(get_app_icon());
    resize(400, 200);
    setWindowModality(Qt::ApplicationModal);

    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* info_label = new QLabel("QWebEngine is not available in this environment.\nBrowser integration is disabled.", this);
    info_label->setAlignment(Qt::AlignCenter);
    layout->addWidget(info_label);

    QPushButton* close_btn = new QPushButton("Close", this);
    connect(close_btn, &QPushButton::clicked, this, &QDialog::reject);
    layout->addWidget(close_btn);
}

void TerminalWindow::onImportClicked() {}

QVariantList TerminalWindow::getImportedDeals() const {
    return imported_deals;
}
