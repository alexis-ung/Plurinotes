#ifndef VERSIONDISPLAYER_H
#define VERSIONDISPLAYER_H

#include <QtWidgets>
#include "note.h"
#include "notesmanager.h"
#include "notewid.h"

/**
 * \class VersionDialog
 * \brief [Inherited from QDialog]   Class that defines the window to restore version
 *
 * Displays all the previous versions of a note and enables to restore them.
 *
 */


class VersionDialog : public QDialog
{
    Q_OBJECT
public:
    VersionDialog(const QString &idnote);

public slots:
    void showData(int row);
    void restoreVersion();

private:
    //Interface
    InterfaceStrategy *m_interface; /*!< The InterfaceStrategy used for the VersionDialog */

    //Instance of the manager
    NotesManager &m;  /*!< Instance of the NotesManager */

    //Iterators
    NotesManager::iteratorNote itNotes;  /*!< Iterator on Notes */
    Note::iterator itVersion;  /*!< Iterator on the Version of the Note */

    //Display
    QHBoxLayout *layout; /*!< The layout for the dialog */
    QListWidget *listVersions; /*!< The list of the Notes in the bin. */
};

#endif // VERSIONDISPLAYER_H
