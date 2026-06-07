#ifndef ROUNDEDITEMDELEGATE_H
#define ROUNDEDITEMDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QPainterPath>

class RoundedItemDelegate : public QStyledItemDelegate
{
public:
    explicit RoundedItemDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        painter->save();

        if (option.state & QStyle::State_Selected)
        {
            QPainterPath path;
            QRect r = option.rect.adjusted(2, 2, -2, -2);
            path.addRoundedRect(r, 30, 30);
            painter->setRenderHint(QPainter::Antialiasing);
            painter->fillPath(path, QColor("#588BF4"));
        }

        QStyledItemDelegate::paint(painter, option, index);
        painter->restore();
    }
};

#endif // ROUNDEDITEMDELEGATE_H
