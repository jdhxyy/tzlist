// Copyright 2019-2021 The jdh99 Authors. All rights reserved.
// list library for C
// Authors: jdh99 <jdh821@163.com>

#ifndef TZLIST_H
#define TZLIST_H

#include <stdint.h>
#include <stdbool.h>

#pragma pack(1)

// tNode list node
typedef struct tagNode {
    uint8_t *Data;
    int Size;

    struct tagNode *Next;
    struct tagNode *Last;
} TZListNode;

#pragma pack()

// TZListCreateList create list
// mid is tzmalloc mid
// if success return list handle,if failed return 0
intptr_t TZListCreateList(int mid);

// TZListCreateNode malloc and create node,if failed return NULL
// Notice:Must use this function create node
// Node's data field should malloc by user
TZListNode* TZListCreateNode(intptr_t list);

// TZListAppend append node
void TZListAppend(intptr_t list, TZListNode* node);

// TZListPrepend prepend node
void TZListPrepend(intptr_t list, TZListNode* node);

// TZListInsertBefore insert node before anchor node
void TZListInsertBefore(intptr_t list, TZListNode* node, TZListNode* anchorNode);

// TZListInsertAfter insert node after anchor node
void TZListInsertAfter(intptr_t list, TZListNode* node, TZListNode* anchorNode);

// TZListRemove remove node
// This operation will free data field space and node's space
void TZListRemove(intptr_t list, TZListNode* node);

// TZListBreakLink break node
// Notice:This node will be dangling,so should free data field space and node's space by user
void TZListBreakLink(intptr_t list, TZListNode* node);

// TZListDrop drop list
void TZListDrop(intptr_t list);

// TZListClear remove all node
void TZListClear(intptr_t list);

// TZListIsEmpty is list empty
bool TZListIsEmpty(intptr_t list);

// TZListGetNodeNum get node num
int TZListGetNodeNum(intptr_t list);

// TZListGetHeader get header node
TZListNode* TZListGetHeader(intptr_t list);

// TZListGetTail get tail node
TZListNode* TZListGetTail(intptr_t list);

#endif


