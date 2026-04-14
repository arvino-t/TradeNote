#include "themes.h"

namespace themes {
    const QString JETBRAINS_DARK_STYLESHEET = R"(
QMainWindow, QDialog, QTabWidget, QScrollArea {
    background-color: #2b2b2b;
    color: #a9b7c6;
}

QFrame#MetricCard, QFrame#StatisticsGroup {
    background-color: #323232;
    border-radius: 8px;
    border: 1px solid #3c3f41;
}

QProgressBar {
    background-color: #3c3f41;
    border: 1px solid #2b2b2b;
    border-radius: 4px;
    text-align: center;
    color: white;
}

QTabBar::tab {
    background-color: #3c3f41;
    color: #bbbbbb;
    padding: 8px;
    border: 1px solid #2b2b2b;
}

QTabBar::tab:selected {
    background-color: #4e5254;
    color: #ffffff;
}

QLabel {
    color: #a9b7c6;
}

QPushButton {
    background-color: #365880;
    color: #ffffff;
    border: 1px solid #4b4b4b;
    padding: 5px;
    border-radius: 2px;
}

QPushButton:hover {
    background-color: #436995;
}

QPushButton:pressed {
    background-color: #2b4561;
}

QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox, QFontComboBox {
    background-color: #323232;
    color: #a9b7c6;
    border: 1px solid #646464;
    padding: 3px;
    selection-background-color: #214283;
}

QMenuBar {
    background-color: #3c3f41;
    color: #bbbbbb;
}

QMenuBar::item:selected {
    background-color: #4b4d4d;
}

QMenu {
    background-color: #3c3f41;
    color: #bbbbbb;
    border: 1px solid #2b2b2b;
}

QMenu::item:selected {
    background-color: #4b4d4d;
}

QStatusBar {
    background-color: #3c3f41;
    color: #909090;
}

QAbstractItemView {
    background-color: #323232;
    color: #a9b7c6;
    selection-background-color: #214283;
    gridline-color: #4b4b4b;
}

QHeaderView::section {
    background-color: #3c3f41;
    color: #a9b7c6;
    padding: 4px;
    border: 1px solid #2b2b2b;
}
)";

    const QString DRACULA_STYLESHEET = JETBRAINS_DARK_STYLESHEET;

    const QString SOLARIZED_DARK_STYLESHEET = R"(
QMainWindow, QDialog, QTabWidget, QScrollArea {
    background-color: #002b36;
    color: #839496;
}

QFrame#MetricCard, QFrame#StatisticsGroup {
    background-color: #073642;
    border-radius: 8px;
    border: 1px solid #586e75;
}

QProgressBar {
    background-color: #073642;
    border: 1px solid #002b36;
    border-radius: 4px;
    text-align: center;
    color: #eee8d5;
}

QTabBar::tab {
    background-color: #073642;
    color: #839496;
    padding: 8px;
    border: 1px solid #002b36;
}

QTabBar::tab:selected {
    background-color: #586e75;
    color: #eee8d5;
}

QLabel {
    color: #839496;
}

QPushButton {
    background-color: #268bd2;
    color: #eee8d5;
    border: 1px solid #073642;
    padding: 5px;
    border-radius: 2px;
}

QPushButton:hover {
    background-color: #2aa198;
}

QPushButton:pressed {
    background-color: #21669e;
}

QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox, QFontComboBox {
    background-color: #073642;
    color: #839496;
    border: 1px solid #586e75;
    padding: 3px;
    selection-background-color: #586e75;
}

QMenuBar {
    background-color: #073642;
    color: #839496;
}

QMenuBar::item:selected {
    background-color: #586e75;
}

QMenu {
    background-color: #073642;
    color: #839496;
    border: 1px solid #002b36;
}

QMenu::item:selected {
    background-color: #586e75;
}

QStatusBar {
    background-color: #073642;
    color: #586e75;
}

QAbstractItemView {
    background-color: #073642;
    color: #839496;
    selection-background-color: #586e75;
    gridline-color: #586e75;
}

QHeaderView::section {
    background-color: #073642;
    color: #839496;
    padding: 4px;
    border: 1px solid #002b36;
}
)";

    const QString GRUVBOX_STYLESHEET = R"(
QMainWindow, QDialog, QTabWidget, QScrollArea {
    background-color: #282828;
    color: #ebdbb2;
}

QFrame#MetricCard, QFrame#StatisticsGroup {
    background-color: #3c3836;
    border-radius: 8px;
    border: 1px solid #504945;
}

QProgressBar {
    background-color: #3c3836;
    border: 1px solid #282828;
    border-radius: 4px;
    text-align: center;
    color: #fbf1c7;
}

QTabBar::tab {
    background-color: #3c3836;
    color: #ebdbb2;
    padding: 8px;
    border: 1px solid #282828;
}

QTabBar::tab:selected {
    background-color: #504945;
    color: #fbf1c7;
}

QLabel {
    color: #ebdbb2;
}

QPushButton {
    background-color: #458588;
    color: #fbf1c7;
    border: 1px solid #3c3836;
    padding: 5px;
    border-radius: 2px;
}

QPushButton:hover {
    background-color: #83a598;
}

QPushButton:pressed {
    background-color: #076678;
}

QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox, QFontComboBox {
    background-color: #3c3836;
    color: #ebdbb2;
    border: 1px solid #665c54;
    padding: 3px;
    selection-background-color: #504945;
}

QMenuBar {
    background-color: #3c3836;
    color: #ebdbb2;
}

QMenuBar::item:selected {
    background-color: #504945;
}

QMenu {
    background-color: #3c3836;
    color: #ebdbb2;
    border: 1px solid #282828;
}

QMenu::item:selected {
    background-color: #504945;
}

QStatusBar {
    background-color: #3c3836;
    color: #928374;
}

QAbstractItemView {
    background-color: #3c3836;
    color: #ebdbb2;
    selection-background-color: #504945;
    gridline-color: #504945;
}

QHeaderView::section {
    background-color: #3c3836;
    color: #ebdbb2;
    padding: 4px;
    border: 1px solid #282828;
}
)";
}
