#include "delegue.h"

Delegue::Delegue(QWidget *parent)
{
    /*! Do nothing */
    Q_UNUSED(parent);
}

void Delegue::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    /*! Get the data of a note and defines the components to draw it */
    Dico dataMap = index.model()->data(index).value<Dico>();

    painter->setBrush(Qt::white);

    //Paint the back in white

    painter->setPen(Qt::white);
    painter->drawRect(option.rect);

    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());

    //Prepare the data to be painted in black
    QPen pen;

    pen.setBrush(Qt::black);

    painter->setPen(pen);

    //Align the title in the center
    QTextOption optTitle;
    optTitle.setAlignment(Qt::AlignHCenter);

    //Align the main text to the left
    QTextOption optMainText;
    optMainText.setAlignment(Qt::AlignLeft);

    QString titleToDraw= dataMap["title"].toString();;
    QString textToDraw;

    //Necessary to set text below the title
    QRect rectText = option.rect;
    rectText.setY(rectText.y()+30+(titleToDraw.size()/20)*30);





    //Get the text to draw in terms of the type of note
    switch(index.column())
    {
        case ArticleType:
        {
            textToDraw = dataMap["text"].toString();

            break;
        }
        case MediaType:
        {
            textToDraw = dataMap["description"].toString();
            break;
        }
        case TaskType:
        {
            textToDraw = dataMap["action"].toString();
            break;
        }
    }

    //Draw the text
    painter->drawText(option.rect, titleToDraw, optTitle);
    painter->drawText(rectText, textToDraw, optMainText);//
}

QSize Delegue::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    /*! Arbritrary size a case by default*/
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(100, 100);
}
