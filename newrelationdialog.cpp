#include "newrelationdialog.h"


NewCoupleDialog::NewCoupleDialog()
{
    /*! Constructor of the dialog used to add a new couple in a Relation.*/
    ui.setupUi(this);
    NotesManager& manager = NotesManager::getInstance();

    // We had the Notes' id to the combo in order for the user to choose which couple to create
    QStringList listIDNotes;
    for(NotesManager::const_iteratorNote itN = manager.cbeginNote(); itN != manager.cendNote();itN++){
        if ((*itN)->isEditable()){ // We just propose the Notes that are editable
            listIDNotes.append((*itN)->getId());
        }
    }
    ui.comboNote1->addItems(listIDNotes);
    ui.comboNote2->addItems(listIDNotes);

    // We do the same for the Relations
    QStringList listNameRelations;
    for(NotesManager::const_iteratorRelation itR = manager.cbeginRelation(); itR != manager.cendRelation();itR++){
        QString currentRelationName = (*itR)->getName();
        if (currentRelationName != "Référence")
            listNameRelations.append((*itR)->getName());
    }
    ui.comboRelation->addItems(listNameRelations);

    connect(ui.submitButton, SIGNAL(clicked(bool)), this, SLOT(submitButtonClicked()));
}



NewCoupleDialog::~NewCoupleDialog()
{
}

void NewCoupleDialog::submitButtonClicked()
{
     /*! Get the data selected by the user and adds the Couple to the Relation if it's not already added */

    QString relationName = ui.comboRelation->currentText();

    QString idNote1 = ui.comboNote1->currentText();
    QString idNote2 = ui.comboNote2->currentText();

    QString label = ui.labelEdit->text();

    NotesManager& manager = NotesManager::getInstance();

    Relation * relation = manager.findRelation(relationName);
    Note * note1 = manager.findNote(idNote1);
    Note * note2 = manager.findNote(idNote2);

    if (relation->getCouple(note1,note2)){
        ui.notificationLabel->setText(QString("Ce couple existe !"));
    }
    else {
        ui.notificationLabel->setText(QString::null);
        relation->createCouple(note1,note2,label);
    this->close();
    }
}


NewRelationDialog::NewRelationDialog()
{
    /*! Constructor of the dialog used to add a create a new Relation.*/
    ui.setupUi(this);
    connect(ui.submitButton, SIGNAL(clicked(bool)), this, SLOT(submitButtonClicked()));

    this->close();
}


NewRelationDialog::~NewRelationDialog()
{
}

void NewRelationDialog::submitButtonClicked()
{
    /*! Get the data selected by the user and creates a Relation if one is not already existing */

    QString name = ui.nameEdit->text();
    QString desc = ui.descEdit->toPlainText();
    bool isOriented = !(Qt::Unchecked == ui.orientedCheckBox->checkState());
    NotesManager& manager = NotesManager::getInstance();
    if (manager.findRelation(name)){
        ui.notificationLabel->setText(QString("%1 existe !").arg(name));
    }
    else {
        ui.notificationLabel->setText(QString::null);
        manager.createRelation(name,desc,isOriented);
    this->close();
    }
}
