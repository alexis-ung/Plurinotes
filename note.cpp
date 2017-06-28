#include "note.h"
#include "notesmanager.h"
#include "datamanager.h"


using namespace std;

const QString REGEXPTOFINDID = "\\\\ref{\\w+}";

void Note::setTitle(QString title) {
    m_title = title;
    NotesManager::getInstance().getDataManager().saveNote(*this);
}

void Note::setState(NoteState s) {
    m_state = s;
    NotesManager::getInstance().getDataManager().saveNote(*this);
}

void Note::createVersion(Dico& dico, bool fromPersistentData) {
    /*! Creates and add a version to the Note object based on the type of this object
     * with the a Dico */

    // Since modifDateTime is a common attribute inherited from Version
    QDateTime modifDateTime;

    if(fromPersistentData) // Loading the version from the DataBase
        modifDateTime = dico["modifDateTime"].toDateTime();
    else // Case of a version from the application
        modifDateTime= QDateTime::currentDateTime();


    // The behavior of the method depends on the type of the note.
    switch (this->getType()) {
        case ArticleType:
        {
            QString text = dico["text"].toString();

            Article * newArticle = new Article(modifDateTime, text);

            m_versions.push_front(newArticle);
            break;
        }
        case MediaType:
        {
            QString description = dico["description"].toString();
            QString filename = dico["filename"].toString();

            Media * newMedia = new Media(modifDateTime, description,filename);

            m_versions.push_front(newMedia);
            break;
        }
        case TaskType:
        {
            QString action = dico["action"].toString();
            TaskStatus status = static_cast<TaskStatus>(dico["status"].toInt());
            uint priority =  dico["priority"].toInt();
            QDateTime deadLine = dico["deadLine"].toDateTime(); // toDateTime

            Task * newTask = new Task(modifDateTime, action,status,priority,deadLine);

            m_versions.push_front(newTask);
            break;
        }
        case EmptyType:
            throw NoteException("Note::createVersion() : EmptyType");
            break;
        default:
            throw NoteException("Note::createVersion() : type problem");
            break;
    }
    if (!fromPersistentData) {// If the version was created by the app,
        Version *newVersionJustCreated = getLastversion();
        // We save it in the via the datamanager
        NotesManager::getInstance().getDataManager().saveVersion(newVersionJustCreated,getId(),getType());
        // We update the couple of the relation Référence
        updateReferences();
    }
}

Version *Note::getLastversion()
{
    /*! Returns the last version of the note */
    if(!m_versions.isEmpty())
        return m_versions.front();
    return nullptr;
}

const Version *Note::getLastversion() const
{
    /*! Returns the last version of the note */
    if(!m_versions.isEmpty())
        return m_versions.front();
    return nullptr;
}


void Note::debugPrintVersions() const
{
    for (const_iterator i = cbegin(); i != cend(); ++i) {
        (*i)->debugPrintInfo();
    }
}

QSet<QString> getIDsFromText(const QString& stringToFindIdWithin)
{
    /*! Returns the list of ID (eg "foo") contained in the string in this way : "\id{foo}" */
    QRegularExpression re(REGEXPTOFINDID); // Our regex (see the header)
    // We use an iterator that contains all the id catched
    QRegularExpressionMatchIterator it = re.globalMatch(stringToFindIdWithin);

    QSet<QString> IDset; // the set of ID to return

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString idMatched = match.captured().remove("\\ref{").remove("}");

        IDset.insert(idMatched);
    }

    return IDset;
}

void Note::updateReferences() const
{
    /*! [Trigger] When called, updates the Référence relations in creating couples for new references and in deleting old references' couples.*/
    // We use set of string in order to find the couples to add and delete
    QSet<QString> newIdSet;

    // Getting IDs of other notes in text attributes
    newIdSet = getIDsFromText(getTitle());
    newIdSet = getLastversion()->parseData(newIdSet);

    // Compare the referenced notes
    NotesManager& manager = NotesManager::getInstance();
    QSet<QString> oldIdSet = manager.getReferencedNotes(this);

    // We copy the set in other to make set difference
    QSet<QString> idToDereference(oldIdSet);
    QSet<QString> idToReference(newIdSet);

    // The couples will be deleted thanks idToDereference ; the ones to be built are going to use idToReference
    idToDereference -= newIdSet;
    idToReference -= oldIdSet;

    // Removing QString::null from the sets.
    idToReference.remove("");
    idToDereference.remove("");

    // Hypothesis : a note doesn't references itself
    idToReference.remove(this->getId());


    // We catch the reference relation
    Relation * referenceRelation = manager.findRelation("Référence");

    Couple * currentCouple = nullptr;
    Note * noteToHandle = nullptr;

    // We create the new couples
    for(QSet<QString>::iterator it = idToReference.begin(); it != idToReference.end(); it++){
        noteToHandle = manager.findNote(*it);
        if (noteToHandle){
            referenceRelation->createCouple(this,noteToHandle);
        }
    }

    // In order to ask to delete notes if there are no more referenced
    QSet<QString> setNoteToAskDelete;

    // We delete the old couples
    for(QSet<QString>::iterator it = idToDereference.begin(); it != idToDereference.end(); it++){
        noteToHandle = manager.findNote(*it);
        currentCouple = referenceRelation->getCouple(this,noteToHandle);
        if(noteToHandle->getState() == archive && (manager.getNotesThatReference(noteToHandle)).size()== 1){
            // If so, then the last one couple is the one to delete and we can ask to delete the note
            setNoteToAskDelete << noteToHandle->getId();
        }
        referenceRelation->deleteCouple(currentCouple);
    }
    qDebug() << setNoteToAskDelete.size();
    for(QSet<QString>::iterator it = setNoteToAskDelete.begin(); it != setNoteToAskDelete.end(); it++) {
        QMessageBox::StandardButton askDeleteNote;
        QWidget widgetToDel;
        askDeleteNote = QMessageBox::question(&widgetToDel, "Suppression sur déréférencement", QString("La note %1 n'est plus référencée ; voulez-vous la supprimer ?").arg(*it), QMessageBox::Yes|QMessageBox::No);

        if(askDeleteNote == QMessageBox::Yes)
        {
            manager.deleteNote(*it);
        }
    }
}



void Article::debugPrintInfo() const
{
    /*! Polymorphic method : display the info stored in the article */
    qDebug() << " - Text : " << getText();
}

QSet<QString> Article::parseData(QSet<QString> idSet) const
{
    /*! Polymorphic method : returns the list of the IDs contained in the text of the article. */
    idSet += getIDsFromText(getText());
    return idSet;
}

void Media::debugPrintInfo() const
{
    /*! Polymorphic method : display the info stored in the media */
    qDebug() << " - Description : " << getDescription();
    qDebug() << " - Filename : " << getFileName();
}

QSet<QString> Media::parseData(QSet<QString> idSet) const
{
    /*! Polymorphic method : returns the list of the IDs contained in the description and in the filename of the media. */
    idSet += getIDsFromText(getDescription());
    idSet += getIDsFromText(getFileName());
    return idSet;
}



void Task::debugPrintInfo() const
{
    /*! Polymorphic method : display the info stored in the task */
    qDebug() << " - Action : " << getAction();
    qDebug() << " - Status : " << getStatus();
    qDebug() << " - Priority : " << getPriority();
    qDebug() << " - DeadLine : " << getDeadLine().toString();

}

QSet<QString> Task::parseData(QSet<QString> idSet) const
{
    /*! Polymorphic method : returns the list of the IDs contained in the description and in the action of the task. */
    idSet += (getIDsFromText(getAction()));
    return idSet;
}
