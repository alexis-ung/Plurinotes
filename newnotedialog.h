#ifndef NEWNOTEDIALOG_H
#define NEWNOTEDIALOG_H

#include <QtWidgets>
#include "notewid.h"
#include "notesmanager.h"

/**
 * \class NewNoteDialog
 * \brief [Inherited from QDialog]  Class that defines the window to add a note
 *
 * Displays the widgets to ask for the data of the new note
 * and send it to the NoteManager
 *
 */
class NewNoteDialog : public QDialog
{
    /* Définition de la fenêtre de dialogue lors de la création d'une nouvelle note */
    Q_OBJECT
public:
    NewNoteDialog(const NoteType &type);

public slots:
    void sendData();

private:
    // Interfaces
    InterfaceStrategy *m_interface; /*!< The InterfaceStrategy used for the NewNoteDialog */
    NoteType m_type; /*!< The type of the Note to create */
    NotesManager &m; /*!< Instance of the NotesManager */

};

#endif // NEWNOTEDIALOG_H
