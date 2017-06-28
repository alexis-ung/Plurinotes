#include "mainwindow.h"

const QString DATEFORMAT = "yyyy-MM-dd hh:mm:ss";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    /*!
     *Constructor of the MainWindow.
     * Aggregates all the widget used by plurinotes. Defines the menu bar.
     * Connect some of the widgets with slots in order to make them communicate together
     *  */
    setWindowTitle("PluriNotes");

    // Left-tab that displays note order by type in columns
    m_gridview = new QTableView;
    m_listArchive = new QListWidget;
    m_leftPart = new QVBoxLayout;
    m_leftPart->addWidget(m_gridview);

    QLabel *noteArchive = new QLabel("Notes archivées: ");

    m_leftPart->addWidget(noteArchive);
    m_leftPart->addWidget(m_listArchive);

    //Set the proportion of the left layout
    connect(m_listArchive,SIGNAL(currentRowChanged(int)), this, SLOT(selectionChangedInArchive(int)));
    connect(m_listArchive, SIGNAL(activated(QModelIndex)), this, SLOT(listArchiveClicked(QModelIndex)));


    loadArchive();

    // Classical top bar menu
    QMenu *noteMenu = menuBar()->addMenu(tr("&Note"));
    QMenu *relationMenu = menuBar()->addMenu(tr("&Relation"));
    QMenu *editMenu = menuBar()->addMenu(tr("&Edition"));
    QMenu *binMenu = menuBar()->addMenu(tr("&Corbeille"));


    QAction *newArtAction = noteMenu->addAction("Nouvel article");
    newArtAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A));
    connect(newArtAction, &QAction::triggered, this, [this]{openDialog(ArticleType);});

    QAction *newMedAction = noteMenu->addAction("Nouveau media");
    newMedAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_M));
    connect(newMedAction, &QAction::triggered, this, [this]{openDialog(MediaType);});

    QAction *newTaskAction = noteMenu->addAction("Nouvelle tâche");
    newTaskAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
    connect(newTaskAction, &QAction::triggered, this, [this]{openDialog(TaskType);});



    QAction *newCoupleAction = relationMenu->addAction("Nouveau couple");
    newCoupleAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_C));
    connect(newCoupleAction,SIGNAL(triggered(bool)), this, SLOT(createCouple()));

    QAction *newRelationAction = relationMenu->addAction("Nouvelle relation");
    newRelationAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_R));
    connect(newRelationAction, SIGNAL(triggered(bool)), this, SLOT(createRelation()));

    relationMenu->addSeparator();

    QAction *actionTreeDisplay = relationMenu->addAction("Affichage de l'arborescence");
    connect(actionTreeDisplay,&QAction::triggered,this,[=]{
        m_relations->hide();
        m_relationstree->show();
    });


    QAction *actionTabDisplay = relationMenu->addAction("Affichage du tableau des couples d'une relation");
    connect(actionTabDisplay,&QAction::triggered,this,[=]{
        m_relationstree->hide();
        m_relations->show();
    });

    QAction *actionUndo = editMenu->addAction("Annuler");
    actionUndo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
    connect(actionUndo, SIGNAL(triggered(bool)), this, SLOT(undo()));

    QAction *actionRedo = editMenu->addAction("Rétablir");
    actionRedo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));
    connect(actionRedo, SIGNAL(triggered(bool)), this, SLOT(redo()));

    editMenu->addSeparator();

    QAction *actionArchive = editMenu->addAction("Archiver");
    actionArchive->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    connect(actionArchive, SIGNAL(triggered(bool)), this, SLOT(archiveNote()));

    QAction *actionRemove = editMenu->addAction("Supprimer");
    actionRemove->setShortcut(Qt::Key_Delete);
    connect(actionRemove, SIGNAL(triggered(bool)), this, SLOT(deleteNote()));

    QAction *actionShowBin = binMenu->addAction("Afficher la corbeille");
    actionShowBin->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_B);
    connect(actionShowBin, SIGNAL(triggered(bool)), this, SLOT(showBin()));

    QAction *actionEmptyBin = binMenu->addAction("Vider la corbeille");
    connect(actionEmptyBin, SIGNAL(triggered(bool)), this, SLOT(emptyBin()));

    actionFlushBinBeforeQuit = binMenu->addAction("Vider la corbeille en partant");
    actionFlushBinBeforeQuit->setCheckable(true);
    actionFlushBinBeforeQuit->setChecked(false);

    QAction *actionSave = new QAction();
    actionSave->setShortcut(QKeySequence(Qt::CTRL + + Qt::Key_S));
    connect(actionSave, SIGNAL(triggered(bool)), this, SLOT(updateNote()));
    addAction(actionSave);

    binMenu->addSeparator();

    // Interfacing with the data
    m_model = new TableModel;
    m_gridview->setModel(m_model);

    m_selectionModel = m_gridview->selectionModel(); // must be after setModel

    Delegue *delegate = new Delegue(this);
    m_gridview->setItemDelegate(delegate);

    m_gridview->resizeRowsToContents();      // necessary to use size defined in the delegate
    m_gridview->resizeColumnsToContents(); // necessary to use size defined in the delegate

    resize(500,500);
    setMinimumSize(1000,500);

    //Only one item can be selectable
    m_gridview->setSelectionMode(QAbstractItemView::SingleSelection);

    //Adjust the size of the view to the size of the left part or the app
    m_gridview->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //selectionChangedInTable() is called when an item is selected
    connect(m_selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(selectionChangedInTable(QModelIndex, QModelIndex)));

    //Improve display performance
    m_interfaceArt = new ArticleStrategy();
    m_interfaceArt->hide();

    m_interfaceMed = new MediaStrategy();
    m_interfaceMed->hide();

    m_interfaceTask = new TaskStrategy();
    m_interfaceTask->hide();

    m_interface = m_interfaceArt;

    m_relations = new RelationView();
    m_relations->show();
    connect(m_relations, SIGNAL(selectIdSignal(QString)), this, SLOT(selectionChangedInRightWid(QString)));

    m_relationstree = new RelationTreeView(this,m_interface->getID());
    m_relationstree->show();
    connect(m_relationstree, SIGNAL(selectIdSignal(QString)), this, SLOT(selectionChangedInRightWid(QString)));

    // Showing up the main interface
    m_leftPartWid = new QWidget(this);
    m_leftPartWid->setLayout(m_leftPart);

    retractRelationButton = new QPushButton;
    retractRelationButton->setShortcut(Qt::CTRL + Qt::Key_Space);
    retractRelationButton->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    connect(retractRelationButton, &QPushButton::clicked, this,
        [&]
        {
            if(m_rightPartWid->isHidden())
            {
                m_rightPartWid->show();
                retractRelationButton->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
            }
            else
            {
                m_rightPartWid->hide();
                retractRelationButton->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
            }
        }
    );

    m_centralLayout = new QHBoxLayout();

    m_centralLayout->addWidget(m_leftPartWid);
    m_centralLayout->addWidget(m_interfaceArt);
    m_centralLayout->addWidget(m_interfaceMed);
    m_centralLayout->addWidget(m_interfaceTask);
    m_centralLayout->addWidget(retractRelationButton);

    m_rightPartWid = new QWidget;
    m_rightLayout = new QHBoxLayout;

    m_rightLayout->addWidget(m_relationstree);
    m_rightLayout->addWidget(m_relations);

    m_rightPartWid->setLayout(m_rightLayout);

    m_centralLayout->addWidget(m_rightPartWid);
    actionTreeDisplay->trigger();

    QWidget *mainWidget = new QWidget;
    mainWidget->setLayout(m_centralLayout);

    setCentralWidget(mainWidget);

    // Loading context
    readSettings();
}

MainWindow::~MainWindow()
{

}


void MainWindow::openDialog(NoteType type)
{

    /*! Open the dialog to create a new note */
    NewNoteDialog dia(type);
    dia.exec();                                        // we could return a value
    emit m_model->layoutChanged();        // Necessary to update the model and the view
    m_gridview->resizeRowsToContents(); // Necessary to have cases at the right size
}

void MainWindow::versionDisplayer()
{
    /*! Open the dialog to show old versions of a note */

    Dico dataMap = m_selectionModel->currentIndex().model()->data(m_selectionModel->currentIndex()).value<Dico>();
    if(m_interface->getID() != "")
    {
        VersionDialog dia(dataMap["id"].toString());
        dia.exec();
    }
    selectionChangedInTable(m_selectionModel->currentIndex(),QModelIndex());
}

void MainWindow::updateNote()
{
    /*! Send the data to the TableModel in order to create a new version of a note */

    if(m_interface->isSubmitButtonEnabled())
    {
        Dico dataToSend = m_interface->getData();
        QDateTime deadline = dataToSend["deadLine"].toDateTime();
        if((deadline != QDateTime(QDate(0,0,0), QTime(0,0))) && (deadline.toString(DATEFORMAT) != "2000-01-01 00:00:00") && (deadline < QDateTime::currentDateTime()))
        {
            QMessageBox::warning(this, "Problème deadline", "Deadline dans le passé");
            return;
        }

        const QVariant v =qVariantFromValue(dataToSend);

        const QModelIndex &ref = m_selectionModel->currentIndex();
        m_model->setData(ref, v);
        selectionChangedInTable(m_selectionModel->currentIndex(), QModelIndex());
        m_interface->disableSubmitButton();
        m_relations->addCoupleRelation(m_relations->getCurrentRelation());
        loadArchive();
        emit m_model->layoutChanged();
    }

}

void MainWindow::deleteNote()
{
    /*! Delete a note */

    if(m_interface->getID() != "")
        m.deleteNote(m_interface->getID());

    selectionChangedInTable(m_selectionModel->currentIndex(), QModelIndex()); // Update the note at the new position
    loadArchive(); // Update the archive liste
    m_model->layoutChanged();

}

void MainWindow::archiveNote()
{
    /*! Archive a note */


    //Test if the widget is not empty and if the note is not already archived
    if(m_interface->getID() != "" && (m.findNote(m_interface->getID())->getState() == active))
    {
        m_undoStack.push(new ArchiveCommand(m_interface->getID()));
        m_listArchive->addItem(m_interface->getID() + " - " + m_interface->getTitle());
        emit m_model->layoutChanged();
        selectionChangedInTable(m_selectionModel->currentIndex(), QModelIndex()); // Update the note at the new position
    }
}

void MainWindow::restoreNote()
{

    /*! Restore a note from archive */

    m_undoStack.push(new RestoreCommand(m_interface->getID()));
    loadArchive();
    emit m_model->layoutChanged();
    m_gridview->resizeRowsToContents();

}

void MainWindow::showBin()
{
    /*! Displays the dustbin */

    Trash dia;
    dia.exec();

    //Reload the tableview
    emit m_model->layoutChanged();
    m_gridview->resizeRowsToContents();


}

void MainWindow::emptyBin()
{
    /*! Empty the bin */

    m.emptyBin();
}


void MainWindow::undo()
{
    /*! Undo the last command pushed on the stack */

    m_undoStack.undo();
    loadArchive();
    m_model->layoutChanged();
}

void MainWindow::redo()
{
    /*! Redo te last command undone */

    m_undoStack.redo();
    loadArchive();
    m_model->layoutChanged();
}

void MainWindow::createCouple()
{
    /*! Open the dialog to create a couple */

    NewCoupleDialog cp;
    cp.exec();
    m_relations->addCoupleRelation(m_relations->getRelation());
    m_relationstree->setId(m_interface->getID());
}

void MainWindow::createRelation()
{
    /*! Open the dialog to create a relation */

    NewRelationDialog relat;
    relat.exec();
}

void MainWindow::selectionChangedInArchive(int row)
{
    /*! Displays the selected note in the central interface
     * Called when user selected a row in the archived note list */

    Q_UNUSED(row);

    m_selectionModel->clearSelection();
    m_relationstree->clearSelection();

    if(m_listArchive->currentItem() == nullptr)
        return;

    QString id = m_listArchive->currentItem()->text().split(" - ").at(0);

    Note *n = m.findNote(id);
    if(n == nullptr)
        return;

    Dico dataMap = getDataMapFromNote(n);
    chooseStrategy(n->getType(), dataMap);

    m_interface->loadRestoreInteface();
    m_relationstree->setId(m_interface->getID());

    //Disconnect old slots that may be used by an other interface
    disconnect(m_interface, SIGNAL(submitButtonSignal()), this, SLOT(restoreNote()));
    disconnect(m_interface, SIGNAL(submitButtonSignal()), this, SLOT(updateNote()));

    //Connect the slot necessay to this interface
    connect(m_interface, SIGNAL(submitButtonSignal()), this, SLOT(restoreNote()));
}


void MainWindow::selectionChangedInTable(const QModelIndex &current, const QModelIndex &previous)
{
    /*! Displays the selected note in the central interface
     * Called when user selected a case in the table */

    Q_UNUSED(previous);

    //Cette fonction récupère les pointeurs sur note, puis recupère la dernière version et envoie les données brutes aux widgets

    m_listArchive->clearSelection();
    m_relationstree->clearSelection();

    Dico dataMap = current.model()->data(current).value<Dico>();

    chooseStrategy(static_cast<NoteType>(current.column()), dataMap);

    //Disconnect old slots that may be used by an other interface
    disconnect(m_interface, SIGNAL(submitButtonSignal()), this, SLOT(updateNote()));
    disconnect(m_interface, SIGNAL(submitButtonSignal()), this, SLOT(restoreNote()));
    disconnect(m_interface, SIGNAL(versionButtonSignal()), this, SLOT(versionDisplayer()));

    //Connect slots necessay to the default interface
    connect(m_interface, SIGNAL(submitButtonSignal()), this, SLOT(updateNote()));
    connect(m_interface, SIGNAL(versionButtonSignal()), this, SLOT(versionDisplayer()));

    m_interface->loadDefaultInterface();

    // In order for the selection to work
    //m_selectionModel->clearSelection();
    //m_selectionModel->select(current,QItemSelectionModel::Select);
}


void MainWindow::selectionChangedInRightWid(const QString &id)
{
    /*! Displays the selected note in the central interface
     * Called when user selected a note in the relation view or relation tree view */

    m_selectionModel->clearSelection();
    m_listArchive->clearSelection();

    Note *n = m.findNote(id);
    Dico dataMap = getDataMapFromNote(n);
    chooseStrategy(n->getType(), dataMap);

    m_interface->loadDefaultInterface();
}


void MainWindow::listArchiveClicked(QModelIndex idx)
{
    /*! Send the row of the selected note in the archive list to the function that selects the widget to show
    */
    selectionChangedInArchive(idx.row());
}


void MainWindow::writeSettings()
{
    /*! Save the context of the MainWindow */

    /* Write the current settings in the config file (used in at close event of the app)*/
    QSettings settings("JAJ & Co", "PluriNotes");

    settings.beginGroup("MainWindow");
        // MainWindow Size
        settings.setValue("size", size());
        // MainWindow Position
        settings.setValue("pos", pos());
        // Central widget Geometry
        settings.setValue("interfaceGeometryRect",m_interface->geometry());
        // Left Part Widget Geometry
        settings.setValue("gridviewGeometryRect",m_leftPartWid->geometry());
        // Selection in gridview : doesn't work on Windows10
//        settings.setValue("gridviewCurrentIndex",m_selectionModel->currentIndex()); //Erreur qui fait planter sur Windows
        // Boolean for the bin
        settings.setValue("emptyBinBeforeQuit",actionFlushBinBeforeQuit->isChecked());
        // Right pannel
        settings.setValue("m_rightPartWidHidden",m_rightPartWid->isHidden());
        // Relation windows in the pannel
        settings.setValue("relationsHidden",m_relations->isHidden());
    settings.endGroup();

}

void MainWindow::readSettings()
{
    /*! Load the context of the MainWindow */

    /* Read the current settings in the config file (used in at start of the app)*/
    QSettings settings("JAJ & Co", "PluriNotes");

    settings.beginGroup("MainWindow");
        // MainWindow Size
        resize(settings.value("size", QSize(1000, 500)).toSize());
        // MainWindow Position
        move(settings.value("pos", QPoint(200, 200)).toPoint());
        // Central widget Geometry
        m_interface->setGeometry(settings.value("interfaceGeometryRect",QRect()).toRect());
        // Left Part Widget Geometry
        m_leftPartWid->setGeometry(settings.value("gridviewGeometryRect",QRect()).toRect());
        // Selection in gridview : doesn't work on Windows10
//        m_selectionModel->setCurrentIndex(settings.value("gridviewCurrentIndex",QModelIndex()).toModelIndex(),QItemSelectionModel::Select);

        // Boolean for the bin
        actionFlushBinBeforeQuit->setChecked(settings.value("emptyBinBeforeQuit",false).toBool());

        // Right pannel
        if(settings.value("m_rightPartWidHidden",false).toBool()){
            m_rightPartWid->hide();
            retractRelationButton->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
        }
        else{
            m_rightPartWid->show();
            retractRelationButton->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
        }

        // Relation windows in the pannel
        if(settings.value("relationsHidden").toBool()){
            m_relations->hide();
            m_relationstree->show();
        }
        else{
            m_relationstree->hide();
            m_relations->show();
        }
    settings.endGroup();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    /*!
     *Function called when the user wants to close the app.
     * Ask the user if he wants to empty the dustbin if it is not empty and if the option is not selected
     *  */
    uint nbNotesBin = m.nbNotesInBin();
    if(actionFlushBinBeforeQuit->isChecked()){
        m.emptyBin();
    }
    else if(nbNotesBin != 0)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Corbeille", QString("Vider la corbeille de %1 notes?").arg(nbNotesBin), QMessageBox::Yes|QMessageBox::No);

        if(reply == QMessageBox::Yes)
        {
            m.emptyBin();
        }
    }

    writeSettings();
    event->accept();
}

void MainWindow::loadArchive()
{
    /*! Clear the archive list and displays all archived elements */

    m_listArchive->clear();
    for(NotesManager::const_iteratorNote it = m.cbeginNote();it!=m.cendNote();it++)
    {
        if((*it)->getState() == archive)
            m_listArchive->addItem(new QListWidgetItem((*it)->getId() + " - " + (*it)->getTitle()));
    }

}

void MainWindow::chooseStrategy(const NoteType &type, const Dico &dataMap)
{
    /*! Defines which widget must be displayed in terms of the type received
     *  Send the data of the selected note to this widget
    */

    m_interfaceArt->hide();
    m_interfaceMed->hide();
    m_interfaceTask->hide();

    switch(type)
     {
        case ArticleType:
        {
             m_interface = m_interfaceArt;
           break;

        }
        case MediaType:
        {
            m_interface = m_interfaceMed;
            break;
        }
        case TaskType:
        {
            m_interface = m_interfaceTask;
            break;
        }
    case EmptyType:
    default:
        break;
     }
    m_interface->show();
    // Set the current geometry of the object
    m_interface->setData(dataMap);
    m_interface->disableSubmitButton();
    if(m_interface->getID() != "")
        m_relationstree->setId(m_interface->getID());
}

Dico MainWindow::getDataMapFromNote(Note *n)
{
    /*!
     * Get the data of a note and put it into a Dico
     *
    */

    Dico dataMap;
    dataMap["id"] = n->getId();
    dataMap["title"] = n->getTitle();
    dataMap["creatDate"] = n->getCreationDateTime().toString(DATEFORMAT);
    dataMap["lastUpdate"] = n->getLastversion()->getModifDate().toString(DATEFORMAT);
    switch(n->getType())
     {
        case ArticleType:
        {
           Article *a = dynamic_cast<Article*>(n->getLastversion());
           dataMap["text"] = a->getText();
           break;

        }
        case MediaType:
        {
            Media *med = dynamic_cast<Media*>(n->getLastversion());
            dataMap["description"] = med->getDescription();
            dataMap["filename"] = med->getFileName();

            break;
        }
        case TaskType:
        {
            Task *t = dynamic_cast<Task*>(n->getLastversion());
            dataMap["priority"] = t->getPriority();
            dataMap["action"] = t->getAction();
            dataMap["deadLine"] = t->getDeadLine();
            break;
        }
    case EmptyType:
    default:
        break;
     }

    return dataMap;
}
