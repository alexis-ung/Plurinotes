#include "relationtreeview.h"


QString RelationTreeView::currentId()
{
    return m_id;
}

RelationTreeView::RelationTreeView(QWidget *parent, const QString &id): QTreeWidget(parent),m_id(id),manager(NotesManager::getInstance())
{
    this->setColumnCount(2);
    QStringList ColumnNames;
    ColumnNames<<"Nom"<<"Description";
    this->setHeaderLabels(ColumnNames);
    QTreeView::setSortingEnabled(true);
    addRelationToTree();
    this->sortByColumn(1);
    connect(this,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(itemhasChanged()));
}


void RelationTreeView::setId(const QString &id)
{
    m_id = id;
    addRelationToTree();
}

void RelationTreeView::itemhasChanged()
{
    if (manager.findNote(currentItem()->text(0)) != nullptr)
    {
        emit selectIdSignal(currentItem()->text(0));

    }
}



void RelationTreeView::addRelationToTree()
{
    this->clear();
    NotesManager& manager = NotesManager::getInstance();
    NotesManager::const_iteratorRelation itR;
    for ( itR=manager.cbeginRelation(); itR!= manager.cendRelation(); itR++ )
    {
        relItem = new QTreeWidgetItem;
        const Relation *current = *itR;
        relItem->setText(0,current->getName());
        relItem->setText(1,current->getDescription());
        this->addTopLevelItem(relItem);
        if (current->isOriented())
        {
            Asc = new QTreeWidgetItem;
            Desc = new QTreeWidgetItem;
            Asc->setText(0,"Ascendants");
            Desc->setText(0,"Descendants");
            relItem->addChild(Asc);
            relItem->addChild(Desc);
            QSet<pair<QString,QString>> coupleSetAsc = manager.getCouplesWithNote(manager.findNote(m_id),current->getName(),false);
            QSet<pair<QString,QString>> coupleSetDesc = manager.getCouplesWithNote(manager.findNote(m_id),current->getName(),true);
            QSet<pair<QString,QString>>::const_iterator itS;
            for (itS = coupleSetAsc.cbegin(); itS != coupleSetAsc.cend() ; itS++)
            {
                noteToAdd = new QTreeWidgetItem;
                noteToAdd->setText(0,(*itS).first);
                noteToAdd->setText(1,(*itS).second);
                Asc->addChild(noteToAdd);
            }
            for (itS = coupleSetDesc.cbegin(); itS != coupleSetDesc.cend() ; itS++)
            {
                noteToAdd = new QTreeWidgetItem;
                noteToAdd->setText(0,(*itS).first);
                noteToAdd->setText(1,(*itS).second);
                Desc->addChild(noteToAdd);
            }
        }
        else
        {
            QSet<pair<QString,QString>> coupleSetAsc = manager.getCouplesWithNote(manager.findNote(currentId()),current->getName());
            QSet<pair<QString,QString>>::const_iterator itS;
            for (itS = coupleSetAsc.cbegin(); itS != coupleSetAsc.cend() ; itS++)
            {
                noteToAdd = new QTreeWidgetItem;
                noteToAdd->setText(0,(*itS).first);
                noteToAdd->setText(1,(*itS).second);
                relItem->addChild(noteToAdd);
            }

            QSet<pair<QString,QString>> coupleSetDesc = manager.getCouplesWithNote(manager.findNote(currentId()),current->getName(),false);
            for (itS = coupleSetDesc.cbegin(); itS != coupleSetDesc.cend() ; itS++)
            {
                noteToAdd = new QTreeWidgetItem;
                noteToAdd->setText(0,(*itS).first);
                noteToAdd->setText(1,(*itS).second);
                relItem->addChild(noteToAdd);
            }
        }
    }

    this->expandAll();
}
