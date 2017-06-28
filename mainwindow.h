#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "datamanager.h"
#include "tablemodel.h"
#include "delegue.h"
#include "notewid.h"
#include "newnotedialog.h"
#include "versiondisplayer.h"
#include "trash.h"
#include "commands.h"
#include "newrelationdialog.h"
#include "relationtreeview.h"
#include "relationview.h"

/*! \class MainWindow
 * \brief [Inherited from QMainWindow] Class that manages all the widgets of the project
 *
 * Aggregates all the widgets in in order to show them in the same window
 */

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
      MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
      void closeEvent(QCloseEvent *event);

private slots:
    void openDialog(NoteType type);
    void versionDisplayer();
    void updateNote();
    void deleteNote();
    void archiveNote();
    void restoreNote();
    void showBin();
    void emptyBin();
    void undo();
    void redo();
    void createCouple();
    void createRelation();
    void listArchiveClicked(QModelIndex idx);

    void selectionChangedInTable(const QModelIndex &current, const QModelIndex &previous);
    void selectionChangedInArchive(int row);
    void selectionChangedInRightWid(const QString &id);

private:
    void writeSettings();
    void readSettings();
    void loadArchive();
    void chooseStrategy(const NoteType &type, const Dico &dataMap);
    Dico getDataMapFromNote(Note *n);

     // Right part of the interface
    InterfaceStrategy *m_interface; /*!< Pointer to the current strategy used */
    ArticleStrategy *m_interfaceArt; /*!< Pointer to a widget that shows an article */
    MediaStrategy *m_interfaceMed; /*!< Pointer to a widget that shows a media */
    TaskStrategy *m_interfaceTask; /*!< Pointer to a widget that shows a task */
    QHBoxLayout *m_rightLayout; /*!< Layout that aggregates relation view and relation tree view, in the right part of the window  */

    // Left part ot the interface : grid
    QTableView *m_gridview; /*!< Table on the left used to show active notes */
    QItemSelectionModel *m_selectionModel; /*!< Used to manage the selected case of the table*/
    QListWidget *m_listArchive; /*!< List of the archived notes on the left part */
    QVBoxLayout *m_leftPart; /*!< Layout used to aggregate m_listArchive and m_gridview */
    QWidget *m_leftPartWid; /*!< Widget used to adapt the m_leftPart layout*/
    QWidget *m_rightPartWid;  /*!< Widget used to adapt the m_rightLayout layout*/
    QPushButton *retractRelationButton; /*!< Arrow button, used to show and hide the left part*/

    // Vertical Splitter
    QHBoxLayout *m_centralLayout; /*!< Main layout used to aggregates all the other widgets */


    // To interface with the data
    TableModel *m_model; /*!< Model used by m_gridview to define how the data is shown */

    RelationTreeView *m_relationstree; /*!< Tree on the right that shows all the relations of a note*/
    RelationView *m_relations; /*!< Table on the right that shows all the notes of a relation */

    //Instance of the manager
    NotesManager &m = NotesManager::getInstance(); /*!< Instance of the notemanager*/

    //Undo/redo feature
    QUndoStack m_undoStack; /*!< undostack used to implement the undo/redo module */

    //Boolean to flush bin at the end
    QAction *actionFlushBinBeforeQuit; /*!< Setting of the user, used in the save of context*/
};

#endif // MAINWINDOW_H
