#ifndef STRAT_H
#define STRAT_H

#include <QtWidgets>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QWindow>

typedef QMap<QString,QVariant> Dico;

/**
 * \class InterfaceStrategy
 * \brief Abstract class that defines widgets common to all notes
 *
 * Abstract class used with the design pattern strategy. Defines three types of scenario depending
 * on how the widget will be used: to modify a version, to add a note or to restore a version;
 *
 */


class InterfaceStrategy : public QWidget
{
Q_OBJECT
public:
    InterfaceStrategy(const QString &id="", const QString &title="",  const QString &creat="00/00/0000", const QString &modif="00/00/0000");
    virtual ~InterfaceStrategy();

    //Different scenarios corresponding to our needs
    void loadRestoreInteface();
    void loadAddingInterface();
    void loadDefaultInterface();

    //Getters and setters
    void setID(const QString &id);
    void setTitle(const QString &t);
    QString getID() const;
    QString getTitle() const;
    void setDateCreation(const QString &t);
    void setDateModif(const QString &t);
    void disableSubmitButton();
    bool isSubmitButtonEnabled();

     virtual Dico getData() const = 0; /*!< Virtual pure function that get the data written in the widget */
    //Set the data
    virtual void setData(const Dico &dataMap);

protected:

    //Enables sons to set their own interface
    QPushButton *submitButton; /*!< Submit button at the bottom of the widget*/
    QVBoxLayout *layout; /*!< Main layout of the widget */
    QLineEdit *lineID; /*!< Line to set the ID, only editable with the adding scenario*/
    QLineEdit *titleEdit; /*!< Line to edit the title */
    QHBoxLayout *datesLayout; /*!< Layout that aggregates the labels of the dates*/
    QHBoxLayout *firstLayout; /*!< Layout at the top of the widget that aggregates lineID and versionButton */
    bool m_firstCall; /*!< Boolean that ensures we insert all the widgets only once, in other words, we don't create new widgets when the scenarios changes */

signals:
    void versionButtonSignal();  /*!< Signal emitted when the version button is clicked */
    void submitButtonSignal(); /*!< Signal emitted when the submit button is clicked */


private slots:
    void versionButtonClicked(); /*!< Internal slot use to emit version button signal*/
    void submitButtonClicked();  /*!< Internal slot use to emit submit button signal*/

private:
    QLabel  labelDateCreat; /*!< Label before the creation date */
    QLabel  labelDateModif;/*!< Label before the last update */
    QLabel  dateModif; /*!< Label that shows the last update */
    QLabel  dateCreat; /*!< Label that shows the creation date */

    QPushButton *versionButton; /*!< Version button positionned after the LineID */

    virtual void loadCompleteInterface(bool readOnly) = 0; /*!< Load minimal interface depending on each strategy */




};

/**
 * \class ArticleStrategy
 * \brief [Inherited from InterfaceStrategy]  Concrete class that defines widgets to show an article
 *
 *
 */


class ArticleStrategy : public InterfaceStrategy
{
public:
    ArticleStrategy(const QString &id="", const QString &title="", const QString &text="", const QString &creat="00/00/0000", const QString &modif="00/00/0000");
    ~ArticleStrategy();

    void setText(const QString &t);

    virtual Dico getData() const;
    virtual void setData(const Dico &dataMap);

private:

    QTextEdit *textEdit;  /*!< Text editor at the middle of the widget */

    virtual void loadCompleteInterface(bool readOnly);


};

/**
 * \class MediaStrategy
 * \brief [Inherited from InterfaceStrategy] Concrete class that defines widgets to show a media
 *
 */

class MediaStrategy : public InterfaceStrategy
{
    Q_OBJECT
public:
    MediaStrategy(const QString &id="", const QString &title="",const  QString &description="",const QString &filename="", const QString &creat="00/00/0000", const QString &modif="00/00/0000");
    ~MediaStrategy();

    void setDescription(const QString &desc);
    void setFileName(const QString &f);
    virtual Dico getData() const;
    virtual void setData(const Dico &dataMap);

protected:
    void hideEvent(QHideEvent *event);

public slots:
    void chooseFileName();
    void play();

private:
    QLabel labelFile;  /*!< Label before the file */
    QLabel labelImage;  /*!< Label that shows an image */

    QTextEdit *textEdit;  /*!< Text editor at the middle of the widget */

    QLineEdit *fileEdit;  /*!< Line that shows the current file choosen */

    QPushButton *dialogButton;  /*!< Button to set a choose file */

    const QRegularExpression regex;  /*!< Regulax expression used to find file extension */

    QHBoxLayout *fileLayout;  /*!< Layout that aggregates labelFile and fileEdit */
    QWidget *imageDisplayer;  /*!< Widget used to show the image in the label labelImage */
    QHBoxLayout *imgLayout;  /*!< Layout used to show the imageDisplayer */
    QMediaPlayer *mediaplayer; /*!< Play a media, such as a song or a video */
    QVideoWidget *videoDisplayer; /*!< Displays the video */
    QPushButton *playButton; /*!< Button to play or to pause the mediaplayer */

    virtual void loadCompleteInterface(bool readOnly);
    void setFormatedWidget(const QString &filename);

};

/**
 * \class TaskStrategy
 * \brief [Inherited from InterfaceStrategy] Concrete class that defines widgets to show a task
 *
 */

class TaskStrategy: public InterfaceStrategy
{
    Q_OBJECT

public:
    TaskStrategy(const QString &id="",const QString &title="",const QString &action="", int prio=0, const QDateTime &deadL=QDateTime(QDateTime::fromString("2000-01-01T00:00:00Z")), const QString &creat="00/00/0000", const QString &modif="00/00/0000");
    ~TaskStrategy();

    void setAction(const QString &id);
    void setPriority(const int &prio);
    void setDateTime(const QDateTime &dateTime);

    virtual Dico getData() const;
    virtual void setData(const Dico &dataMap);
public slots:

    void deadLineCheckChanged(int state);  /*!< Checks if the deadline ComboBox is checked */
    void prioCheckChanged(int state); /*!< Checks if the priority ComboBox is checked */


private:
    QLabel labelPrio; /*!< Label before the priority */
    QLabel labelDeadline; /*!< Layout before the deadline */

    QTextEdit *textEdit; /*!< Text editor at the middle of the widget */

    QDateTimeEdit *deadlineEdit; /*!< Widget used to show and edit the deadline */

    QComboBox *prioEdit; /*!< Widget used to show and edit the priority */

    QHBoxLayout *layoutDeadline;  /*!< Layout that aggregates labelDeadLine and deadLineEdit */
    QHBoxLayout *layoutPrio; /*!< Layout that aggregates labelPrio and prioEdit */

    QCheckBox *prioCheckBox; /*!< CheckBox that enables or disable the priority */
    QCheckBox *deadLineCheckBox;  /*!< CheckBox that enables or disable the deadline */


    virtual void loadCompleteInterface(bool readOnly);

};


#endif // STRAT_H
