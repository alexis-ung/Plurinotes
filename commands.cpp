#include "commands.h"

ArchiveCommand::ArchiveCommand(const QString &id) : m(NotesManager::getInstance())
{
    /*! Load the instance of the NoteManager and set the id received
     *  */
    m_id = id;
}

void ArchiveCommand::undo()
{
    /*!
     * Undo the action of archive a note
     *  */
    if(m.findNote(m_id)->getState() == archive)
        m.changeState(m_id, active);
}

void ArchiveCommand::redo()
{
    /*!
     *Archive a note
     *  */
    if(m.findNote(m_id)->getState() == active)
        m.changeState(m_id, archive);
}

RestoreCommand::RestoreCommand(const QString &id) : m(NotesManager::getInstance())
{
    /*! Load the instance of the NoteManager and set the id received
     *  */
    m_id = id;
}

void RestoreCommand::undo()
{
    /*!
     * Undo the action of restore an archived note
     *  */
    const Note * noteToRestore = m.findNote(m_id);
    if(noteToRestore->getState() == active){
        m.changeState(m_id, archive);
        noteToRestore->updateReferences();
    }
}

void RestoreCommand::redo()
{
    /*!
     * Restore an archived note
     *  */
    if(m.findNote(m_id)->getState() == archive)
        m.changeState(m_id, active);
}
