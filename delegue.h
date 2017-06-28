#ifndef DELEGUE_H
#define DELEGUE_H

 #include <QtWidgets>
#include "note.h"

/**
 * \class Delegue
 * \brief [Inherited from QStyledItemDelegate]  Class that defines how the cases are drawn in the table
 *
 *
 */
class Delegue : public QStyledItemDelegate
{
public:
    Delegue(QWidget *parent=0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // DELEGUE_H
