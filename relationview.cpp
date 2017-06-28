#include "relationview.h"

RelationView::RelationView(QWidget *parent): QWidget(parent),manager(NotesManager::getInstance())
{
    relationCombo = new QComboBox;
    coupleTab = new QTableWidget;
    coupleTab->setColumnCount(3);
    QStringList ColumnNames;
    ColumnNames<<"Label"<<"Note 1"<<"Note 2";
    coupleTab->setHorizontalHeaderLabels(ColumnNames);
    coupleTab->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout = new QVBoxLayout;
    layout->addWidget(relationCombo);
    layout->addWidget(coupleTab);

    setLayout(layout);

    NotesManager::const_iteratorRelation itR;
    for (itR = manager.cbeginRelation(); itR != manager.cendRelation() ; itR++)
    {
        relationCombo->addItem((*itR)->getName());
    }
    m_relation = relationCombo->currentText();
    addCoupleRelation(m_relation);
    connect(relationCombo,SIGNAL(currentIndexChanged(QString)),this,SLOT(selectedRelChanged(QString)));
    connect(coupleTab,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(itemhasChanged()));
}


void RelationView::selectedRelChanged(QString r)
{
    addCoupleRelation(r);
}

void RelationView::itemhasChanged()
{
    if (manager.findNote(coupleTab->currentItem()->text()) != nullptr)
    {
        emit selectIdSignal(coupleTab->currentItem()->text());

    }
}

void RelationView::addCoupleRelation(const QString &r)
{
    m_relation = r;
    coupleTab->clearContents();
    for (int i=0; i < coupleTab->rowCount(); i++)
        coupleTab->removeRow(i);

    Relation* relation = manager.findRelation(r);
    Relation::const_iterator itC;
    int row = 0;
    for (itC = relation->cbegin(); itC != relation->cend(); itC++)
    {
        coupleTab->insertRow(row);
        QTableWidgetItem *coupleLabel = new QTableWidgetItem;
        QTableWidgetItem *coupleAsc = new QTableWidgetItem;
        QTableWidgetItem *coupleDesc = new QTableWidgetItem;

        coupleLabel->setText((*itC)->getLabel());
        coupleAsc->setText((*itC)->getIdAsc());
        coupleDesc->setText((*itC)->getIdDesc());

        coupleLabel->setFlags(coupleLabel->flags() & ~Qt::ItemIsEditable);
        coupleAsc->setFlags(coupleAsc->flags() & ~Qt::ItemIsEditable);
        coupleDesc->setFlags(coupleDesc->flags() & ~Qt::ItemIsEditable);

        coupleTab->setItem(row,0,coupleLabel);
        coupleTab->setItem(row,1,coupleAsc);
        coupleTab->setItem(row,2,coupleDesc);
        row++;
    }
}

