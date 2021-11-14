#include "list.h"
#include <stdio.h>
#include <assert.h>

ListsMemoryAllocator lma;
bool all_lists_initiated = false;
NodesMemoryAllocator nma;
bool all_nodes_initiated = false;

void init_all_nodes() {
    for(int i=0; i<LIST_MAX_NUM_NODES-1; i++) {
        nma.node_array[i].next = &(nma.node_array[i+1]);
    }
    nma.node_array[LIST_MAX_NUM_NODES-1].next = NULL;
    nma.free_node_ptr = &(nma.node_array[0]);
}

Node* pop_node() {
    if(nma.free_node_ptr == NULL) {
        return NULL;
    }

    Node* new_node = nma.free_node_ptr;
    nma.free_node_ptr = new_node->next;
    new_node->next = NULL;
    return new_node;
}

void push_node(Node* deleted_node) {
    assert(deleted_node != NULL);

    deleted_node->prev = NULL;
    deleted_node->item = NULL;

    deleted_node->next = nma.free_node_ptr;
    nma.free_node_ptr = deleted_node;
}

void init_all_lists() {
    for(int i=0; i<LIST_MAX_NUM_HEADS-1; i++) {
        lma.list_array[i].next = &(lma.list_array[i+1]);
    }
    lma.list_array[LIST_MAX_NUM_HEADS-1].next = NULL;
    lma.free_list_ptr = &(lma.list_array[0]);
}

List* pop_list() {
    if(lma.free_list_ptr == NULL) {
        return NULL;
    }

    List* new_list = lma.free_list_ptr;
    lma.free_list_ptr = new_list->next;
    new_list->next = NULL;
    return new_list;
}

void push_list(List* deleted_list) {
    assert(deleted_list != NULL);

    deleted_list->curr = NULL;
    deleted_list->head = NULL;
    deleted_list->tail = NULL;
    deleted_list->oob = LIST_NOT_OOB;
    deleted_list->size = 0;

    deleted_list->next = lma.free_list_ptr;
    lma.free_list_ptr = deleted_list;
}

List* List_create() {
    if(!all_nodes_initiated) {
        init_all_nodes();
        all_nodes_initiated = true;
    }
    if(!all_lists_initiated) {
        init_all_lists();
        all_lists_initiated = true;
    }

    List* new_list = pop_list();
    if(new_list == NULL) {
        return NULL;
    }

    new_list->curr = NULL;
    new_list->head = NULL;
    new_list->tail = NULL;
    new_list->oob = LIST_NOT_OOB;
    new_list->size = 0;
    return new_list;
}

int List_count(List* pList) {
    assert(pList != NULL);

    int list_size = pList->size;
    assert(list_size >= 0);
    return list_size;
}

void* set_curr_node_and_get_item(List* pList, Node* tgt_node) {
    if(List_count(pList) == 0) {
        return NULL;
    }
    pList->curr = tgt_node;
    return tgt_node->item;
}

void* List_first(List* pList) {
    assert(pList != NULL);

    return set_curr_node_and_get_item(pList, pList->head);
}

void* List_last(List* pList) {
    assert(pList != NULL);

    return set_curr_node_and_get_item(pList, pList->tail);
}

void* List_next(List* pList) {
    assert(pList != NULL);

    if(pList->size == 0) {
        return NULL;
    }

    enum ListOutOfBounds list_oob = pList->oob;
    switch(list_oob) {
        case LIST_OOB_END:
            return NULL;
            break;
        case LIST_OOB_START:
            pList->curr = pList->head;
            pList->oob = LIST_NOT_OOB;
            break;
        case LIST_NOT_OOB:
            if(pList->curr->next == NULL) {
                pList->curr = NULL;
                pList->oob = LIST_OOB_END;
                return NULL;
            }

            pList->curr = pList->curr->next;
            break;
    }

    return pList->curr->item;
}

void* List_prev(List* pList) {
    assert(pList != NULL);

    if(pList->size == 0) {
        return NULL;
    }

    enum ListOutOfBounds list_oob = pList->oob;
    switch(list_oob) {
        case LIST_OOB_START:
            return NULL;
            break;
        case LIST_OOB_END:
            pList->curr = pList->tail;
            pList->oob = LIST_NOT_OOB;
            break;
        case LIST_NOT_OOB:
            if(pList->curr->prev == NULL) {
                pList->curr = NULL;
                pList->oob = LIST_OOB_START;
                return NULL;
            }

            pList->curr = pList->curr->prev;
            break;
    }

    return pList->curr->item;
}

void* List_curr(List* pList) {
    assert(pList != NULL);

    if(pList->curr == NULL) {
        return NULL;
    }
    return pList->curr->item;
}

void insert_in_empty_list(List* pList, Node* new_node) {
    assert(pList != NULL);

    pList->curr = new_node;
    pList->head = new_node;
    pList->tail = new_node;
    new_node->next = NULL;
    new_node->prev = NULL;
    pList->oob = LIST_NOT_OOB;
}

void insert_list_end(List* pList, Node* new_node) {
    new_node->next = NULL;
    new_node->prev = pList->tail;
    pList->tail->next = new_node;
    pList->curr = new_node;
    pList->tail = new_node;
    pList->oob = LIST_NOT_OOB;
}

void insert_list_start(List* pList, Node* new_node) {
    new_node->prev = NULL;
    new_node->next = pList->head;
    pList->head->prev = new_node;
    pList->curr = new_node;
    pList->head = new_node;
    pList->oob = LIST_NOT_OOB;
}

int List_insert_after(List* pList, void* pItem) {
    assert(pList != NULL);

    Node* new_node = pop_node();

    if(new_node == NULL) {
        return LIST_FAIL;
    }
    new_node->item = pItem;

    if(pList->size == 0) {
        insert_in_empty_list(pList, new_node);
    } else {
        enum ListOutOfBounds list_oob = pList->oob;
        switch(list_oob) {
            case LIST_OOB_END:
                insert_list_end(pList, new_node);
                break;
            case LIST_OOB_START:
                insert_list_start(pList, new_node);
                break;
            case LIST_NOT_OOB:
                new_node->prev = pList->curr;
                new_node->next = pList->curr->next;
                if(pList->curr != pList->tail) {
                    pList->curr->next->prev = new_node;
                } else {
                    pList->tail = new_node;
                }
                pList->curr->next = new_node;
                pList->curr = new_node;
                break;
        }
    }

    (pList->size)++;
    return LIST_SUCCESS;
}

int List_insert_before(List* pList, void* pItem) {
    assert(pList != NULL);

    Node* new_node = pop_node();

    if(new_node == NULL) {
        return LIST_FAIL;
    }
    new_node->item = pItem;

    if(pList->size == 0) {
        insert_in_empty_list(pList, new_node);
    } else {
        enum ListOutOfBounds list_oob = pList->oob;
        switch(list_oob) {
            case LIST_OOB_END:
                insert_list_end(pList, new_node);
                break;
            case LIST_OOB_START:
                insert_list_start(pList, new_node);
                break;
            case LIST_NOT_OOB:
                new_node->next = pList->curr;
                new_node->prev = pList->curr->prev;
                if(pList->curr != pList->head) {
                    pList->curr->prev->next = new_node;
                } else {
                    pList->head = new_node;
                }
                pList->curr->prev = new_node;
                pList->curr = new_node;
                break;
        }
    }

    (pList->size)++;
    return LIST_SUCCESS;
}

int List_append(List* pList, void* pItem) {
    assert(pList != NULL);

    Node* new_node = pop_node();
    if(new_node == NULL) {
        return LIST_FAIL;
    }

    new_node->item = pItem;

    if(pList->size == 0) {
        insert_in_empty_list(pList, new_node);
    } else {
        insert_list_end(pList, new_node);
    }

    (pList->size)++;
    return LIST_SUCCESS;
}

int List_prepend(List* pList, void* pItem) {
    assert(pList != NULL);

    Node* new_node = pop_node();
    if(new_node == NULL) {
        return LIST_FAIL;
    }

    new_node->item = pItem;

    if(pList->size == 0) {
        insert_in_empty_list(pList, new_node);
    } else {
        insert_list_start(pList, new_node);
    }

    (pList->size)++;
    return LIST_SUCCESS;
}

void* List_remove(List* pList) {
    assert(pList != NULL);

    if(pList->size == 0) {
        return NULL;
    }

    Node* curr_node = pList->curr;
    if(curr_node == NULL) {
        return NULL;
    }

    if(curr_node == pList->tail) {
        pList->tail = curr_node->prev;

        pList->oob = LIST_OOB_END;
    }
    if(curr_node == pList->head) {
        pList->head = curr_node->next;
    }
    if(curr_node->prev != NULL) {
        curr_node->prev->next = curr_node->next;
    }
    if(curr_node->next != NULL) {
        curr_node->next->prev = curr_node->prev;
    }
    pList->curr = curr_node->next;

    (pList->size)--;
    void* curr_item =  curr_node->item;

    push_node(curr_node);

    return curr_item;
}

void* List_trim(List* pList) {
    assert(pList != NULL);
    if(pList->size == 0) {
        return NULL;
    }

    Node* last_node = pList->tail;
    if(last_node->prev == NULL) {
        pList->head = NULL;
    } else {
        last_node->prev->next = NULL;
        
    }
    pList->tail = last_node->prev;
    pList->curr = pList->tail;
    pList->oob = LIST_NOT_OOB;

    (pList->size)--;
    void* last_item =  last_node->item;

    push_node(last_node);

    return last_item;
}

void List_concat(List* pList1, List* pList2) {
    assert(pList1 != NULL);
    assert(pList2 != NULL);
    if(pList2->size == 0) {
        push_list(pList2);
        return;
    }

    if(pList1->size == 0) {
        pList1->head = pList2->head;
        pList1->tail = pList2->tail;
    } else {
        pList2->head->prev = pList1->tail;
        pList1->tail->next = pList2->head;
        pList1->tail = pList2->tail;
    }
    pList1->size = pList1->size + pList2->size;
    push_list(pList2);
}

void List_free(List* pList, FREE_FN pItemFreeFn) {
    assert(pList != NULL);

    Node* freed_node = pList->head;
    while(freed_node) {
        if(pItemFreeFn) {
            (*pItemFreeFn)(freed_node->item);
        }
        pList->head = freed_node->next;
        (pList->size)--;
        push_node(freed_node);
        freed_node = pList->head;
    }

    push_list(pList);
}

void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg) {
    assert(pList != NULL);

    if(pComparator == NULL) {
        return NULL;
    }

    if(pList->oob == LIST_OOB_START) {
        pList->curr = pList->head;
        pList->oob = LIST_NOT_OOB;
    }
    
    if(pList->curr == NULL) {
        return NULL;
    }

    while(pList->curr && !(*pComparator)(pList->curr->item, pComparisonArg)) {
        pList->curr = pList->curr->next;
    }

    if(pList->curr) {
        pList->oob = LIST_NOT_OOB;
    } else {
        pList->oob = LIST_OOB_END;
    }
    return pList->curr;
}