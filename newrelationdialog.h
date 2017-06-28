#ifndef NEWRELATIONDIALOG_H
#define NEWRELATIONDIALOG_H

#include "ui_addcoupledialog.h"
#include "ui_addrelationdialog.h"
#include "QtWidgets"
#include "notesmanager.h"


/*! \class NewRelationDialog
 * \brief [Inherited from QDialog]  Class that defines the window to add a Relation.
 *
 * Checks if the Relation is already present before adding it in.
 */
class NewRelationDialog : public QDialog
{
    Q_OBJECT
public:
    NewRelationDialog();
    ~NewRelationDialog();

public slots:
    void submitButtonClicked();

private:
    Ui::addrelationdialog ui; /*!< The Qt UI that was built with Qt Designer. */
};

/*! \class NewCoupleDialog
 * \brief [Inherited from QDialog] Class that defines the window to add a Couple to a Relation.
 *
 * Retrieve all the Relations and Notes in order for the user to create a new Couple.
 * Checks if the Couple is already present (with regard to the orientation of the Relation selected) before adding it in.
 * The Relation 'Référence' isn't proposed since its lives throught direct références in texts.
 */
class NewCoupleDialog : public QDialog
{
    Q_OBJECT
public:
    NewCoupleDialog();
    ~NewCoupleDialog();

public slots:
    void submitButtonClicked();

private:
    Ui::addcoupledialog ui; /*!< The Qt UI that was built with Qt Designer. */
};

#endif // NEWRELATIONDIALOG_H
