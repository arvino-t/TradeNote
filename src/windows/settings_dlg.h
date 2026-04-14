#ifndef SETTINGS_DLG_H
#define SETTINGS_DLG_H

#include <QDialog>
#include <QComboBox>
#include <QFontComboBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    void setSelectedSettings(const QString& theme, const QFont& font, const QString& language, const QString& data_dir);
    QVariantMap getSelectedSettings() const;

private:
    QComboBox* theme_combo;
    QFontComboBox* font_combo;
    QComboBox* lang_combo;
    QLineEdit* data_dir_line;
};

#endif // SETTINGS_DLG_H
