#ifndef RELATIONTREEVIEW_H
#define RELATIONTREEVIEW_H

#include "relation.h"
#include "notesmanager.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QTreeWidgetItemIterator>
#include <QDebug>

/**
 * \class RelationTreeView
 * \brief [Inherited from QTreeWidget] Class that defines the treeView on the right part of the interface
 *
 *
 */

class RelationTreeView : public QTreeWidget
{
   Q_OBJECT

public:
   QString currentId(); /*!< Returns the m_id of the class object */
   RelationTreeView(QWidget* parent,const QString& id); /*!< Constructor of the class */
   void setId(const QString& id); /*!< Sets the m_id of the class object*/


public slots:
   void itemhasChanged(); /*!< Checks if the item selected has been changed */

signals:
   void selectIdSignal(const QString& id); /*!< Emitted if the slot itemhasChanged is trigerred */

private:
   //QTreeWidgetItem to fill the tree
   QTreeWidgetItem *Desc; /*!< Descendant notes of the current note in a given relation */
   QTreeWidgetItem *Asc; /*!< Ascendant notes of the current note in a given relation */
   QTreeWidgetItem *relItem;  /*!< Relation item in the tree widget */
   QTreeWidgetItem *noteToAdd; /*!< Item used to add all the childs */

   QString m_id; /*!< Refers to the current active Note ID */


   void addRelationToTree(); /*!< Adds the relations and its couples to the tree */
   NotesManager& manager;  /*!< Instance of the NotesManager */
};

#endif // RELATIONTREEVIEW_H
