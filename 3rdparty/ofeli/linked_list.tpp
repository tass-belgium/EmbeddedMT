/****************************************************************************
**
** Copyright (C) 2010-2012 Fabien Bessy.
** Contact: fabien.bessy@gmail.com
**
** This file is part of project Ofeli.
**
** http://www.cecill.info/licences/Licence_CeCILL_V2-en.html
** You may use this file under the terms of the CeCILL license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Fabien Bessy and its Subsidiary(-ies) nor the
**     names of its contributors may be used to endorse or promote products
**     derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/

namespace ofeli
{

template <typename T>
typename list<T>::Link list<T>::mem_head = NULL;

///////////////////// list initialization/destruction /////////////////////

template <typename T>
void list<T>::alloc_mem_pool()
{
    // build a part of the shared linked list memory pool
    Link new_mem_head;
    for( int i = 0; i < mem_pool_size; i++ )
    {
        new_mem_head = new Node( T(), mem_head );
        mem_head = new_mem_head; // update of the head link of the memory pool linked list
    }

    // * creates sentinel/dummy node after the last node and not before the first node
    // ==> 'push_front' a value never invalidates an iterator,
    // even if this iterator is on the first node.
    // ==> 'erase' is possible for every element of a list (instead of 'erase_after'),
    // even for the last node.

    // push_front property is required for the fast level set algorithm

    //head = new Node( T(), NULL ); // *
    head = Node::new_( T(), NULL ); // with memory_pool

    return;
}

template <typename T>
list<T>::list(int mem_pool_size1) : mem_pool_size(mem_pool_size1) // *
{
    alloc_mem_pool();
}

template <typename T>
list<T>::list(int mem_pool_size1, int n, const T& value) : mem_pool_size(mem_pool_size1) // *
{
    alloc_mem_pool();
    assign(n,value);
}

template <typename T>
list<T>::list(int mem_pool_size1, const T array[], int array_length) : mem_pool_size(mem_pool_size1) // *
{
    alloc_mem_pool();
    assign(array,array_length);
}

template <typename T>
list<T>::list(int mem_pool_size1, const list& copied) : mem_pool_size(mem_pool_size1) // *
{
    alloc_mem_pool();
    push_front(copied);
}

template <typename T>
list<T>& list<T>::operator=(const list& rhs)
{
    // 2 implementations

    // assignment with copy-and-swap idiom
    /*
    list<T> temp(rhs);
    list<T>::swap(*this,temp);
    return *this;
    */

    // optimized assignment which minimizes the number of new and delete
    if( this != &rhs ) // no auto-affectation
    {
        iterator       it_lhs = this->begin();
        const_iterator it_rhs = rhs.begin();

        // while a node exists in each list
        while( !it_lhs.end() && !it_rhs.end() )
        {
            // copies data without creating a new node
            *it_lhs = *it_rhs;

            ++it_lhs;
            ++it_rhs;
        }

        if( it_rhs.end() )
        {
            if( !it_lhs.end() ) // it means this->size() > rhs.size()
            {
                // function 'erase_after' is faster than 'erase'
                // specially for large object of type T
                // because it needn't copying data field of the next node
                while( !it_lhs.node->next->end() )
                {
                    // ==> it deletes nodes
                    it_lhs = erase_after(it_lhs);
                }
                erase(it_lhs); // deletes the last node too
            }
        }
        else
        {
            if( it_lhs.end() )
            {
                // it means this->size() < rhs.size()
                // ==> it creates new nodes with the value *it_rhs

                // it_lhs is at the end of list *this,
                // it_lhs is the sentinel node position
                it_lhs = insert_before(it_lhs,*it_rhs);
                ++it_rhs;

                for(  ; !it_rhs.end(); ++it_rhs )
                {
                    // now, it_lhs is the position of the last "true" node
                    // before the sentinel node
                    it_lhs = insert_after(it_lhs,*it_rhs);
                }
            }
        }
    }

    return *this;
}

template <typename T>
list<T>::~list()
{
    // while the list is not empty (including the sentinel node)
    while( head != NULL )
    {
        pop_front();
    }

    // delete a part of the shared linked list memory pool
    Link new_head;
    for( int i = 0; i < mem_pool_size; i++ )
    {
        new_head = mem_head->next;
        delete mem_head; // delete the first node
        mem_head = new_head;
    }
}

template <typename T>
void list<T>::clear()
{
    // while the list is not empty (excluding the sentinel node)
    while( !empty() )
    {
        pop_front();
    }

    return;
}


///////////////////////// list moving and reading /////////////////////////

template <typename T>
typename list<T>::iterator list<T>::begin()
{
    return head; // implicit type conversion
    // iterator encapsulates a Link which is a pointer to a Node
}

template <typename T>
typename list<T>::const_iterator list<T>::begin() const // const overloading
{
    return head; // implicit type conversion
    // iterator encapsulates a Link which is a pointer to a Node
}


////////////////////////////// list changes ///////////////////////////////

template <typename T>
inline void list<T>::assign(const T array[], int array_length)
{
    clear();

    iterator it_this = begin();
    int index = 0;

    if( index < array_length )
    {
        // list *this is empty so
        // it_this is at the end of list *this,
        // it_this is the sentinel node position
        it_this = insert_before(it_this,array[index]);
        index++;
    }

    for(  ; index < array_length; index++ )
    {
        // now, it_this is the position of the last "true" node
        // before the sentinel node
        it_this = insert_after(it_this,array[index]);
    }

    return;
}

template <typename T>
inline void list<T>::assign(int n, const T& value)
{
    clear();

    for( int iteration = 0; iteration < n; iteration++ )
    {
        push_front(value);
    }

    return;
}

template <typename T>
inline void list<T>::pop_front()
{
    Link new_head = head->next;

    //delete head; // delete the first node
    Node::delete_(head); // with memory_pool

    head = new_head;
    return;
}

template <typename T>
inline void list<T>::push_front(const T& value)
{
    // it never invalidates an iterator of *this
    // to link the new head node to the former head node

    //Link new_head = new Node(value,head);
    Link new_head = Node::new_(value,head); // with memory_pool

    head = new_head; // update of the head link

    return;
}

template <typename T>
inline void list<T>::push_front(const list& copied)
{
    iterator         it_this = this->begin();
    const_iterator it_copied = copied.begin();

    if( !it_copied.end() )
    {
        // insert the first node
        it_this = insert_before(it_this,*it_copied);
        ++it_copied;
    }

    for(  ; !it_copied.end(); ++it_copied )
    {
        // insert the other nodes
        it_this = insert_after(it_this,*it_copied);
    }

    return;
}

template <typename T>
template <typename BinaryPredicate>
inline void list<T>::put_away(const T& value, BinaryPredicate compare)
{
    iterator position = begin();

    while( !position.end() )
    {
        if( compare(value,*position) )
        {
            break;
        }

        ++position;
    }

    insert_before(position,value);

    return;
}

template <typename T>
inline void list<T>::put_away(const T& value)
{
    put_away( value, less<T>() );
    return;
}

template <typename T>
inline typename list<T>::iterator list<T>::insert_before(iterator position, const T& value)
{
    //Link new_node = new Node(*position.node);
    Link new_node = Node::new_(*position.node); // with memory_pool

    position.node->data = value;
    position.node->next = new_node;
    return position;
}

template <typename T>
inline typename list<T>::iterator list<T>::insert_after(iterator position, const T& value)
{
    //Link new_node = new Node(value,position.node->next);
    Link new_node = Node::new_(value,position.node->next); // with memory_pool

    position.node->next = new_node;
    return new_node; // implicit type conversion
    // iterator encapsulates a Link which is a pointer to a Node
}

template <typename T>
inline typename list<T>::iterator list<T>::erase(iterator position)
{    
    Link next_node = position.node->next;

    // copy the next node to the node
    *position.node = *next_node;
    //  <==> position.node->data = next_node->data; position.node->next = next_node->next;

    //delete next_node; // erase the next node
    Node::delete_(next_node); // with memory_pool

    return position;
}

template <typename T>
inline typename list<T>::iterator list<T>::erase_after(iterator position)
{
    Link next_node = position.node->next;

    position.node->next = next_node->next;

    //delete next_node; // erase the next node
    Node::delete_(next_node); // with memory_pool

    return position;
}

template <typename T>
inline typename list<T>::iterator list<T>::splice_front(iterator moved)
{   
    // save the node of iterator 'moved'
    T value = *moved;
    Link next_node = moved.node->next;

    // copy the next node to the node
    *moved.node = *next_node;
    //  <==> moved.node->data = next_node->data; moved.node->next = next_node->next;

    next_node->data = value; // the next node sets the value that we want to move
    next_node->next = head; // move the next node to the beginnning of list *this
    head = next_node; // update of the head link

    return moved;
}

template <typename T>
inline void list<T>::splice_front(list& moved)
{
    Link saved_head = moved.head;
    Link position = saved_head;

    // if list 'moved' is not empty
    if( !position->end() )
    {
        while( !position->next->end() )
        {
            position = position->next;
        }
        // here, Link 'position' points to the last element of list 'moved', just before the sentinel node
        moved.head = position->next; // 'moved' must be empty so it head is the sentinel node
        position->next = this->head; // the last element of 'moved' points to the firt element of *this
        this->head = saved_head; // head of list *this is the former head of list 'moved'
    }

    return;
}

template <typename T>
void list<T>::remove(const T& value)
{
    // function 'erase_after' is faster than 'erase'
    // specially for large object of type T
    // because it needn't copying data field of the next node

    iterator position = begin();

    if( !position.end() )
    {
        while( !position.node->next->end() )
        {
            // checks and erases possibly from the second node to the last node
            if( position.node->next->data == value )
            {
                position = erase_after(position);
            }
            else
            {
                ++position;
            }
        }

        position = begin();
        // checks and erases possibly the first node
        if( *position == value )
        {
            erase(position);
        }
    }

    return;
}

template <typename T>
template <typename UnaryPredicate>
void list<T>::remove_if(UnaryPredicate predicate)
{
    // function 'erase_after' is faster than 'erase'
    // specially for large object of type T
    // because, it needn't copying data field of the next node

    iterator position = begin();

    if( !position.end() )
    {
        while( !position.node->next->end() )
        {
            // checks and erases possibly the second node to the last node
            if( predicate(position.node->next->data) )
            {
                position = erase_after(position);
            }
            else
            {
                ++position;
            }
        }

        position = begin();
        // checks and erases possibly the first node
        if( predicate(*position) )
        {
            erase(position);
        }
    }

    return;
}

template <typename T>
template <typename BinaryPredicate>
void list<T>::unique(BinaryPredicate compare)
{
    iterator position = begin();

    if( !position.end() )
    {
        while( !position.node->next->end() )
        {
            if( compare(position.node->next->data,*position) )
            {
                position = erase_after(position);
            }
            else
            {
                ++position;
            }
        }
    }

    return;
}

template <typename T>
void list<T>::unique()
{
    unique( equal_to<T>() );

    return;
}

template <typename T>
inline void list<T>::swap(list& list1, list& list2)
{
    if( &list1 != &list2 ) // no auto-swap
    {
        // swap heads
        Link temp = list1.head;
        list1.head = list2.head;
        list2.head = temp;
    }

    return;
}

template <typename T>
void list<T>::reverse()
{
    Link position = head;

    if( !position->end() )
    {
        Link moved;

        // while a node of list *this after 'position' exists
        while( !position->next->end() )
        {
            moved = position->next; // save the node which is moved

            position->next = moved->next; // skip node 'moved'

            moved->next = head; // move node 'moved' to the beginnning of list *this
            head = moved; // update of the head link
        }
    }

    return;
}

// Merge sort algorithm :
// - Bottom-up merge sort which is a non-recursive variant
// - Temporal complexity is O(n log n) in average and in the worst case.
// - Spatial complexity is O(1), it is an in-place algorithm.
// - It produces a stable sort, i.e. it preserves the input order of equal elements.
template <typename T>
template <typename BinaryPredicate>
void list<T>::sort(BinaryPredicate compare)
{
    // if list *this is empty,
    // there is nothing to sort, we stop
    if( this->empty() )
    {
        return;
    }

    // head and tail of the merged list
    Link merged_head = this->head;
    Link merged_tail;

    Link p1, p2; // positions in sorted sublist 1 and 2
    unsigned int size1, size2; // lengths of sorted sublist 1 and 2

    Link node_to_append; // node to append to merged list

    unsigned int nbr_merges; // count number of merges

    // exit is possible at the end of the loop 'for' below with a condition on variable 'nbr_merges'
    // in fact, this loop is passed 'log n' times, where 'n' is the length of list *this
    for( unsigned int sublist_size = 1; 1; sublist_size *= 2 )
    {
        p1 = merged_head;
        merged_head = NULL;
        merged_tail = NULL;

        nbr_merges = 0;

        while( !p1->end() )
        {
            // There exists a merge to be done.
            // Step 'sublist_size' places along from p1.
            p2 = p1;
            size1 = 0;
            for( unsigned int iteration = 0; iteration < sublist_size; iteration++ )
            {
                size1++;
                p2 = p2->next;
                if( p2->end() )
                {
                    break;
                }
            }

            // If p2 hasn't fallen off end, we have two lists to merge.
            size2 = sublist_size;

            // Now we have two lists; merge them.
            while( size1 > 0 || ( size2 > 0 && !p2->end() ) )
            {
                // Decide whether next node of merge comes from p1 or p2.

                if( size1 == 0 )
                {
                    // p1 is empty; node_to_append must come from p2.
                    node_to_append = p2;
                    p2 = p2->next;
                    size2--;
                }
                else if( size2 == 0 || p2->end() )
                {
                    // p2 is empty; node_to_append must come from p1.
                    node_to_append = p1;
                    p1 = p1->next;
                    size1--;
                }
                else if( compare(p1->data,p2->data) )
                {
                    // First node of p1 is lower; node_to_append must come from p1.
                    node_to_append = p1;
                    p1 = p1->next;
                    size1--;
                }
                else
                {
                    // First node of p2 is lower; node_to_append must come from p2.
                    node_to_append = p2;
                    p2 = p2->next;
                    size2--;
                }

                // Add the next node to the merged list.
                if( merged_tail == NULL )
                {
                    merged_head = node_to_append;
                }
                else
                {
                    merged_tail->next = node_to_append;
                }

                merged_tail = node_to_append; // merged_tail update
            }

            // Now p1 has stepped 'sublist_size' places along, and p2 has too.
            p1 = p2;
            nbr_merges++;
        }

        // p1 is at the position of the sentinel node and is appended to the merged list
        merged_tail->next = p1;

        // If we have done only one merge, we're finished.
        // Allow for nbr_merges == 0, the empty merged list case.
        if( nbr_merges <= 1 )
        {
            // list *this is the final merged list
            this->head = merged_head;
            return;
        }
    }
}

// Sorts the elements into ascending order by default.
template <typename T>
inline void list<T>::sort()
{
    sort( less<T>() );
    return;
}

//////////////////////////// list information /////////////////////////////

template <typename T>
inline bool list<T>::empty() const
{
    if( head->end() ) // if the head node is the sentinel
    {
        return true;
    }
    else
    {
        return false;
    }
}

template <typename T>
int list<T>::size() const
{
    int size = 0;

    for( const_iterator position = begin(); !position.end(); ++position )
    {
        size++;
    }

    return size;
}

template <typename U>
bool operator==(const list<U>& lhs, const list<U>& rhs)
{
    typename list<U>::const_iterator it_lhs = lhs.begin();
    typename list<U>::const_iterator it_rhs = rhs.begin();

    while( !it_lhs.end() && !it_rhs.end() )
    {
        if( *it_lhs != *it_rhs )
        {
            return false;
        }

        ++it_lhs;
        ++it_rhs;
    }

    return it_lhs.end() && it_rhs.end();
}

template <typename U>
bool operator!=(const list<U>& lhs, const list<U>& rhs)
{
    return !( lhs == rhs );
}

template <typename U>
std::ostream& operator<<(std::ostream& os, const list<U>& displayed)
{
    os << std::endl << " -------------" << std::endl;
    os << " index | value" << std::endl;
    os << " -------------" << std::endl;

    int index = 0;
    for( typename list<U>::const_iterator position = displayed.begin(); !position.end(); ++position )
    {
        os.width(6);
        os << std::right << index++ << " | " << *position << std::endl;
    }

    os << " -------------" << std::endl;

    return os;
}

template <typename T>
void list<T>::display() const
{
    std::cout << *this;
}

template <typename T>
T* list<T>::get_array(int array_length) const
{
    T* array = new T[array_length];

    int index = 0;
    for( const_iterator position = begin(); !position.end(); ++position )
    {
        array[index++] = *position;
    }

    return array;
}

}
