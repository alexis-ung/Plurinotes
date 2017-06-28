#ifndef COMMANDS_H
#define COMMANDS_H

#include <QtWidgets>
#include "notesmanager.h"

/**
 * \class ArchiveCommand
 * \brief [Inherited from QUndoCommand]  Class that implement the undo/redo module,
 * enables to archive a note
 *
 */

class ArchiveCommand : public QUndoCommand
{
public:
    ArchiveCommand(const QString &id);

    void undo() override;
    void redo() override;

private:
    NotesManager &m; /*!< Instance of the notemanager */
    QString m_id; /*!< Id of the note we want to archive */

};

/**
 * \class RestoreCommand
 * \brief [Inherited from QUndoCommand]  Class that implement the undo/redo module,
 * enables to restore an archived note
 *
 */

class RestoreCommand : public QUndoCommand
{
public:
    RestoreCommand(const QString &id);

    void undo() override;
    void redo() override;

private:
    NotesManager &m; /*!< Instance of the notemanager */
    QString m_id; /*!< Id of the note we want to archive */

};



#endif // COMMANDS_H
