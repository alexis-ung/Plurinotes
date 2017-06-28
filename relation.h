#ifndef RELATION_H
#define RELATION_H

#include "note.h"
#include <iostream>
#include <ctime>
#include <vector>
#include <set>
#include <QString>

using namespace std;

typedef pair<const Note*,const Note*> PairNotes;

/*! \class Couple
 * \brief A pair of two notes that can be labelled. Each Couple lives in a specific Relation and can be seen as an oriented couple is the associated Relation does.
 *
 */
class Couple {
public:
    //Necessary to be declared as a MetaType, data included has to be considered as inoperant
    Couple();
    Couple(const Note * first,const Note* sec,const QString& l="")
    :m_pair(make_pair(first,sec)),m_label(l) {} /*!< The canonical constructor of a couple. The label is optionnal.*/

    // Getters
    const Note * getAsc() const { return m_pair.first;} /*!< Returns the ascendant note of the couple.*/
    const QString getIdAsc() const {return m_pair.first->getId();} /*!< Returns the ID of the ascendant note of the couple.*/
    const Note * getDesc() const { return m_pair.second;} /*!< Returns the descendant note of the couple.*/
    const QString getIdDesc() const {return m_pair.second->getId();} /*!< Returns the ID of the descendant note of the couple.*/

    const QString& getLabel() const { return m_label;} /*!< Returns the label of the couple.*/

    // Setters
    void setLabel(const QString& label) {m_label =label;} /*!< Setter for the label of the couple.*/

    void debugPrintInfo() const;

    bool operator==(const Couple& c) const { return (getAsc()==c.getAsc() && getDesc()==c.getDesc());} /*!< Redefinition of the '=' operator.*/
private:

    const PairNotes  m_pair; /*!< The pair containing the two Notes. The first is often called Asc and the second Desc.*/
    QString m_label; /*!< Optionnal label for the couple. */
};

/*! \class Relation
 * \brief A relation is defined as a subset of the cartesian product of the set of Note with itself.
 * A relation contains a QSet of couple, a name (that is unique), a description and it can be oriented.
 * There exists a specified Relation of name 'Référence' that is oriented.
 *
 */
class Relation {
public:
    Relation(const QString& title,const QString& description,bool isOriented=true)
      :m_name(title),m_description(description),m_isOriented(isOriented){} /*!< The canonical constructor of a couple. isOriented is by default at True.*/

    ~Relation(){ m_couples.clear(); } /*!< Destructor of the relation : erase all the couples */

    // Getters
    const QString& getName() const { return m_name;} /*!< Returns the name of the relation.*/
    const QString& getDescription() const { return m_description;} /*!< Returns the description of the relation.*/
    bool isOriented() const { return m_isOriented;} /*!< Returns true if the relation isOriented.*/


    void createCouple(const Note * first,const Note* sec,const QString& l="",bool saveInDB = true);
    Couple * getCouple(const Note * noteAsc,const Note * noteDesc); /*!< Returns the couple.*/
    const Couple * getCouple(const Note * noteAsc,const Note * noteDesc) const ; /*!< Returns the couple.*/


    // Setters
    void setDescription(const QString& description) {m_description = description;} /*!< Setter for the name of the relation.*/
    void setOrientation(bool orientation) {m_isOriented = orientation;}  /*!< Setter for the orientation of the relation.*/


    Relation& operator<<(Couple * toAddCouple) { m_couples<<toAddCouple; return *this;} /*!< A way to add couple in the relation.*/

    void debugPrintCouples() const;

    /** Iterator on the couples of the relation. Adapted from QSet<Couple*>::iterator via our custom iterators. */
    typedef customIterator::iterator<Couple, QSet<Couple*>,Relation> iterator;
    iterator begin() { return iterator(m_couples.begin()); } /*!< Returns a iterator of versions set on the most recent version. */
    iterator end() { return iterator(m_couples.end()); } /*!< Returns a iterator of versions set on the most recent version. */
    iterator erase(const iterator& it) { return iterator(m_couples.erase(it.m_iterator)); } /*!< Erase a couple based on an iterator pointing on it. */

    iterator deleteCouple(Couple * coupleToDel);

    /** Const Iterator on the couples of the relation. Adapted from QSet<Couple*>::const_iterator via our custom iterators. */
    typedef customIterator::const_iterator<Couple, QSet<Couple*>,Relation> const_iterator;
    const_iterator cbegin() const { return const_iterator(m_couples.begin()); } /*!< Returns a iterator of versions set on the most recent version. */
    const_iterator cend() const { return const_iterator(m_couples.end()); } /*!< Returns a iterator of versions set on the most recent version. */


private:
    const QString m_name; /*!< Name of the relation. Const since it cannot be changed. */
    QString m_description; /*!< Description of the relation. */
    bool m_isOriented; /*!< Indicates if the relation is oriented or not. */
    QSet<Couple*> m_couples; /*!< Couples' set. */
};

#endif // RELATION_H
