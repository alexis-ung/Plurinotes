#include "relation.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include "notesmanager.h"

void Couple::debugPrintInfo() const
{
    /*! Displays the info stored in the article */
    qDebug() << " - Label : " << getLabel();
    qDebug() << " - Asc : " << getAsc()->getTitle();
    qDebug() << " - Desc : " << getDesc()->getTitle();
}


void Relation::createCouple(const Note *first, const Note *sec, const QString &l, bool saveInDB)
{
    /*! Creates a couple in the relations.
     * Checks if there's already a couple in the relation.
     * saveInDB indicates if the couple must be saved via the dataManager */

    if (first == nullptr)
        throw NoteException("Relation::createCouple : first Note is nullptr.");

    if (sec == nullptr)
        throw NoteException("Relation::createCouple : second Note is nullptr.");

    // We look if there is already a couple in the relation
    Couple * newCouple = getCouple(first,sec);

    if (newCouple) { // If a couple was found
        throw NoteException("Relation::createCouple() : couple already exists");
    }

    // Otherwise we can add a new couple to the relation
    newCouple = new Couple(first,sec,l);
    *this << newCouple;

    if (saveInDB) {
        NotesManager::getInstance().getDataManager().saveCouple(*newCouple,this->getName(),true);
    }
}

Couple *Relation::getCouple(const Note *noteAsc, const Note *noteDesc)
{
    /*! Returns the couple if it exists in the relation ; otherwise returns nullptr */
    if (noteAsc == nullptr || noteDesc == nullptr)
        return nullptr;

    Couple coupleToFind(noteAsc,noteDesc);

    Couple * currentCouple = nullptr;

    if (!isOriented()){ // we have to check for the other order
        Couple anotherCoupleToFind(noteDesc,noteAsc);
        for(iterator it = begin(); it != end(); it++){
            currentCouple = (*it);
            if(*currentCouple == coupleToFind || *currentCouple == anotherCoupleToFind)
                return currentCouple;
        }
    }
    else {
        for(iterator it = begin(); it != end(); it++){
            currentCouple = (*it);
            if(*currentCouple == coupleToFind)
                return currentCouple;
        }
    }
    return nullptr;
}

const Couple *Relation::getCouple(const Note *noteAsc, const Note *noteDesc) const
{
    /*! Returns the couple if it exists in the relation ; otherwise returns nullptr */
    if (noteAsc == nullptr || noteDesc == nullptr)
        return nullptr;

    Couple coupleToFind(noteAsc,noteDesc);

    const Couple * currentCouple = nullptr;

    if (!isOriented()){ // we have to check for the other order
        Couple anotherCoupleToFind(noteDesc,noteAsc);
        for(const_iterator it = cbegin(); it != cend(); it++){
            currentCouple = (*it);
            if(*currentCouple == coupleToFind || *currentCouple == anotherCoupleToFind)
                return currentCouple;
        }
    }
    else {
        for(const_iterator it = cbegin(); it != cend(); it++){
            currentCouple = (*it);
            if(*currentCouple == coupleToFind)
                return currentCouple;
        }
    }
    return nullptr;
}


Relation::iterator Relation::deleteCouple(Couple *coupleToDel)
{
    /*! Deletes the couple both in the NotesManager and via the dataManager */
    iterator it=begin();
    while(it != end()){
        if ((*it) == coupleToDel) {
            NotesManager::getInstance().getDataManager().deleteCouple(coupleToDel,getName());
            it = erase(it);
            return it;
        }
        else {
            it++;
        }
   }
   throw NoteException("Relation::deleteCouple : couple not found");
}



void Relation::debugPrintCouples() const
{
    /*! Display the couples stored in the relation */
    qDebug() << "Couples dans la relations : ";

    for (const_iterator i = cbegin(); i != cend(); ++i) {
        (*i)->debugPrintInfo();
    }
    qDebug() << "";
}

