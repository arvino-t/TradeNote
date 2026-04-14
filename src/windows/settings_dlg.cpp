#include "settings_dlg.h"
#include "resources/icons.h"
#include <QVariantMap>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("General Settings");
    setWindowIcon(get_app_icon());
    setFixedSize(500, 300);
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

    data_dir_line = new QLineEdit(this);
    QPushButton* browse_btn = new QPushButton("Browse...", this);
    connect(browse_btn, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Strategies Directory", data_dir_line->text());
        if (!dir.isEmpty()) {
            data_dir_line->setText(dir);
        }
    });

    QHBoxLayout* dir_layout = new QHBoxLayout();
    dir_layout->addWidget(data_dir_line);
    dir_layout->addWidget(browse_btn);
    layout->addRow("Data Directory:", dir_layout);

    QDialogButtonBox* button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addRow(button_box);
}

void SettingsDialog::setSelectedSettings(const QString& theme, const QFont& font, const QString& language, const QString& data_dir) {
    theme_combo->setCurrentText(theme);
    font_combo->setCurrentFont(font);
    lang_combo->setCurrentText(language);
    data_dir_line->setText(data_dir);
}

QVariantMap SettingsDialog::getSelectedSettings() const {
    QVariantMap settings;
    settings["theme"] = theme_combo->currentText();
    settings["font"] = font_combo->currentFont().family();
    settings["language"] = lang_combo->currentText();
    settings["data_dir"] = data_dir_line->text();
    return settings;
}
