#ifndef TERMINAL_WINDOW_H
#define TERMINAL_WINDOW_H

#include <QDialog>
#include <QVariantList>

class TerminalWindow : public QDialog {
    Q_OBJECT
public:
    explicit TerminalWindow(QWidget* parent = nullptr);
    QVariantList getImportedDeals() const;

private slots:
    void onImportClicked();

private:
    QVariantList imported_deals;
};

#endif // TERMINAL_WINDOW_H
