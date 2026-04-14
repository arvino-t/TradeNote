#include "settings_dlg.h"
#include "resources/icons.h"
#include <QVariantMap>

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("General Settings");
    setWindowIcon(get_app_icon());
    setFixedSize(400, 250);
    setWindowModality(Qt::ApplicationModal);

    QFormLayout* layout = new QFormLayout(this);

    theme_combo = new QComboBox(this);
    theme_combo->addItems({"Light", "JetBrains Dark", "Dracula", "Solarized Dark", "Gruvbox"});
    layout->addRow("Theme:", theme_combo);

    font_combo = new QFontComboBox(this);
    layout->addRow("Font:", font_combo);

    lang_combo = new QComboBox(this);
    lang_combo->addItems({"English", "Russian"});
    layout->addRow("Language:", lang_combo);

    QDialogButtonBox* button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addRow(button_box);
}

void SettingsDialog::setSelectedSettings(const QString& theme, const QFont& font, const QString& language) {
    theme_combo->setCurrentText(theme);
    font_combo->setCurrentFont(font);
    lang_combo->setCurrentText(language);
}

QVariantMap SettingsDialog::getSelectedSettings() const {
    QVariantMap settings;
    settings["theme"] = theme_combo->currentText();
    settings["font"] = font_combo->currentFont().family();
    settings["language"] = lang_combo->currentText();
    return settings;
}
