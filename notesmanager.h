#ifndef NOTEMANAGER_H
#define NOTEMANAGER_H

#include "note.h"
#include "relation.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QApplication>
#include <QDebug>
#include <string>
#include <iostream>
#include "datamanager.h"

typedef QMap<QString,Note*> DicoNotes;
typedef QMap<QString,Relation*> DicoRelations;


/*! \class NotesManager
 * \brief Class that manages all the data and their
 *
 *  Is implemented under the Singleton Design Pattern
 */
class NotesManager
{
public:
    static NotesManager& getInstance(); /*!< Gives tha unique instance of the NotesManager */
    static void freeManager(); /*!< Free the memory used by the NotesManager; it can be rebuild later */

    // Getters
    uint getNbArticles() const {return nbArticle;} /*!< Returns the number of articles */
    uint getNbMedia() const {return nbMedia;} /*!< Returns the number of media */
    uint getNbTask() const {return nbTask;} /*!< Returns the number of tasks */
    uint getNbNotes() const {return getNbArticles()+getNbArticles()+getNbTask();} /*!< Returns the total number of notes */

    AbstractDataManager& getDataManager() {return *dataManager;} /*!< Returns the datamanager used in the application.*/

    // Creation and management of objects (Note and Relation)
    Note * createNote(const QString &id, const QString &title, const NoteType type, const QDateTime& creationDateTime=QDateTime::currentDateTime(), const NoteState& state=active,bool saveInDB = true);
    Note * findNote(const QString& id); /*!< Returns the note based on the id. nullptr is returned if the notes doesn't exist. */
    void deleteNote(const QString& id);

    bool isPresent(const QString &id) { return (findNote(id) != nullptr);} /*!< Returns true if a Note with this ID is present. */
    void changeState(const QString &id, const NoteState &state);
    void emptyBin();
    uint nbNotesInBin() const;
    void triggerArchivedSubSet();

    Relation * createRelation(const QString &name, const QString &description, const bool &isOriented=true,bool saveInDB = true);
    Relation * findRelation(const QString& name);

    int getRowCount() const {return max(max(nbArticle, nbMedia), nbTask);} /*!< Used by the model to know the right number of rows to show */

    QSet<pair<QString,QString>> getCouplesWithNote(const Note *noteToFind, const QString& relationName, bool findAsc = true);
    QSet<QString> getReferencedNotes(const Note * noteThatReferences);
    QSet<QString> getNotesThatReference(const Note * noteThatIsReferenced);


    // DEBUG functions
    void debugPrintNotes();
    void debugPrintRelations();

    /** Iterator on the notes used in the application. Adapted from DicoNotes::iterator via our custom iterators.. */
    typedef customIterator::iterator<Note, DicoNotes,NotesManager> iteratorNote;

    iteratorNote beginNote() {return iteratorNote(m_notes.begin());} /*!< Returns a iterator of notes set on the first note (the notes are sorted alphabeticaly on their ID). */
    iteratorNote endNote() {return iteratorNote(m_notes.end());} /*!< Returns a iterator of notes set on the last note (the notes are sorted alphabeticaly on their ID). */
    iteratorNote eraseNote(const iteratorNote& it) { return iteratorNote(m_notes.erase(it.m_iterator)); } /*!< Erase a note based on an iterator pointing on it. */

    /**  Const Iterator on the notes used in the application. Adapted from DicoNotes::const_iterator via our custom iterators. */
    typedef  customIterator::const_iterator<Note, DicoNotes,NotesManager> const_iteratorNote;

    const_iteratorNote cbeginNote() const {return const_iteratorNote(m_notes.begin());} /*!< Returns a iterator of notes set on the first note (the notes are sorted alphabeticaly on their ID). */
    const_iteratorNote cendNote() const {return const_iteratorNote(m_notes.end());} /*!< Returns a iterator of notes set on the last note (the notes are sorted alphabeticaly on their ID). */


    /**  Iterator on the relations used in the application. Adapted from DicoRelations::iterator via our custom iterators. */
    typedef  customIterator::iterator<Relation, DicoRelations,NotesManager> iteratorRelation;
    iteratorRelation beginRelation() {return iteratorRelation(m_relations.begin());} /*!< Returns a iterator of relations set on the first relation (the relations are sorted alphabeticaly on their name). */
    iteratorRelation endRelation() {return iteratorRelation(m_relations.end());} /*!< Returns a iterator of relations set on the last relation (the relations are sorted alphabeticaly on their name). */

    /** Const Iterator on the relations used in the application. Adapted from DicoRelations::const_iterator via our custom iterators. */
    typedef  customIterator::const_iterator<Relation, DicoRelations,NotesManager> const_iteratorRelation;
    const_iteratorRelation cbeginRelation() const {return const_iteratorRelation(m_relations.begin());} /*!< Returns a iterator of relations set on the first relation (the relations are sorted alphabeticaly on their name). */
    const_iteratorRelation cendRelation() const {return const_iteratorRelation(m_relations.end());} /*!< Returns a iterator of relations set on the last relation (the relations are sorted alphabeticaly on their name). */

private:
    NotesManager();
    ~NotesManager() {}

    /*! \struct NotesManager::Handler
     *  \brief The class that handles the unique instance of NotesManager for the Singleton.
     *
     */
    struct Handler {
        NotesManager *instance; /*!< Points on the unique instance of NotesManager*/
        Handler():instance(nullptr){}
        ~Handler() { delete instance; }
    };
    static Handler handler; /*!< The handler of the unique instance of the manager */
    AbstractDataManager * dataManager; /*!< The specific data manager used for the data persistance */
    DicoNotes m_notes; /*!< Map the notes indexed by their ID */
    DicoRelations m_relations; /*!< Map the relations indexed by their names */
    uint nbArticle; /*!< Number of articles in the NotesManager */
    uint nbMedia; /*!< Number of media in the NotesManager */
    uint nbTask; /*!< Number of tasks in the NotesManager */
};

#endif // NOTEMANAGER_H
