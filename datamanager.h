#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QApplication>
#include <QDebug>
#include <string>
#include <iostream>

#include "note.h"
#include "relation.h"


/*! \class AbstractDataManager
 *  \brief The abstract class that interfaces with the data.
 *
 */
class AbstractDataManager
{
    friend class NotesManager;
    virtual void loadNotesAndVersions() const = 0; /*!< The virtual method that loads the Notes and Versions from the persistent data. */
    virtual void loadRelationsAndCouples() const = 0; /*!< The virtual method that loads the Relations and Couples from the persistent data. */
public:
    AbstractDataManager() {} /*!< The canonical constructor. */
    virtual ~AbstractDataManager() {} /*!< The canonical destructor. */

    virtual bool deleteNote(const Note * noteToDel) const = 0; /*!< The virtual method that deletes a Note from the persistent data. */
    virtual bool saveNote(const Note& n,bool toInsert=false) const = 0; /*!< The virtual method that saves a Note in the persistent data. */
    virtual bool saveVersion(const Version *vers, const QString &noteID, const NoteType &nt) const = 0; /*!< The virtual method that saves a Version of a Note in the persistent data. */
    virtual bool saveRelation(const Relation &r, bool toInsert) const = 0; /*!< The virtual method that saves a Relation in the persistent data. */
    virtual bool deleteCouple(const Couple * coupleToDel,const QString& name) const = 0; /*!< The virtual method that deletes a Couple from the persistent data. */
    virtual bool saveCouple(const Couple& c,const QString& name, bool toInsert) const = 0; /*!< The virtual method that saves a Couple in the persistent data. */
};

/*! \class SQLiteManager
 *  \brief [Inherited from AbstractDataManager] A concrete class that interfaces with the data via a SQLiteDatabase
 *
 */
class SQLiteManager : public AbstractDataManager {
    /*! \struct SQLiteManager::Handler
     *  \brief The class that handles the unique instance of SQLiteManager for the Singleton.
     *
     */
    friend class NotesManager;
    SQLiteManager();
    ~SQLiteManager() {}
    void operator=(const SQLiteManager&) {} /*!< Private redéfinition of the = operator for the Singleton */
    SQLiteManager(const SQLiteManager&) {} /*!< Private redéfinition of the copy constructor for the Singleton. */

    QSqlDatabase plurinotesDatabase; /*!< The object corresponding to the database used. */

    /*! \struct SQLiteManager::Handler
     *  \brief The class that handles the unique instance of SQLiteManager for the Singleton.
     *
     */
    struct Handler {
        SQLiteManager *instance; /*!< Points on the unique instance of NotesManager*/
        Handler():instance(nullptr){}
        ~Handler() { delete instance; }
    };
    static Handler handler; /*!< The handler of the unique instance of the manager */

    const QString DATEFORMAT = QString("yyyy-MM-dd hh:mm:ss"); /*!< The DateTime format used to store DateTime strings in the database */

    virtual void loadNotesAndVersions() const override;
    virtual void loadRelationsAndCouples() const override;
    bool createTemplateDataBase();
    bool connectionWithDataBase();
    static SQLiteManager& getInstance(); /*!< Gives the unique instance of the SQLiteManager */

public:
    virtual bool deleteNote(const Note * noteToDel) const override;
    virtual bool saveNote(const Note& n,bool toInsert=false) const override;
    virtual bool saveVersion(const Version *vers, const QString &noteID, const NoteType &nt) const override;
    virtual bool saveRelation(const Relation &r, bool toInsert=false) const override;
    virtual bool deleteCouple(const Couple * coupleToDel,const QString& name) const override;
    virtual bool saveCouple(const Couple& c,const QString& name, bool toInsert=false) const override;
};

#endif // DATAMANAGER_H
