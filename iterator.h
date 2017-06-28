#ifndef ITERATOR_H
#define ITERATOR_H

namespace customIterator {
/*! \class iterator
 *  \brief Custom Template iterator. Adapted from a specific DataStructure::iterator.
 *
 */
template<class T,class DataStructure,class FriendClass>
class iterator{
    friend FriendClass; /*!< The friend class of the iterator. Generally the class containing the data structure */
    typename DataStructure::iterator m_iterator; /*!< The iterator to adapt */
    iterator(typename DataStructure::iterator it) : m_iterator(it) {} /*!< The adapter's constructor.*/
public:
    iterator() {}
    iterator& operator=(const iterator& it) {m_iterator = it.m_iterator; return *this;} /*!< Adaptation of the operator '='*/
    T* operator*() {return *m_iterator;} /*!< Adaptation of the operator '*'*/
    iterator operator+(int i) {return m_iterator+i;} /*!< Adaptation of the operator +*/
    iterator &operator ++() {m_iterator++; return *this;} /*!< Adaptation of the prefixe operator '++'*/
    iterator operator ++(int) {iterator cpy(m_iterator); m_iterator++; return cpy;} /*!< Adaptation of the postfixe operator '++'*/
    bool operator !=(iterator other) {return m_iterator != other.m_iterator;} /*!< Adaptation of the operator '!='*/
};

/*! \class const_iterator
 *  \brief Custom Template const_iterator. Adapted from a specific DataStructure::const_iterator.
 *
 */
template<class T,class DataStructure,class FriendClass>
class const_iterator{
    friend FriendClass; /*!< The friend class of the iterator. Generally the class containing the data structure */
    typename DataStructure::const_iterator m_iterator; /*!< The iterator to adapt */
    const_iterator(typename DataStructure::const_iterator it) : m_iterator(it) {} /*!< The adapter's constructor.*/
public:
    const_iterator() {}
    const_iterator& operator=(const const_iterator& it) {m_iterator = it.m_iterator; return *this;} /*!< Adaptation of the operator '='*/
    const T* operator*() {return *m_iterator;} /*!< Adaptation of the operator '*'*/
    const_iterator operator+(int i) {return m_iterator+i;} /*!< Adaptation of the operator +*/
    const_iterator &operator ++() {m_iterator++; return *this;} /*!< Adaptation of the prefixe operator '++'*/
    const_iterator operator ++(int) {const_iterator cpy(m_iterator); m_iterator++; return cpy;} /*!< Adaptation of the postfixe operator '++'*/
    bool operator !=(const_iterator other) {return m_iterator != other.m_iterator;} /*!< Adaptation of the operator '!='*/
};

}
#endif // ITERATOR_H
