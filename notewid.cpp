#include "notewid.h"

const QString DATEFORMAT = "yyyy-MM-dd hh:mm:ss";


InterfaceStrategy::InterfaceStrategy(const QString &id, const QString &title, const QString &creat, const QString &modif)
    : m_firstCall(true), labelDateCreat("Création: "),  labelDateModif("Dernière modification: "), dateModif(""), dateCreat("")
{
    /*! Defines the interface common between all the notes */
    setMinimumSize(564, 477);

    //Shape the widget
    lineID = new QLineEdit;
    versionButton = new QPushButton("Afficher les versions");
    versionButton->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_V));

    firstLayout = new QHBoxLayout;
    firstLayout->addWidget(lineID);
    firstLayout->addWidget(versionButton);

    datesLayout = new QHBoxLayout;
    datesLayout->addWidget(&labelDateCreat);
    datesLayout->addWidget(&dateCreat);
    datesLayout->addWidget(&labelDateModif);
    datesLayout->addWidget(&dateModif);

    titleEdit = new QLineEdit();

    submitButton = new QPushButton("");
    submitButton->setDisabled(true);

    layout = new QVBoxLayout(this);
    layout->addLayout(firstLayout);
    layout->addWidget(titleEdit);
    layout->addLayout(datesLayout);
    layout->addWidget(submitButton);

    setTitle(title);
    setID(id);
    setDateCreation(creat);
    setDateModif(modif);

    //Connect buttons in order to send signals outside of the widget
    connect(versionButton, SIGNAL(clicked(bool)), this, SLOT(versionButtonClicked()));
    connect(submitButton, SIGNAL(clicked(bool)), this, SLOT(submitButtonClicked()));

    setLayout(layout);
    connect(titleEdit, &QLineEdit::textEdited, this, [=]{submitButton->setDisabled(false);});

}

InterfaceStrategy::~InterfaceStrategy()
{
}

void InterfaceStrategy::loadRestoreInteface()
{
    /*! Scenario of the interface that enables to restore notes */
    submitButton->setText("Restaurer");
    versionButton->hide();

    titleEdit->setReadOnly(true);
    lineID->setDisabled(true);
    submitButton->setDisabled(false);

    loadCompleteInterface(true);

}

void InterfaceStrategy::loadAddingInterface()
{
    /*! Scenario of the interface that enables to add notes */
    versionButton->hide();
    submitButton->setText("Ajouter");
    labelDateCreat.hide();
    labelDateModif.hide();
    dateCreat.hide();
    dateModif.hide();
    lineID->setPlaceholderText("ID");
    titleEdit->setPlaceholderText("Titre");


    loadCompleteInterface(false);
}

void InterfaceStrategy::loadDefaultInterface()
{
   /*! Scenario of the interface that enables to modify notes, used by default by the mainwindow */
    submitButton->setText("Modifier");
    versionButton->show();
    lineID->setDisabled(true);

    loadCompleteInterface(false);
}

//Getters and setters
void InterfaceStrategy::setID(const QString &id)
{
    /*! Setter for the ID */
    lineID->setText(id);
}

void InterfaceStrategy::setTitle(const QString &t)
{
    /*! Setter for the title */
    titleEdit->setText(t);
}

QString InterfaceStrategy::getID() const
{
    /*! Getter for the ID */
    return lineID->text();
}

QString InterfaceStrategy::getTitle() const
{
    /*! Getter for the title */
    return titleEdit->text();
}

void InterfaceStrategy::setDateCreation(const QString &t)
{
    /*! Setter for the creation date */
    dateCreat.setText(t);
}

void InterfaceStrategy::setDateModif(const QString &t)
{
    /*! Setter for the last modification date */
    dateModif.setText(t);
}

void InterfaceStrategy::disableSubmitButton()
{
    /*! Disable the button, useful to forbid modification of a note */
    submitButton->setDisabled(true);
}

bool InterfaceStrategy::isSubmitButtonEnabled()
{
    /*! Returns a boolean stating if the submit button is enabled */
    return submitButton->isEnabled();
}

void InterfaceStrategy::setData(const Dico &dataMap)
{
    /*! Setter for all the data of a widget, with a dictionnary of QVariant */
    setTitle(dataMap["title"].toString());
    setID(dataMap["id"].toString());
    setDateCreation(dataMap["creatDate"].toString());
    setDateModif(dataMap["lastUpdate"].toString());
}

//Emit signals
void InterfaceStrategy::versionButtonClicked()
{
    /*! Signal emitted when the versionButton is clicked */
    emit versionButtonSignal();
}

void InterfaceStrategy::submitButtonClicked()
{
     /*! Signal emitted when the submitButton is clicked */
        emit submitButtonSignal();
}




ArticleStrategy::ArticleStrategy(const QString &id, const QString &title, const QString &text, const QString &creat, const QString &modif)
    : InterfaceStrategy(id, title, creat, modif)
{

     /*! Defines the interface specific to an article */
    textEdit = new QTextEdit;

    //Show data
     setText(text);

    connect(textEdit, &QTextEdit::textChanged, this, [=]{submitButton->setDisabled(false);});
}

ArticleStrategy::~ArticleStrategy()
{
}

void ArticleStrategy::setText(const QString &t)
{
     /*! Setter for the text */
    textEdit->setText(t);
}

Dico ArticleStrategy::getData() const
{
     /*! Virtual pure function to get all the data of a widget, through a dictionnary */
    //Return the data
    Dico dataToSend;
    dataToSend["id"] = getID();
    dataToSend["title"] = getTitle();
    dataToSend["text"] = textEdit->toPlainText();

    return dataToSend;
}

void ArticleStrategy::setData(const Dico &dataMap)
{
     /*! Virtual function to fill all the interface */
    InterfaceStrategy::setData(dataMap);
    setText(dataMap["text"].toString());
}

void ArticleStrategy::loadCompleteInterface(bool readOnly)
{
     /*! Virtual pure function which displays the minimal interface specific to each type of note */
    textEdit->setReadOnly(readOnly);

    if(m_firstCall)
    {
        layout->insertWidget(layout->count()-1,textEdit);
        m_firstCall = false;
    }
}



MediaStrategy::MediaStrategy(const QString &id,const QString &title,const QString &description,const QString &filename, const QString &creat, const QString &modif) :
    InterfaceStrategy(id, title, creat, modif), labelFile("Fichier: "), labelImage(filename), regex(".(mp3|mp4|avi|jpg|png)$"),
    imageDisplayer(nullptr), imgLayout(nullptr), mediaplayer(nullptr), videoDisplayer(nullptr), playButton(nullptr)
{
     /*! Defines the interface specific to an article */
    textEdit = new QTextEdit;
    fileEdit = new QLineEdit;
    dialogButton = new QPushButton();
    dialogButton->setIcon(QIcon::fromTheme("folder"));
    connect(dialogButton, SIGNAL(clicked(bool)), this, SLOT(chooseFileName()));

    fileLayout = new QHBoxLayout;
    fileLayout->addWidget(&labelFile);
    fileLayout->addWidget(fileEdit);
    fileLayout->addWidget(dialogButton);


    imgLayout = new QHBoxLayout;
    imageDisplayer = new QWidget;
    mediaplayer = new QMediaPlayer;
    videoDisplayer = new QVideoWidget;
    playButton = new QPushButton;

    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));



    //Show data
    setDescription(description);
    setFileName(filename);

    connect(textEdit, &QTextEdit::textChanged, this, [=]{submitButton->setDisabled(false);});
    connect(fileEdit, &QLineEdit::textChanged, this, [=]{submitButton->setDisabled(false);});
    connect(playButton, &QAbstractButton::clicked,this, &MediaStrategy::play);

    setFormatedWidget(filename);

}

MediaStrategy::~MediaStrategy()
{
    /*! Destructor of MediaStrategy, stop the media if not already done */

    if(mediaplayer !=nullptr)
    {
        mediaplayer->stop();
    }
}

void MediaStrategy::setDescription(const QString &desc)
{
     /*! Setter for the description */
    textEdit->setText(desc);
}

void MediaStrategy::setFileName(const QString &f)
{
     /*! Setter for the fileEdit and load the media */
    fileEdit->setText(f);
    labelImage.setPixmap(QPixmap(fileEdit->text()));// A suppr?

}

Dico MediaStrategy::getData() const
{
    /*! Virtual pure function to get all the data of a widget, through a dictionnary */

    //Return the data
    Dico dataToSend;
    dataToSend["id"] = getID();
    dataToSend["title"] = getTitle();
    dataToSend["description"] = textEdit->toPlainText();
    dataToSend["filename"] = fileEdit->text();

    return dataToSend;
}

void MediaStrategy::setData(const Dico &dataMap)
{
    /*! Virtual function to fill all the interface */
    InterfaceStrategy::setData(dataMap);
    setDescription(dataMap["description"].toString());
    setFileName(dataMap["filename"].toString());
    setFormatedWidget(dataMap["filename"].toString());
}

void MediaStrategy::hideEvent(QHideEvent *event)
{
    /*! The media is paused when the widget is not shown */

    Q_UNUSED(event);
    mediaplayer->stop();
    videoDisplayer->hide();
}

void MediaStrategy::chooseFileName()
{
    /*! Find the format of the media selected and plays the media */
    //Load the file and display it
    fileEdit->setText(QFileDialog::getOpenFileName(this));
    setFormatedWidget(fileEdit->text());
}

void MediaStrategy::play()
{
    /*! Play or pause the media and change the icon */

    switch(mediaplayer->state())
    {
    case QMediaPlayer::PlayingState:
        mediaplayer->pause();
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    default:
        mediaplayer->play();
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    }
}



void MediaStrategy::loadCompleteInterface(bool readOnly)
{
    /*! Virtual pure function which displays the minimal interface specific to each type of note */

    textEdit->setReadOnly(readOnly);
    fileEdit->setReadOnly(readOnly);
    dialogButton->setDisabled(readOnly);

    if(m_firstCall)
    {

        layout->insertWidget(layout->count()-1,imageDisplayer);
        layout->setAlignment(imageDisplayer, Qt::AlignHCenter);
        layout->insertWidget(layout->count()-1, playButton);
        //layout->insertWidget(layout->count()-1,videoDisplayer);

        //Insert the specific widget before the submit button

        layout->insertWidget(layout->count()-1,textEdit);
        layout->insertLayout(layout->count()-1,fileLayout);
        m_firstCall = false;
    }
}

void MediaStrategy::setFormatedWidget(const QString &filename)
{
    /*! Find the format of the media selected and plays the media */

    QRegularExpressionMatchIterator it = regex.globalMatch(filename);

    QString formatMatched;
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        formatMatched = match.captured();
    }

    mediaplayer->stop();


    if((formatMatched == ".jpg") || (formatMatched == ".png"))
    {
        videoDisplayer->hide();
        playButton->hide();
        imageDisplayer->show();

        imgLayout->addWidget(&labelImage);
        imageDisplayer->setLayout(imgLayout);
    }
    else if((formatMatched == ".mp3") || (formatMatched == ".wav"))
    {
        videoDisplayer->hide();
        imageDisplayer->hide();        
        playButton->show();

        mediaplayer->setMedia(QUrl::fromLocalFile(filename));
        mediaplayer->setVolume(50);
        mediaplayer->play();
    }
    else if((formatMatched == ".avi") || (formatMatched == ".mp4"))
    {

        imageDisplayer->hide();
        videoDisplayer->show();
        playButton->show();
        mediaplayer->setMedia(QUrl::fromLocalFile(filename));
        mediaplayer->setVideoOutput(videoDisplayer);
        mediaplayer->setVolume(50);
        mediaplayer->play();

    }
}

TaskStrategy::TaskStrategy(const QString &id, const QString &title, const QString &action, int prio, const QDateTime &deadL, const QString &creat, const QString &modif)
    : InterfaceStrategy(id, title, creat, modif),labelPrio("Priorité: "), labelDeadline("Deadline: ")
{
    /*! Defines the interface specific to a task */
    textEdit = new QTextEdit;
    deadlineEdit = new QDateTimeEdit;
    prioEdit = new QComboBox;

    prioEdit->addItem("Aucune", 0);
    prioEdit->addItem("Très faible", 1);
    prioEdit->addItem("Faible", 2);
    prioEdit->addItem("Moyenne", 3);
    prioEdit->addItem("Forte", 4);
    prioEdit->addItem("Très forte", 5);

    //Shape the widget

    prioCheckBox = new QCheckBox;
    deadLineCheckBox = new QCheckBox;

    layoutDeadline = new QHBoxLayout;
    layoutDeadline->addWidget(&labelDeadline);
    layoutDeadline->addWidget(deadLineCheckBox);
    layoutDeadline->addWidget(deadlineEdit);

    layoutPrio = new QHBoxLayout;
    layoutPrio->addWidget(&labelPrio);
    layoutPrio->addWidget(prioCheckBox);
    layoutPrio->addWidget(prioEdit);

    prioEdit->setEnabled(false);
    deadlineEdit->setDisabled(true);

    //Show the data
    setAction(action);
    setDateTime(deadL);
    setPriority(prio);

    connect(textEdit, &QTextEdit::textChanged, this, [=]{submitButton->setDisabled(false);});
    connect(deadlineEdit, &QDateTimeEdit::dateTimeChanged, this, [=]{submitButton->setDisabled(false);});
    connect(prioEdit, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](){ submitButton->setDisabled(false);});
    connect(prioCheckBox, SIGNAL(stateChanged(int)), this, SLOT(prioCheckChanged(int)));
   // connect(deadLineCheckBox, &QCheckBox::stateChanged, this, [=]{});
    connect(deadLineCheckBox, SIGNAL(stateChanged(int)), this, SLOT(deadLineCheckChanged(int)));

    // [=]{prioEdit->setDisabled(!prioCheckBox->isChecked()); setPriority(0);}
}

TaskStrategy::~TaskStrategy()
{
}

void TaskStrategy::setAction(const QString &id)
{
    /*! Setter for the action */
    textEdit->setText(id);
}

void TaskStrategy::setDateTime(const QDateTime &dateTime)
{
    /*! Setter for the deadline */
    deadlineEdit->setDateTime(dateTime);
}

void TaskStrategy::setPriority(const int &prio)
{
    /*! Setter for the priority */
    prioEdit->setCurrentIndex(prio);
}

Dico TaskStrategy::getData() const
{
    /*! Virtual pure function to get all the data of a widget, through a dictionnary */

    //Return the data
    Dico dataToSend;
    dataToSend["id"] = getID();
    dataToSend["title"] = getTitle();
    dataToSend["action"] = textEdit->toPlainText();
    dataToSend["deadLine"] = deadlineEdit->dateTime();
    dataToSend["priority"] = prioEdit->currentIndex();

    return dataToSend;
}

void TaskStrategy::setData(const Dico &dataMap)
{
    /*! Virtual function to fill all the interface */

    InterfaceStrategy::setData(dataMap);
    setAction(dataMap["action"].toString());
    setPriority(dataMap["priority"].toInt());
    setDateTime(dataMap["deadLine"].toDateTime());

    if(dataMap["priority"].toInt() != 0)
    {
        prioCheckBox->setChecked(true);
        prioEdit->setEnabled(true);

    }
    else
    {
        prioCheckBox->setChecked(false);
        prioEdit->setDisabled(true);
    }


    if(dataMap["deadLine"].toDateTime().toString(DATEFORMAT) != "2000-01-01 00:00:00")
    {
        deadLineCheckBox->setChecked(true);
    }
    else
    {
        deadLineCheckBox->setChecked(false);
    }



}

void TaskStrategy::deadLineCheckChanged(int state)
{
    Q_UNUSED(state);

    submitButton->setDisabled(false);
    deadlineEdit->setDisabled(!deadLineCheckBox->isChecked());
    if(!deadLineCheckBox->isChecked())
    {
        setDateTime(QDateTime::fromString("2000-01-01 00:00:00",DATEFORMAT));
    }
}

void TaskStrategy::prioCheckChanged(int state)
{
    Q_UNUSED(state);

    submitButton->setDisabled(false);
    prioEdit->setDisabled(!prioCheckBox->isChecked());

    if(!prioCheckBox->isChecked())
    {
        setPriority(0);
    }

}


void TaskStrategy::loadCompleteInterface(bool readOnly)
{
    /*! Virtual pure function which displays the minimal interface specific to each type of note */

    textEdit->setReadOnly(readOnly);
    deadlineEdit->setReadOnly(readOnly);
    prioEdit->setDisabled(readOnly);

    if(m_firstCall)
    {

        //Insert the specific widget before the submit button
        layout->insertWidget(layout->count()-1, textEdit);
        layout->insertLayout(layout->count()-1,layoutDeadline);
        layout->insertLayout(layout->count()-1,layoutPrio);
        m_firstCall = false;
    }
}
