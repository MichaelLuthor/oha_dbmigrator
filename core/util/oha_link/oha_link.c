#include <stdlib.h>

#include "oha_link.h"
#include "../oha_data/oha_data.h"

oha_link * oha_link_create() {
    oha_link * head = (oha_link *)malloc(sizeof(oha_link));
    head->length = 0;
    head->first_node = NULL;
    head->last_node = NULL;
    head->current_node = NULL;
    return head;
}

boolean oha_link_append( oha_link * head, void * data ) {
    oha_link_node * data_node = (oha_link_node *)malloc(sizeof(oha_link_node));
    data_node->data = data;
    data_node->next = NULL;

    if ( 0 == head->length ) {
        head->current_node = data_node;
        head->first_node = data_node;
    } else {
        head->last_node->next = data_node;
        data_node->prev = head->last_node;
    }
    head->last_node = data_node;
    head->length ++;
    return OHA_TRUE;
}

boolean oha_link_reset(oha_link * head) {
    head->current_node = head->first_node;
    return OHA_TRUE;
}

boolean oha_link_prev(oha_link * head) {
    if ( 0 == head->length ) {
        return OHA_FALSE;
    }
    if ( head->current_node == head->first_node ) {
        return OHA_FALSE;
    }
    head->current_node = head->current_node->prev;
    return OHA_TRUE;
}

boolean oha_link_next(oha_link * head) {
    if ( 0 == head->length ) {
        return OHA_FALSE;
    }
    if ( head->current_node == head->last_node ) {
        return OHA_FALSE;
    }
    head->current_node = head->current_node->next;
    return OHA_TRUE;
}

oha_link_node * oha_link_current(oha_link * head) {
    return head->current_node;
}

uint64 oha_link_get_length(oha_link * head) {
    return head->length;
}

boolean oha_link_current_remove(oha_link * head) {
    if ( 0 == head->length ) {
        return OHA_FALSE;
    }

    oha_link_node * node = head->current_node;
    if ( NULL == node->prev ) {
        head->first_node = node->next;
    } else {
        node->prev->next = node->next;
    }

    if ( NULL == node->next ) {
        head->last_node = node->prev;
    } else {
        node->next->prev = node->prev;
    }
    head->length --;
    return OHA_TRUE;
}

boolean oha_link_desctory(oha_link * head) {
    oha_link_node * node = head->first_node;
    oha_link_node * next_node = NULL;
    while ( NULL != node ) {
        next_node = node->next;
        free(node);
        node = next_node;
    }

    free(head);
    return OHA_TRUE;
}
