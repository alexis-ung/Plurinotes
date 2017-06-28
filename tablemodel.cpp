#include "tablemodel.h"

/*
    C'est ici que se passe la "mise en forme" des données pour qu'elles correspondent à un tableau
    Soit les données sont codées en dur(pour rowCount et headerData) soit,
    elles font appel à la classe Note, via le membre mNote.
    Et c'est cette dernière qui va récuperer les données dans la bdd
*/


TableModel::TableModel() : m(NotesManager::getInstance())
{

}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    /*! Return the number of activated notes in the notemanager, in order to set the number of rows */

    return m.getRowCount(); // Getting the number of lines from the NotesManager

}

int TableModel::columnCount(const QModelIndex &parent) const
{
    /*! Return the number columns, which correspond to every type of notes: Task/Media/Article */

    Q_UNUSED(parent);
    return 3; //  (Task/Media/Article)
}

int TableModel::indexNextTask(QList<Note*> list, int i) const
{
    /*! Return the index in the Task column. */
    while(i < list.size() && list[i]->getType() != TaskType)
    {
        i++;
    }
    return i;
}


QVariant TableModel::data(const QModelIndex &index, int role) const
{
    /*! Return the note associated to the case at the index index */

    // Enables the view to get the data to show
    QVariant v;
    Dico mapToSend;

    NotesManager::iteratorNote it = m.beginNote();

    int i=0, j = 0;

    QList<Note*> sortedListNote;

    for(it = m.beginNote(); it!=m.endNote();it++)
    {
        if((*it)->getState() == active)
            sortedListNote.append(*it);

    }

    int k = 0;
    int l;
    bool swapped = true;

    while(swapped)
    {
        swapped = false;
        k++;
        for(l = 0;l<sortedListNote.size()-1;l++)
        {
            if(sortedListNote[l]->getType() == TaskType)
            {
                int nextIndex = indexNextTask(sortedListNote, l+1);
                Task *t1 = dynamic_cast<Task*>(sortedListNote[l]->getLastversion());
                Task *t2 = dynamic_cast<Task*>(sortedListNote[nextIndex]->getLastversion());

                if(t1->getDeadLine() < t2->getDeadLine())
                {
                    swapped = true;
                    sortedListNote.swap(l, nextIndex);
                }
            }
        }
    }


    for(i = 0;i<sortedListNote.count();i++)
    {
        if(index.column() == sortedListNote.at(i)->getType())
        {
            if(j == index.row())
                break;
            j++;
        }

    }
    if(i == sortedListNote.count())
        return QVariant();

     if(role == Qt::DisplayRole)
     {

         mapToSend["id"] = sortedListNote.at(i)->getId();
         mapToSend["title"] = sortedListNote.at(i)->getTitle();
         mapToSend["state"] = sortedListNote.at(i)->getState();
         mapToSend["creatDate"] = sortedListNote.at(i)->getCreationDateTime();
         mapToSend["lastUpdate"] = sortedListNote.at(i)->getLastversion()->getModifDate();

         switch(index.column())
         {
         case ArticleType:
         {
             Article *a = dynamic_cast<Article*>(sortedListNote.at(i)->getLastversion());
             mapToSend["text"] = a->getText();

             break;
         }
         case MediaType:
            {
             Media *a = dynamic_cast<Media*>(sortedListNote.at(i)->getLastversion());

             mapToSend["description"] = a->getDescription();
             mapToSend["filename"] = a->getFileName();
             break;
            }
         case TaskType:
            {

             Task *a = dynamic_cast<Task*>(sortedListNote.at(i)->getLastversion());
             mapToSend["action"] = a->getAction();
             mapToSend["priority"] = a->getPriority();
             mapToSend["deadLine"] = a->getDeadLine();
             break;
            }
         }

         v =qVariantFromValue((Dico) mapToSend);
         return v;
     }
     else if(role == Qt::SizeHintRole)
         return QSize();
     else
        return QVariant();


}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    /*! Defines the text and the style of the header columns */

    switch(role){

    // Set the head title of every column
    case Qt::DisplayRole:
        if (orientation == Qt::Horizontal){
            switch(section){
            case ArticleType:
                return QString("Articles");
                break;
            case MediaType:
                return QString("Médias");
                break;
            case TaskType:
                return QString("Tâches");
                break;
           }
        }
        break;

    // Bold Font for header
    case Qt::FontRole:
        QFont boldFont;
        boldFont.setBold(true);
        return boldFont;
        break;
    }
    // Backgroundcolor
    if (role == Qt::BackgroundRole){
        QBrush darkCyanBackground(Qt::darkCyan);
        return darkCyanBackground;
    }
    return QVariant();

}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    /*! Create a new version of the note at the index index */

    Q_UNUSED(index);
    Q_UNUSED(role);
    Dico dataReceived = value.value<Dico>();

    for(NotesManager::iteratorNote it = m.beginNote(); it!=m.endNote();it++)
    {
        if((*it)->getId() == dataReceived["id"].toString())
        {
            (*it)->setTitle(dataReceived["title"].toString());
            (*it)->createVersion(dataReceived);
        }
    }
    return true;
}




