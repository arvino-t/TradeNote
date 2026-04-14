#include "icons.h"
#include <QPixmap>
#include <QPainter>
#include <QFont>

QIcon get_app_icon() {
    QPixmap pixmap(256, 256);
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Ubuntu Orange background (rounded rectangle)
    painter.setBrush(QColor("#E95420"));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(0, 0, 256, 256, 40, 40);
    
    // White 'TN' text
    painter.setPen(Qt::white);
    QFont font("Ubuntu", 120, QFont::Bold);
    if (!font.exactMatch()) {
        font.setFamily("Arial");
    }
    painter.setFont(font);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, "TN");
    
    painter.end();
    return QIcon(pixmap);
}

QIcon get_priority_icon(Priority p) {
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor color;
    switch (p) {
        case Priority::Low: color = Qt::gray; break;
        case Priority::Medium: color = QColor("#FFD700"); break; // Gold/Yellow
        case Priority::High: color = Qt::red; break;
    }

    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(16, 16, 32, 32);

    painter.end();
    return QIcon(pixmap);
}
