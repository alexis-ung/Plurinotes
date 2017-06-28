#ifndef RELATIONVIEW_H
#define RELATIONVIEW_H

#include <QWidget>
#include <QComboBox>
#include <QTableWidget>
#include <QStringList>
#include "notesmanager.h"

/**
 * \class RelationView
 * \brief [Inherited from QWidget] Class that defines the view of the couples of a given relation
 *
 * Using ComboBox to select the relation and QTableWidget to display the couples
 */


class RelationView : public QWidget
{
    Q_OBJECT
public:
    RelationView(QWidget *parent=0); /*!< Constructor of the class*/

    const QString getCurrentRelation() {return relationCombo->currentText();} /*!< Get the selected relation in the ComboBox*/
    void addCoupleRelation(const QString &r); /*!< Adds the couples of the given relation*/
    const QString& getRelation(){return m_relation;} /*!< Return the name of the relation selected */
signals:
    void selectedRelSignal(); /*!< Emitted if the slot selectedRelChanged is trigerred*/
    void selectIdSignal(const QString& id); /*!< Emitted if the slot itemhasChanged is trigerred*/
public slots:
    void selectedRelChanged(QString r); /*!< Trigerred if the relation selected in the ComboBox changes*/
    void itemhasChanged(); /*!< Trigerred if the item selected in the QTableView changes*/

private:

    QString m_relation; /*!< Name of the relation selected. */
    NotesManager& manager;  /*!< Name of the relation selected. */

    //Interface widgets and layout
    QComboBox *relationCombo;  /*!< The combo to select the relation. */
    QTableWidget *coupleTab; /*!< The widget to show the couples involved in the relations. */
    QVBoxLayout *layout; /*!< The layout containing the current widget to show.*/

};

#endif // RelationView_H
