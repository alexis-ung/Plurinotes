#include "datamanager.h"
#include "notesmanager.h"

SQLiteManager::Handler SQLiteManager::handler=Handler();

SQLiteManager::SQLiteManager()
{
    /*! The canonical constructor of the SQLiteManager. It creates the connection with the database here.*/
    connectionWithDataBase();
}


SQLiteManager &SQLiteManager::getInstance()
{
    /*! Returns the unique instance of the SQLiteManager. */
    if (!handler.instance)
        handler.instance = new SQLiteManager;
    return *handler.instance;
}

bool SQLiteManager::deleteNote(const Note * noteToDel) const
{
    /*! Deletes a Note and all its version present in the database. */

    if (noteToDel == nullptr)
        throw NoteException("SQLiteManager::deleteNote : noteToDel is nullptr.");

    QSqlQuery query;
    switch(noteToDel->getType()){
    case ArticleType:
        query.prepare("DELETE FROM Article WHERE id=:id;");
        break;

    case MediaType:
        query.prepare("DELETE FROM Media WHERE id=:id;");
        break;

    case TaskType:
        query.prepare("DELETE FROM Task WHERE id=:id;");
        break;

    case EmptyType:
        throw NoteException("SQLiteManager::deleteNote() : Empty type for the note");
        break;

    default:
        throw NoteException("SQLiteManager::deleteNote() : No type for the note");
        break;
    }
    query.bindValue(":id",noteToDel->getId());
    query.exec();
    // Erasing in the data base
    query.prepare("DELETE FROM Note WHERE id=:id;");
    query.bindValue(":id",noteToDel->getId());
    return query.exec();
}

bool SQLiteManager::saveNote(const Note& n,bool toInsert) const
{
    /*! Saves the note in the data base. If its the first saves in the database (ie if toInsert == True),
     * we insert the the Note table ; otherwise we update it.
     * Returns a boolean stating the result. */
    QSqlQuery query;
    if (toInsert){
        query.prepare("INSERT INTO Note (id, title,type,creationDateTime,state)"
                       "VALUES (:id, :title,:type,:creationDateTime,:state);");
        query.bindValue(":id",n.getId());
        query.bindValue(":title",n.getTitle());
        query.bindValue(":type",QString::number(n.getType()));
        query.bindValue(":creationDateTime",n.getCreationDateTime().toString(DATEFORMAT));
        query.bindValue(":state",QString::number(n.getState()));
    }
    else {
       query.prepare("UPDATE Note SET title=:title,state=:state WHERE id=:id");
       query.bindValue(":title",n.getTitle());
       query.bindValue(":state",QString::number(n.getState()));
       query.bindValue(":id",n.getId());
    }

    bool result = query.exec();
    return result;
}

bool SQLiteManager::saveVersion(const Version * vers,const QString& noteID,const NoteType& nt) const
{
    /*! Saves a Version based on its type by inserting it in the database.*/
    QSqlQuery query;

    if (vers == nullptr)
        throw NoteException("SQLiteManager::saveVersion : Version is nullptr.");

    switch(nt)
    {
    case ArticleType:
    {
        const Article *a = dynamic_cast<const Article*>(vers);

        /*! Polymorphic method : saves the article in the data base based on the id of the note */
        query.prepare("INSERT INTO Article (id, modifDateTime, text)"
                           "VALUES (:id, :modifDateTime, :text);");
        query.bindValue(":id",noteID);
        query.bindValue(":modifDateTime",a->getModifDate().toString(DATEFORMAT));
        query.bindValue(":text",a->getText());

        bool result = query.exec();
        return result;
    }
    case MediaType:
       {
        const Media *a = dynamic_cast<const Media*>(vers);

        query.prepare("INSERT INTO Media (id, modifDateTime, description, filename)"
                           "VALUES(:id, :modifDateTime, :description, :filename);");
        query.bindValue(":id",noteID);
        query.bindValue(":modifDateTime",a->getModifDate().toString(DATEFORMAT));
        query.bindValue(":description",a->getDescription());
        query.bindValue(":filename",a->getFileName());

        bool result = query.exec();
        return result;
       }
    case TaskType:
       {
        const Task *a = dynamic_cast<const Task*>(vers);

        query.prepare("INSERT INTO Task (id, modifDateTime, action, status, priority, deadLine)"
                           "VALUES (:id, :modifDateTime, :action, :status, :priority, :deadLine);");
        query.bindValue(":id",noteID);
        query.bindValue(":modifDateTime",a->getModifDate().toString(DATEFORMAT));
        query.bindValue(":action",a->getAction());
        query.bindValue(":status",a->getStatus());
        query.bindValue(":priority",QString::number(a->getPriority()));
        query.bindValue(":deadLine",a->getDeadLine().toString(DATEFORMAT));

        bool result = query.exec();
        return result;
       }
    case EmptyType:
    default:
    {
        throw NoteException("SQLiteManager::saveVersion : EmptyType or no type");
    }
    }
    return true;
}

bool SQLiteManager::saveRelation(const Relation& r,bool toInsert) const
{
    /*! Saves the relation in the data base. If its the first saves in the database (ie if toInsert == True),
    we insert the Relation table ; otherwise we update it.
    Returns a boolean stating the result. */
    QSqlQuery query;
    if (toInsert){
        query.prepare("INSERT INTO Relation (name, description,isOriented) VALUES (:name, :description, :isOriented);");
    }
    else {
         query.prepare("UPDATE Relation SET description=:description,isOriented=:isOriented WHERE name=:name");
    }
    query.bindValue(":description",r.getDescription());
    query.bindValue(":isOriented",r.isOriented());
    query.bindValue(":name",r.getName());
    bool result = query.exec();
    return result;
}

bool SQLiteManager::deleteCouple(const Couple *coupleToDel, const QString &name) const
{
    QSqlQuery query;
    query.prepare("DELETE FROM Couple WHERE idAsc=:idAsc AND idDesc=:idDesc AND relation=:relation");

    query.bindValue(":idAsc",coupleToDel->getIdAsc());
    query.bindValue(":idDesc",coupleToDel->getIdDesc());
    query.bindValue(":relation",name);

    return query.exec();
}

bool SQLiteManager::saveCouple(const Couple& c,const QString& name, bool toInsert) const
{
    /*! Saves the couple in the data base. If its the first saves in the database (ie if toInsert == True),
    we insert the Relation table ; otherwise we update it.
    Returns a boolean stating the result. */
    QSqlQuery query;
    if (toInsert){
        query.prepare("INSERT INTO Couple (idAsc,idDesc,relation,label) VALUES (:idAsc,:idDesc,:relation,:label);");
        query.bindValue(":idAsc",c.getIdAsc());
        query.bindValue(":idDesc",c.getIdDesc());
        query.bindValue(":relation",name);
        query.bindValue(":label",c.getLabel());

    }
    else {
        query.prepare("UPDATE Couple SET label=:label WHERE idAsc=:idAsc AND idDesc=:idDesc AND relation=:relation;");
        query.bindValue(":label",c.getLabel());
        query.bindValue(":idAsc",c.getIdAsc());
        query.bindValue(":idDesc",c.getIdDesc());
        query.bindValue(":relation",name);
    }

    bool result = query.exec();
    return result;
}

bool SQLiteManager::connectionWithDataBase()
{
    /*! Creates the connection with the database.
     * If there's nothing in it (if the file isn't present for example) it creates a template database? */
    // We set the database in the directory of the executable
    QString localFolder = QCoreApplication::applicationDirPath();
    plurinotesDatabase = QSqlDatabase::addDatabase("QSQLITE");
    plurinotesDatabase.setDatabaseName(localFolder + "/plurinotesDB.db");

    if (!plurinotesDatabase.open()) {
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
            qApp->tr("Unable to establish a database connection.\n"
                     "This example needs SQLite support. Please read "
                     "the Qt SQL driver documentation for information how "
                     "to build it.\n\n"
                     "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }

    QSqlQuery query;

    if(!query.exec("SELECT * FROM Note")){
        createTemplateDataBase();
    }
    return true;
}


bool SQLiteManager::createTemplateDataBase()
{
    /*! Creates a template database. Called if there's nothing in the data base. */
    QSqlQuery query;

    //Creating Tables
    query.exec("CREATE TABLE Note ("
                        "id VARCHAR(30) PRIMARY KEY,"
                        "title VARCHAR(100),"
                        "type INTEGER,"
                        "creationDateTime DATETIME,"
                        "state INTEGER)");

    query.exec("CREATE TABLE Article ("
                        "id VARCHAR(30),"
                        "modifDateTime DATETIME,"
                        "text VARCHAR(300),"
                        "PRIMARY KEY (id,modifDateTime),"
                        "FOREIGN KEY (id) REFERENCES Note (id))");

    query.exec("CREATE TABLE Media ("
                        "id VARCHAR(30),"
                        "modifDateTime DATETIME,"
                        "description VARCHAR(300),"
                        "filename VARCHAR(100),"
                        "PRIMARY KEY (id,modifDateTime),"
                        "FOREIGN KEY (id) REFERENCES Note (id))");

    query.exec("CREATE TABLE Task ("
                        "id VARCHAR(30),"
                        "modifDateTime DATETIME,"
                        "action VARCHAR(50),"
                        "status INTEGER,"
                        "priority INTEGER,"
                        "deadLine DATETIME,"
                        "PRIMARY KEY (id,modifDateTime),"
                        "FOREIGN KEY (id) REFERENCES Note (id))");

    query.exec("CREATE TABLE Relation ("
                        "name VARCHAR(100) PRIMARY KEY,"
                        "description VARCHAR(500),"
                        "isOriented BOOL)");

    query.exec("CREATE TABLE Couple ("
                        "idAsc VARCHAR(30),"
                        "idDesc VARCHAR(30),"
                        "relation VARCHAR(100),"
                        "label VARCHAR(30),"
                        "PRIMARY KEY (idAsc,idDesc,relation),"
                        "FOREIGN KEY (idAsc) REFERENCES Note (id),"
                        "FOREIGN KEY (idDesc) REFERENCES Note (id),"
                        "FOREIGN KEY (relation) REFERENCES Relation (name))");


    //Populating the data base with data

    QString insertIntoNoteAllFields = "INSERT INTO Note (id, title,type,creationDateTime,state) VALUES ";

    /* About SQLite Date format : https://www.sqlite.org/lang_datefunc.html */

    query.exec(insertIntoNoteAllFields + "('A1', 'Henri Poincarré',0,'2017-01-13 17:50:10',0)");
    query.exec(insertIntoNoteAllFields + "('A2', 'Mark Twain',0,'2017-04-07 11:12:10',0)");
    query.exec(insertIntoNoteAllFields + "('A3', 'Doug Rattman',0,'2017-05-01 09:12:10',0)");

    query.exec(insertIntoNoteAllFields + "('M1', 'Lego Léa',1,'2017-02-13 18:45:10',0)");
    query.exec(insertIntoNoteAllFields + "('M2', 'Lego Bart',1,'2017-01-17 12:12:10',0)");


    query.exec(insertIntoNoteAllFields + "('T1', 'Vacances',2,'2017-02-23 19:45:10',0)");
    query.exec(insertIntoNoteAllFields + "('T2', 'Réviser LO21',2,'2017-02-03 08:12:10',0)");

    QString insertIntoArticleAllFields = "INSERT INTO Article (id,modifDateTime, text) VALUES ";

    query.exec(insertIntoArticleAllFields + "('A1','2017-01-13 17:50:10', \"« La pensée n'est qu’un éclair au milieu d'une longue nuit, mais c'est cet éclair qui est tout. » Henri Poincaré\")");
    query.exec(insertIntoArticleAllFields + "('A2','2017-04-07 11:12:10', \"« Ils ne savaient pas que c'était impossible, alors ils l'ont fait » Mark Twain\")");
    query.exec(insertIntoArticleAllFields + "('A3','2017-05-01 09:12:10', 'The cake is a lie.')");

    QString insertIntoMediaAllFields = "INSERT INTO Media (id,modifDateTime, description, filename) VALUES ";

    query.exec(insertIntoMediaAllFields + "('M1','2017-02-13 18:45:10','Lego Léa',':/image/legoLea.jpg')");
    query.exec(insertIntoMediaAllFields + "('M2','2017-01-17 12:12:10','Lego Bart',':/image/legoBart.jpg')");


    QString insertIntoTaskAllFields = "INSERT INTO Task (id,modifDateTime, action, status, priority, deadLine) VALUES ";

    query.exec(insertIntoTaskAllFields + "('T1','2017-02-23 19:45:10', ' - Préparer les vacances avec les potes',0,1, '2017-12-15 00:00:00')");
    query.exec(insertIntoTaskAllFields + "('T2','2017-02-03 08:12:10', ' - Revoir la STL\n - Revoir diagrammes de séquences\n - Revoir métaprogrammation',0,3,'2017-12-15 00:00:00')");

    QString insertIntoRelationAllFields = "INSERT INTO Relation (name,description,isOriented) VALUES ";

    query.exec(insertIntoRelationAllFields + "('Référence','La fameuse relation !','True')");
    query.exec(insertIntoRelationAllFields + "('Brouillon','Le document ascendant est un brouillon du descendant','True')");
    query.exec(insertIntoRelationAllFields + "('Planification','Commun dans une planification','False')");


    return true;
}


void SQLiteManager::loadNotesAndVersions() const
{
    /*! Load all the Notes from the SQLite database ie. query it and creates Note objects
    and Version objects for each object note (see SQL tables) */

    NotesManager& noteManager = NotesManager::getInstance();

    // First Query to get the notes
    QSqlQuery queryAllNote("SELECT * FROM Note;");

    /* Those 'fooField' short uint are used to locate data in the QslQuery
     It avoids using hard coded int to get the data from the query cursor */
    short unsigned int idField = queryAllNote.record().indexOf("id");
    short unsigned int titleField = queryAllNote.record().indexOf("title");
    short unsigned int typeField = queryAllNote.record().indexOf("type");
    short unsigned int creationDateTimeField = queryAllNote.record().indexOf("creationDateTime");
    short unsigned int stateField = queryAllNote.record().indexOf("state");

    // Used to construct a Note object via NoteManager::createNote()
    QString id,title;
    NoteType type;
    QDateTime creationDateTime;
    NoteState state;


    while (queryAllNote.next()) { // For each note in the dataBase
        // We catch the informations
        id = queryAllNote.value(idField).toString();
        title = queryAllNote.value(titleField).toString();
        type = static_cast<NoteType>(queryAllNote.value(typeField).toInt());
        creationDateTime = queryAllNote.value(creationDateTimeField).toDateTime();
        state = static_cast<NoteState>(queryAllNote.value(stateField).toInt());

        // We create a new Note object
        Note * newNote = noteManager.createNote(id,title,type,creationDateTime,state,false);

        // Then we have to create all the versions of this Note, in order to do this :

        // We create a map of parameters to be passed to create a Version object via Note::createVersion()
        Dico dico;

        short unsigned int modifDateTimeField;

        // Getting all the versions of the note based on the type
        switch (newNote->getType()) {
            case ArticleType: {
                QSqlQuery queryArticle("SELECT * FROM Article WHERE ID ='"+id+"' ORDER BY  modifDateTime ASC;");
                modifDateTimeField = queryArticle.record().indexOf("modifDateTime");

                short unsigned int textField = queryArticle.record().indexOf("text");

                while (queryArticle.next()) {
                    dico["modifDateTime"] = queryArticle.value(modifDateTimeField);
                    dico["text"] = queryArticle.value(textField);
                    newNote->createVersion(dico,true); //fromPersistentData == true
                    dico.clear();
                }
            }
                break;
            case MediaType: {
                QSqlQuery queryMedia("SELECT * FROM Media WHERE ID ='"+id+"' ORDER BY  modifDateTime ASC;");
                modifDateTimeField = queryMedia.record().indexOf("modifDateTime");

                short unsigned int descriptionField = queryMedia.record().indexOf("description");
                short unsigned int filenameField = queryMedia.record().indexOf("filename");

                while (queryMedia.next()) {
                    dico["modifDateTime"] = queryMedia.value(modifDateTimeField);
                    dico["description"] = queryMedia.value(descriptionField);
                    dico["filename"] = queryMedia.value(filenameField);
                    newNote->createVersion(dico,true); //fromPersistentData == true
                    dico.clear();
                }
                break;
            }
            case TaskType: {
                QSqlQuery queryTask("SELECT * FROM Task WHERE ID ='"+id+"' ORDER BY  modifDateTime ASC;");
                modifDateTimeField = queryTask.record().indexOf("modifDateTime");

                short unsigned int actionField = queryTask.record().indexOf("action");
                short unsigned int statusField = queryTask.record().indexOf("status");
                short unsigned int priorityField = queryTask.record().indexOf("priority");
                short unsigned int deadLineField = queryTask.record().indexOf("deadLine");


                while (queryTask.next()) {
                    dico["modifDateTime"] = queryTask.value(modifDateTimeField);
                    dico["action"] = queryTask.value(actionField);
                    dico["status"] = queryTask.value(statusField);
                    dico["priority"] = queryTask.value(priorityField);
                    dico["deadLine"] = queryTask.value(deadLineField);


                    newNote->createVersion(dico,true); //fromPersistentData == true
                    dico.clear();
                }
                break;
            }
            case EmptyType:
                throw NoteException("SQLiteManager::loadNotesAndVersions : Empty Type");
                break;

            default:
                throw NoteException("SQLiteManager::loadNotesAndVersions default case : empty");
                break;

        }// End of switch on Note::m_type

        /* At this moment we have created the note and all its versions ;*/
    }
}

void SQLiteManager::loadRelationsAndCouples() const
{
    /*! Load all the Relations from the SQLite database ie. query it and creates Relation objects
    and Couple objects for each object Relation (see SQL tables) */
    NotesManager& noteManager = NotesManager::getInstance();
    QSqlQuery queryAllRelations("SELECT * FROM Relation;");

    /* Those 'fooField' short uint are used to locate data in the QslQuery
     It avoids using hard coded int to get the data from the query cursor */
    short unsigned int nameField = queryAllRelations.record().indexOf("name");
    short unsigned int descriptionField = queryAllRelations.record().indexOf("description");
    short unsigned int isOrientedField = queryAllRelations.record().indexOf("isOriented");

    QString name,description;
    bool isOriented;

    while (queryAllRelations.next()) {

        name = queryAllRelations.value(nameField).toString();
        description = queryAllRelations.value(descriptionField).toString();
        isOriented =queryAllRelations.value(isOrientedField).toBool();

        Relation * newRelation = noteManager.createRelation(name,description,isOriented,false);

        QString idAsc,idDesc,label;
        Note * noteAsc = nullptr;
        Note * noteDesc = nullptr;

        QSqlQuery queryAllCouples("SELECT * FROM Couple WHERE relation ='"+name+"';");

        short unsigned int idAscField = queryAllCouples.record().indexOf("idAsc");
        short unsigned int idDescField = queryAllCouples.record().indexOf("idDesc");
        short unsigned int labelField = queryAllCouples.record().indexOf("label");


        while (queryAllCouples.next()) {
            idAsc = queryAllCouples.value(idAscField).toString();
            idDesc = queryAllCouples.value(idDescField).toString();
            label = queryAllCouples.value(labelField).toString();
            noteAsc = noteManager.findNote(idAsc);
            noteDesc = noteManager.findNote(idDesc);
            newRelation->createCouple(noteAsc,noteDesc,label,false); //toInsert == false
        }
    }
    if(!noteManager.findRelation("Référence"))
        noteManager.createRelation("Référence","Le document A référence le document B",true,true); //isOriented == saveInDB == true
}
