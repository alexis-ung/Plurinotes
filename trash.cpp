#include "trash.h"

const QString DATEFORMAT = "yyyy-MM-dd hh:mm:ss";


Trash::Trash() : m(NotesManager::getInstance())
{
    /*! Displays a list of all the notes in the bin */

    listBin = new QListWidget();

    layout = new QHBoxLayout;

    layout->addWidget(listBin);

    setLayout(layout);

    //Load a strategy before the call of showData
    m_interface = new ArticleStrategy();

    //Find the selected note
    for(itNotes = m.beginNote(); itNotes!=m.endNote();itNotes++)
    {
        if((*itNotes)->getState() == dustbin)
        {
            listBin->addItem((*itNotes)->getId());
            binNotes.append(*itNotes);
        }
    }

    connect(listBin, SIGNAL(currentRowChanged(int)), this, SLOT(showData(int)));


}

void Trash::showData(int row)
{
    /*! Show the data of the note selected in the list */

    InterfaceStrategy *old = m_interface;

    // Load the data of the selected version in the widget
    switch ((*(binNotes.begin()+row))->getType())
    {
       case ArticleType:
       {
            Article *a = dynamic_cast<Article*>((*(binNotes.begin()+row))->getLastversion());
            m_interface = new ArticleStrategy((*(binNotes.begin()+row))->getId(), (*(binNotes.begin()+row))->getTitle(),
                                              a->getText(),(*(binNotes.begin()+row))->getCreationDateTime().toString(DATEFORMAT), a->getModifDate().toString(DATEFORMAT));
            break;
       }
       case MediaType:
       {
            Media *med = dynamic_cast<Media*>((*(binNotes.begin()+row))->getLastversion());
            m_interface = new MediaStrategy((*(binNotes.begin()+row))->getId(), (*(binNotes.begin()+row))->getTitle(),
                                            med->getDescription(), med->getFileName(),(*(binNotes.begin()+row))->getCreationDateTime().toString(DATEFORMAT),
                                            med->getModifDate().toString(DATEFORMAT));
            break;
       }
       case TaskType:
       {
            Task *t = dynamic_cast<Task*>((*(binNotes.begin()+row))->getLastversion());
            m_interface = new TaskStrategy((*(binNotes.begin()+row))->getId(), (*(binNotes.begin()+row))->getTitle(),
                                           t->getAction(), t->getPriority(), t->getDeadLine(), (*(binNotes.begin()+row))->getCreationDateTime().toString(DATEFORMAT),
                                           t->getModifDate().toString(DATEFORMAT));
            break;
       }
       case EmptyType:
       {
           throw NoteException("Trash::Trash : EmptyType in m_type");
           break;
       }
       default:
       {
           throw NoteException("Trash::Trash : no type");
           break;
       }
    }

    delete old;


    //Connect the submit button to restore version
    connect(m_interface, SIGNAL(submitButtonSignal()), this, SLOT(restoreNote()));

    //Load the restore interface
    m_interface->loadRestoreInteface();

    layout->addWidget(m_interface);
}

void Trash::restoreNote()
{
    /*! Restore the selected note */
    m.changeState((*(binNotes.begin()+listBin->currentRow()))->getId(), active);
//    (*(binNotes.begin()+listBin->currentRow()))->setState(active);
    this->done(0);
}

Trash::~Trash()
{

}

