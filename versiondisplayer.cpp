#include "versiondisplayer.h"

const QString DATEFORMAT = "yyyy-MM-dd hh:mm:ss";


VersionDialog::VersionDialog(const QString &idnote) : m(NotesManager::getInstance()), itNotes(m.beginNote()), itVersion((*itNotes)->begin())
{
    /*! Displays a list of all the versions of a note */
    setWindowTitle("Affichage des versions");

    listVersions = new QListWidget(this);

    layout = new QHBoxLayout(this);

    layout->addWidget(listVersions);

    setLayout(layout);

    //Load a strategy before the call of showData
    m_interface = new ArticleStrategy();

    //Find the selected note
    for(itNotes = m.beginNote(); itNotes!=m.endNote();itNotes++)
    {
        if((*itNotes)->getId() == idnote) // .toString not necessary?
        {
            break;
        }
    }

    //Skip the current version
    itVersion = (*itNotes)->begin();
    itVersion++;

    //Add all the versions to the list

    while(itVersion != (*itNotes)->end())
    {
        listVersions->addItem((*itVersion)->getModifDate().toString());
        itVersion++;
    }

    //Connect the listWidget in order to display the selected version
    connect(listVersions, SIGNAL(currentRowChanged(int)), this, SLOT(showData(int)));

}

void VersionDialog::showData(int row)
{
    /*! Show the data of the note selected in the list.
     * Is automatically called at the beginning with the row 0*/
    row++; //Skip the current version
    InterfaceStrategy *old = m_interface;


    // Load the data of the selected version in the widget
    switch ((*itNotes)->getType())
    {
       case ArticleType:
       {
            Article *a = dynamic_cast<Article*>(*((*itNotes)->begin()+row));
            m_interface = new ArticleStrategy((*itNotes)->getId(), (*itNotes)->getTitle(),
                                              a->getText(),(*itNotes)->getCreationDateTime().toString(DATEFORMAT), a->getModifDate().toString(DATEFORMAT));
            break;
       }
       case MediaType:
       {
           Media *med = dynamic_cast<Media*>(*((*itNotes)->begin()+row));
            m_interface = new MediaStrategy((*itNotes)->getId(), (*itNotes)->getTitle(),
                                            med->getDescription(), med->getFileName(),(*itNotes)->getCreationDateTime().toString(DATEFORMAT), med->getModifDate().toString(DATEFORMAT));
            break;
       }
       case TaskType:
       {
           Task *t = dynamic_cast<Task*>(*((*itNotes)->begin()+row));
            m_interface = new TaskStrategy((*itNotes)->getId(), (*itNotes)->getTitle(),
                                           t->getAction(), t->getPriority(), t->getDeadLine(), (*itNotes)->getCreationDateTime().toString(DATEFORMAT), t->getModifDate().toString(DATEFORMAT));
            break;
       }
       case EmptyType:
       {
           throw NoteException("NewNoteDialog::NewNoteDialog : EmptyType in m_type");
           break;
       }
       default:
       {
           throw NoteException("NewNoteDialog::NewNoteDialog : no type");
           break;
       }
    }

    delete old;

    //Connect the submit button to restore version
    connect(m_interface, SIGNAL(submitButtonSignal()), this, SLOT(restoreVersion()));

    //Load the restore interface
    m_interface->loadRestoreInteface();

    layout->addWidget(m_interface);

}

void VersionDialog::restoreVersion()
{
    /*! Restore the selected note */
    //Create a new version with an old one
    Dico d = m_interface->getData();
    (*itNotes)->createVersion(d);

    //Close the dialog
    this->done(0);
}
