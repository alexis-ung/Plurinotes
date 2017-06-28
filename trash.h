#ifndef TRASH_H
#define TRASH_H

#include <QtWidgets>
#include "notesmanager.h"
#include "notewid.h"


/**
 * \class Trash
 * \brief [Inherited from QDialog] Class that display the Notes present in the bin.
 *
 *
 */
class Trash : public QDialog
{

    Q_OBJECT

public:
    Trash();
    ~Trash();

public slots:
    void showData(int row);
    void restoreNote(); /*!< Restore the selected Note in the bin. */

private:
    //Interface
   InterfaceStrategy *m_interface;  /*!< InterfaceStrategy of the current Note displayed. */

    //Instance of the manager
    NotesManager &m;  /*!< Instance of the NotesManager */

    //Iterator
    NotesManager::iteratorNote itNotes; /*!< Iterator on Notes */

    //Display
    QHBoxLayout *layout; /*!< Layout for the bin */
    QListWidget *listBin; /*!< To Display the Notes in the bin. */

    //List of notes
    QList<Note*> binNotes;  /*!< The list of the Notes in the bin. */
};

#endif // TRASH_H
