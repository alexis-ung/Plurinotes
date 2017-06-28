#ifndef NOTE_H
#define NOTE_H

#include <QWidget>
#include <QtWidgets>
#include <QtSql>
#include "iterator.h"

using namespace std;

class NoteException;
class Note;
class Version;
class Article;
class Media;
class Task;

typedef enum ns {active,archive,dustbin} NoteState;
typedef enum ts {progress,standby,done} TaskStatus;
typedef enum nt {ArticleType, MediaType, TaskType, EmptyType} NoteType;
typedef QMap<QString,QVariant> Dico;
typedef QList<Version*> ListVersion;

/*! \class NoteException
 * \brief Custom inheritance of the std::exception class.
 *
 */
class NoteException : public exception {
public:
  const char * what() const throw() { return info.c_str(); } /*!< Overloading of the what() method : returns the error contained in the NoteException. */
  NoteException(const string& s):info(s){} /*!< Canonical constructor of a NoteException. */
  ~NoteException() throw() {}
private:
  string info; /*!< The string stating the error. */
};

/*! \class Note
 *  \brief Note that contains its versions
 *
 *  A note is one of the type objects managed by the NotesManager. It is either an article, a media or a task.
 *  A note has several versions stocked in a ListVersion (ie QList<Version*>) attribute m_versions
 */
class Note
{
public:
    Note() : m_id("UNDEFINED"), m_type(EmptyType), m_creationDateTime(QDateTime(QDate(0,0,0))) {} /*! Necessary for Note to be declared as a MetaType, data included has to be considered as inoperant */

    //Regular constructors
    Note(const QString& id,const QString& title,const NoteType type,const QDateTime& creationDateTime=QDateTime::currentDateTime(),const NoteState& state=active)
        : m_id(id), m_title(title),m_type(type),m_creationDateTime(creationDateTime),m_state(state) {} /*!< The canonical constructor of a Note. By default a Note is active and it created at the current runtime datetime */

    ~Note(){ m_versions.clear(); } /*!< Destructor of the note : erase all the versions */
    Note &operator=(const Note &n)
    {
        //The default operator= does not work
        Note *ne = new Note(n.getId(), n.getTitle(), n.getType());
        return *ne;
    } /*!< Destructor of the '=' operator for Note. */
    // Attributes shared by all types of Notes are declared here, with getters and setters

    // Getters
    const QString getTitle() const {return m_title;} /*!< Getter for the title */
    const QString getId() const {return m_id;} /*!< Getter for the ID */
    const QDateTime getCreationDateTime() const {return m_creationDateTime;} /*!< Getter for the date of creation */
    NoteType getType() const {return m_type;} /*!< Getter for the type */
    NoteState getState() const {return m_state; } /*!< Getter for the state */

    // Setters
    void setTitle(QString title); /*!< Setter for the title */
    void setState(NoteState s); /*!< Setter for the state */

    void createVersion(Dico& dico, bool fromPersistentData =false); /*!< Creates a new version of the note. */
    Version *getLastversion();/*!< Get the most recent version of the note */
    const Version *getLastversion() const; /*!< Get the most recent version of the note */

    void debugPrintVersions() const; /*!< Prints all the informations encapsulated in the Note */

    bool isEditable() const { return (getState() == active);} /*!< Returns True is the Note is editable. This is based on the state of the note : a note is editable iff it is active */
    void toActive() {setState(active);} /*!< Activate the note. It changes its state from 'archive' or 'dustbin' to 'active'.*/
    void toArchive() { setState(archive);} /*!< Archive the note. It changes its state from 'active' to 'archive'.*/
    void toBin() { setState(dustbin);} /*!< Puts the note in the bin. It changes its state from 'archive' or 'active' to 'dustbin'.*/

    void updateReferences() const;

    /** Iterator on the versions of a note. Adapted from ListVersion::iterator via our custom iterators. */
    typedef customIterator::iterator<Version, ListVersion,Note> iterator;
    iterator begin() { return iterator(m_versions.begin()); } /*!< Returns a iterator of versions set on the most recent version. */
    iterator end() { return iterator(m_versions.end()); } /*!< Returns a iterator of versions set on the oldest version. */


    /** Const Iterator on the versions of a note. Adapted from ListVersion::const_iterator via our custom iterators. */
    typedef customIterator::const_iterator<Version, ListVersion,Note> const_iterator;
    const_iterator cbegin() const { return const_iterator(m_versions.begin()); } /*!< Returns a iterator of versions set on the most recent version. */
    const_iterator cend() const { return const_iterator(m_versions.end()); } /*!< Returns a iterator of versions set on the oldest version. */

private:
    const QString m_id; /*!< The string that represents the ID of the note */
    QString m_title; /*!< The string that represents the title of the note */
    const NoteType m_type; /*!< The type of the note : ArticleType, MediaType or TaskType */
    const QDateTime m_creationDateTime; /*!< The date at which the note was created */
    NoteState m_state; /*!< The current state of the note : active, archive or bin */
    ListVersion m_versions; /*!< The list of all the version on the note sorted by last date of modification */
};

/*! \class Version
 *  \brief [Abstract] A version of a Note object
 *
 *  A version of a Note object. It is identified by its date of modification. It is either a Article a Media or a Task.
 */
class Version {
public:
  Version(const QDateTime& d=QDateTime::currentDateTime()) : m_modifDateTime(d){} /*!< The canonical Version constructor. The only argument isn't needed.*/
  virtual ~Version() {} /*!< Virtual destructor */

  virtual NoteType getType() const = 0; /*!< [Pure abstract] Returns the type of the version */
  virtual void debugPrintInfo() const = 0; /*!< [Pure abstract] Prints all the informations encapsulated in the Version */

  virtual QSet<QString> parseData(QSet<QString> listID) const = 0; /*!< [Pure abstract] Parse all text entry to get id of referenced notes */

  const QDateTime getModifDate() const { return m_modifDateTime; } /*!< Returns the date of modification */
private:
  const QDateTime m_modifDateTime; /*!< The date of modification of the note identificating the version */
};


/*! \class Article
 *  \brief [Inherited from Version] Specific Version containing a text
 *
 */
class Article : public Version
{
public:
    Article() : Version() {} /*! Necessary for Article to be declared as a metatype */
    Article(const QDateTime& modDate,const QString& tex):Version(modDate),m_text(tex){} /*!< Canonical constructor of an Article. */

    // Getters
    virtual NoteType getType() const override {return ArticleType;} /*!< [Virtual] Getter for the type */
    QString getText() const {return m_text;} /*!< Getter for the text */

    virtual void debugPrintInfo() const override; /*!< [Virtual] Prints all the informations encapsulated in the Article */

    virtual QSet<QString> parseData(QSet<QString> listID) const override; /*!< [Virtual] Parses all text entry to get id of referenced notes */
    // Setters
    void setText(const QString& text) {m_text = text;} /*!< Setter for the text */


private:
    QString m_text; /*!< The text of the article */

};

/*! \class Media
 *  \brief [Inherited from Version] Specific Version containing a description and a filename
 *
 */
class Media : public Version
{
public:
    Media() : Version(){} /*! Necessary for Media to be declared as a metatype */
    Media(const QDateTime& modDate, const QString& descr,const QString& file=""):Version(modDate),m_description(descr),m_filename(file) {} /*!< Canonical constructor of a Media. */

    // Getters
    virtual NoteType getType() const override {return MediaType;} /*!< [Virtual] Getter for the type */
    QString getDescription() const {return m_description;} /*!< Getter for the description */
    QString getFileName() const {return m_filename;} /*!< Getter for the filename */

    virtual void debugPrintInfo() const override; /*!< [Virtual] Prints all the informations encapsulated in the Media */

    virtual QSet<QString> parseData(QSet<QString> listID) const override; /*!< [Virtual] Parses all text entry to get id of referenced notes */

    // Setters
    void setDescription(QString d) {m_description = d;} /*!< Setter for the description */
    void setFileName(QString filename) {m_filename = filename;} /*!< Setter for the filename */


private:
    QString m_description; /*!< The description of the media */
    QString m_filename; /*!< The file name of the media */

};

/*! \class Task
 *  \brief [Inherited from Version] Specific Version containing an action, a status, and eventually a priority and a dead line
 *
 */
class Task : public Version
{
public:
    Task() : Version(){} /*! Necessary for Task to be declared as a metatype */
    Task(const QDateTime& modDate,const QString& ac,TaskStatus stat,uint prio = 0, const QDateTime& deadL=QDateTime(QDate(0,0,0)))
         :Version(modDate),m_action(ac),m_status(stat),m_priority(prio),m_deadLine(deadL) {} /*!< Canonical constructor of a Task. */

    // Getters
    virtual NoteType getType() const override {return TaskType;} /*!< [Virtual] Getter for the type */
    QString getAction() const {return m_action; } /*!< Setter for the action */
    TaskStatus getStatus() const { return m_status;} /*!< Getter for the status */
    uint getPriority() const { return m_priority;} /*!< Getter for the priority */
    QDateTime getDeadLine() const { return m_deadLine;} /*!< Getter for the dead line */

    virtual void debugPrintInfo() const override;  /*!< [Virtual] Prints all the informations encapsulated in the Task */

    QSet<QString> parseData(QSet<QString> listID) const override; /*!< [Virtual] Parses all text entry to get id of referenced notes */

    // Setters
    void setAction(QString action) {m_action = action;} /*!< Setter for the action */
    void setStatus(TaskStatus status) { m_status = status;} /*!< Setter for the status */
    void setPriority(uint priority) { m_priority = priority;} /*!< Setter for the priority */
    void setDeadLine(QDateTime deadLine) {m_deadLine = deadLine; } /*!< Setter for the dead line */


private:
    QString m_action; /*!< The action of the task */
    TaskStatus m_status; /*!< The status of the task : progress, standby or done */
    uint m_priority; /*!< [Optionnal] The priority of the task */
    QDateTime m_deadLine; /*!< [Optionnal] The dead line of the task */
};



// Enables classes Article, Media and Task to be contained in a QVariant
Q_DECLARE_METATYPE(Note)

Q_DECLARE_METATYPE(Article)
Q_DECLARE_METATYPE(Media)
Q_DECLARE_METATYPE(Task)


Q_DECLARE_METATYPE(Version*)
Q_DECLARE_METATYPE(Article*)
Q_DECLARE_METATYPE(Media*)
Q_DECLARE_METATYPE(Task*)
Q_DECLARE_METATYPE(Note*)


#endif // NOTE_H
