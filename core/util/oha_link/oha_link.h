#ifndef __OHA_LINK_H__
#define __OHA_LINK_H__

#include "../oha_data/oha_data.h"

typedef struct _oha_link_node {
    void * data;
    struct _oha_link_node * prev;
    struct _oha_link_node * next;
} oha_link_node;

typedef struct _oha_link {
    uint64 length;
    oha_link_node * current_node;
    oha_link_node * first_node;
    oha_link_node * last_node;
} oha_link;

oha_link *      oha_link_create         ();
boolean         oha_link_append         (oha_link * head, void * data );
boolean         oha_link_reset          (oha_link * head);
boolean         oha_link_prev           (oha_link * head);
boolean         oha_link_next           (oha_link * head);
oha_link_node * oha_link_current        (oha_link * head);
uint64          oha_link_get_length     (oha_link * head);
boolean         oha_link_current_remove (oha_link * head);
boolean         oha_link_desctory       (oha_link * head);

#endif
