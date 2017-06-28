#include "notesmanager.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

NotesManager::Handler NotesManager::handler=Handler();

NotesManager::NotesManager() : nbArticle(0), nbMedia(0),  nbTask(0)
{
    dataManager = &(SQLiteManager::getInstance());
}


NotesManager &NotesManager::getInstance()
{
    /*! Returns the unique instance of the NotesManager. */
    if (!handler.instance){
        handler.instance = new NotesManager;
        handler.instance->getDataManager().loadNotesAndVersions();
        handler.instance->getDataManager().loadRelationsAndCouples();
    }
    return *handler.instance;
}

void NotesManager::freeManager()
{
    /*! Deletes the unique instance of the NotesManager. */
    delete handler.instance;
    handler.instance=nullptr;
}


void NotesManager::debugPrintNotes()
{
    /*! Prints all the info stored in the notes. */
    const Note * current = nullptr;
    for (NotesManager::const_iteratorNote i = cbeginNote(); i != cendNote(); ++i){
        current = *i;
        qDebug() << current->getId() << "  ; " << current->getTitle() << "  ; " << current->getType() << "\n";
        current->debugPrintVersions();
    }
}

void NotesManager::debugPrintRelations()
{
    /*! Prints all the info stored in the relations. */
    ;
    const Relation * current = nullptr;
    for (NotesManager::const_iteratorRelation i = cbeginRelation(); i != cendRelation(); ++i){
        current = *i;
        qDebug() << current->getName() << "  ; " << current->getDescription() << "\n";
        current->debugPrintCouples();
    }
}


Note *NotesManager::createNote(const QString &id, const QString &title, const NoteType type, const QDateTime& creationDateTime, const NoteState& state, bool saveInDB)
{
    /*! Creates a new note ; adds it to the NotesManager and to the dataManager.
    creationDatetime is set by the default as the current DateTime and state to active */
    Note * newNote = new Note(id,title,type,creationDateTime,state);
    m_notes[id] = newNote;
    // Is the Note is editable, we have to incremente the number of its type for the table view
    if (newNote->isEditable()) {
        switch(newNote->getType()){
        case ArticleType:
            nbArticle++;
            break;
        case MediaType:
            nbMedia++;
            break;
        case TaskType:
            nbTask++;
            break;
        case EmptyType:
        default:
            throw NoteException("NotesManager::createNote : bad type");
        }
    }

    if (saveInDB)
        dataManager->saveNote(*newNote,true); //toInsert set to True to insert in DB
    return newNote;
}

Note *NotesManager::findNote(const QString &id)
{
    /*! Returns a pointer on the Note of ID id ; nullptr if there isn't. */
    DicoNotes::iterator it = m_notes.find(id);
    if(it==m_notes.end()) //No Note with this id
        return nullptr;

    return it.value();
}

void NotesManager::deleteNote(const QString &id)
{
    /*! Deletes or achives the note identified by the id :
     *  - If the note is referenced, we just archive it ;
     *  - otherwise, we delete all its couples in all the relations and we set it in the bin. */

    Note * noteToDelete = findNote(id);
    if(!noteToDelete){
        throw NoteException("NotesManager::deleteNote : Note of id not present in the application.");
    }
    if(noteToDelete->getState() == active)
    {
        switch(noteToDelete->getType())
        {
        case ArticleType:
            nbArticle--;
            break;
        case MediaType:
            nbMedia--;
            break;
        case TaskType:
            nbTask--;
            break;
        case EmptyType:
        default:
            throw NoteException("NotesManager::deleteNote : bad type");
        }
    }

    NoteState currentState = noteToDelete->getState();
    if(currentState == dustbin) {
        return;
    }

    bool isReferenced = !(getNotesThatReference(noteToDelete).isEmpty());

    if (isReferenced){
        noteToDelete->toArchive();
    }
    else {
        for(iteratorRelation itR = beginRelation(); itR != endRelation() ; itR++){
            Relation::iterator itC = (*itR)->begin();
            while(itC != (*itR)->end()) {
                if ((*itC)->getAsc() == noteToDelete || (*itC)->getDesc()== noteToDelete){
                    itC = (*itR)->deleteCouple(*itC);
                }
                else{
                    itC++;
                }
            }
        }
        noteToDelete->toBin();
    }
    // We update the field 'state' in via the dataManager
    dataManager->saveNote(*noteToDelete);
}

void NotesManager::changeState(const QString &id, const NoteState &state)
{
    /*! Change the state of a given Note by the specific state*/
    Note *n = findNote(id);
    if (n == nullptr){
        throw NoteException("NotesManager::changeState : note isn't present.");
    }
    n->setState(state);
    switch(n->getType())
    {
    case ArticleType:
        state ? nbArticle-- : nbArticle++;
        break;
    case MediaType:
        state ? nbMedia-- : nbMedia++;
        break;
    case TaskType:
        state ? nbTask-- : nbTask++;
        break;
    case EmptyType:
    default:
        throw NoteException("NotesManager::changeState : bad type");
    }
}

void NotesManager::triggerArchivedSubSet() {
    /*! EXPERIMENTAL - Checks if there's a subset of Notes that only references itself.
    if it's the case, the user is asked if their want to erase this subset. */
    Relation * relationReference = findRelation("Référence");

    // In order to test if the notes are both archived
    struct archivedNotesInCouple {
        bool operator()(Relation::iterator  itC){
            // if the notes are both archived
            return ((*itC)->getAsc()->getState() == archive && (*itC)->getAsc()->getState());
        }
    };

    archivedNotesInCouple ourTest;

    // We want to find the list of archived component of the graph
    QList<QSet<QString>> listOfComponents;

    // For each couples ; we create the component of the graph if there's not component that contains both the first or the second id
    qDebug() << "Boucle Relation::iterator";
    for(Relation::iterator itC = relationReference->begin(); itC != relationReference->end();itC++){
        if(ourTest(itC)){ //if the notes are both archived
            QString firstID = (*itC)->getIdAsc();
            QString secID = (*itC)->getIdDesc();
            qDebug() << "itC : " <<firstID << secID;
            QList<QSet<QString>>::iterator itS = listOfComponents.begin();
            bool added = false; // to know if we have to create a new component
            while(!added && itS != listOfComponents.end()){
                qDebug() << "# Recherche dans les composantes";
                QSet<QString>  currentComponent = (*itS);
                // If one of the two ID is present in the component ; we had those to it.
                if(currentComponent.find(firstID)!= currentComponent.end() || currentComponent.find(secID)!= currentComponent.end()) {
                    qDebug() << "## Un des deux présent dans une des composantes";
                    currentComponent << firstID << secID;
                    added = true;
                }
                itS++;
            }
            // If nothing was added, we have to create a new component
            if(!added){
                qDebug() << "## Ajout dans une nouvelle composante";
                QSet<QString> newComponent;
                newComponent << firstID << secID;
                listOfComponents.push_back(newComponent);
            }
        }
    }
    // Now we have all the components in the list ; we just have to return the components that are archived.
    for(QList<QSet<QString>>::iterator itC = listOfComponents.begin(); itC!= listOfComponents.end(); ++itC){
        qDebug() << "Component archived :";
        for(QSet<QString>::iterator itN = (*itC).begin(); itN != (*itC).end(); ++itN)
            qDebug() << (*itN);
    }
}

void NotesManager::emptyBin()
{
    /*! Empties the bin and erases all the notes with the state 'dustbin' */
    iteratorNote it = beginNote();
    QSqlQuery query;
    while(it != endNote()){
        if ((*it)->getState()==dustbin) {
            dataManager->deleteNote(*it);
            // Erasing in the NotesManager
            it = eraseNote(it);
        }
        else {
            it++;
        }
    }
}

uint NotesManager::nbNotesInBin() const
{
    /*! Returns the number of notes in the bin. */
    uint nbNotes = 0;
    QSqlQuery query;
    for(const_iteratorNote it = cbeginNote();it != cendNote();it++){
        if ((*it)->getState()==dustbin)
            nbNotes++;
    }
    return nbNotes;
}

Relation *NotesManager::findRelation(const QString &name)
{
    /*! Returns a pointer on the Relation of this name ; nullptr if there isn't. */
    DicoRelations::iterator it = m_relations.find(name);
    if(it==m_relations.end()) //No relation with this name
        return nullptr;

    return it.value();
}

Relation *NotesManager::createRelation(const QString &name, const QString &description, const bool &isOriented,bool saveInDB)
{
    /*! Creates a relation oriented by default and returns it.
     * This method is used when we load the data from the dataManager and when we create relation on the application.
     * saveInDB is by default true and in specifies if the relation must be saved in the DB. */
    if(findRelation(name))
        throw NoteException("NotesManager::createRelation() : this relation already exists.");

    Relation * newRelation = new Relation(name,description,isOriented);
    m_relations[name] = newRelation;
    if (saveInDB)
        dataManager->saveRelation(*newRelation,true); //toInsert set to true
    return newRelation;
}

QSet<pair<QString,QString>> NotesManager::getCouplesWithNote(const Note * noteToFind, const QString &relationName, bool findAsc)
{
    /*! Returns the list of ID of the notes presents in the couples containing a note in a specific relation. */
    QSet<pair<QString,QString>> idSet;
    if (noteToFind == nullptr)
        return idSet;

    Relation * relationToFindCouplesWithin = findRelation(relationName);
    if (relationToFindCouplesWithin == nullptr)
        throw NoteException("NotesManager::getCouplesWithNote : relation not found.");
    for (Relation::iterator it = relationToFindCouplesWithin->begin(); it != relationToFindCouplesWithin->end();it++) {
        if (findAsc && (*it)->getAsc()->getId() == noteToFind->getId()){
            pair<QString,QString> toInsert = make_pair((*it)->getDesc()->getId(),(*it)->getLabel());
            idSet.insert(toInsert);

        }
        else if (!findAsc && (*it)->getDesc()->getId() == noteToFind->getId()){
            pair<QString,QString> toInsert = make_pair((*it)->getAsc()->getId(),(*it)->getLabel());
            idSet.insert(toInsert);
        }
    }
    return idSet;
}

QSet<QString> NotesManager::getReferencedNotes(const Note * noteThatReferences) {
    /*! Returns the list of ID of the notes referenced by 'noteThatReferences'. */

    QSet<pair<QString,QString>> idSet = getCouplesWithNote(noteThatReferences, "Référence",true);
    QSet<QString> refSet;
    for (QSet<pair<QString,QString>>::iterator itS = idSet.begin(); itS != idSet.end();itS++)
    {
        refSet.insert((*itS).first);
    }
    return refSet;
}

QSet<QString> NotesManager::getNotesThatReference(const Note * noteThatIsReferenced) {
    /*! Returns the list of ID of the notes that reference 'noteThatReferences'. */

    QSet<pair<QString,QString>> idSet = getCouplesWithNote(noteThatIsReferenced, "Référence",false);
    QSet<QString> refSet;
    for (QSet<pair<QString,QString>>::iterator itS = idSet.begin(); itS != idSet.end();itS++)
    {
        refSet.insert((*itS).first);
    }
    return refSet;
}
