#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QtWidgets>
#include "notesmanager.h"
#include "note.h"
#include <QBrush>
#include <QFont>

/**
 * \class TableModel
 * \brief [Inherited from QAbstractTableModel] Class that defines which data is shown in the tableview
 *
 * Overrides pure virtual functions
 * in order to show and to edit the notes with the tableview
 *
 */
class TableModel : public QAbstractTableModel
{
public:
    TableModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    int indexNextTask(QList<Note*> list, int i) const;
private:
    NotesManager &m; /*!< Instance of the NotesManager */
};

#endif // TABLEMODEL_H
