#include "newnotedialog.h"

const QString DATEFORMAT = "yyyy-MM-dd hh:mm:ss";

NewNoteDialog::NewNoteDialog(const NoteType &type) : m_type(type),m(NotesManager::getInstance())
{
    /*! Creates an empty widget, with placeholders, correspondings with the type of note we want to had */
    setWindowTitle("Ajout d'une note");

    //Main layout
    QHBoxLayout *layout = new QHBoxLayout;


    //Define the correct interface
    switch(m_type)
    {
        case ArticleType:
        {
            m_interface = new ArticleStrategy();
            break;
        }
        case MediaType:
        {
            m_interface = new MediaStrategy();
            break;
        }
        case TaskType:
        {
            m_interface = new TaskStrategy();           
            break;
        }
        case EmptyType:
        {
            throw NoteException("NewNoteDialog::NewNoteDialog : Empty Type selectionned.");
            break;
        }
        default :
        {
            throw NoteException("NewNoteDialog::NewNoteDialog : no type selectionned.");
            break;
        }
    }

    //Load interface adapted to this window
    m_interface->loadAddingInterface();

    //Link submit button to send data
    connect(m_interface, SIGNAL(submitButtonSignal()), this, SLOT(sendData()));

    //Add the interface to the layout
    layout->addWidget(m_interface);

    //Set the main layout of this window
    setLayout(layout);
}

void NewNoteDialog::sendData()
{
    /*! Create a new note and a new version if the id is not already existing  */
    if(!m.isPresent(m_interface->getID()) && m_interface->getID() != "")
    {
        //Get the data written
        Dico mapToSend = m_interface->getData();
        if(m_type == TaskType)
        {
            QDateTime deadLine = mapToSend["deadLine"].toDateTime();
            if(deadLine < QDateTime::currentDateTime() && deadLine != QDateTime::fromString("2000-01-01 00:00:00",DATEFORMAT))
            {
                QMessageBox::warning(this, "Problème deadline", "la deadline est dans le passé");
                return;
            }
        }

        //Create the new note and create the first version
        Note *newlyCreated = m.createNote(mapToSend["id"].toString(), mapToSend["title"].toString(), m_type);
        newlyCreated->createVersion(mapToSend);

        //Close the dialog
        this->done(0);
    }
    else
    {
        QMessageBox::warning(this, "Ajout impossible", "ID invalide");
    }
}
