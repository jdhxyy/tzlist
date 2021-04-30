// Copyright 2019-2021 The jdh99 Authors. All rights reserved.
// list library for C
// Authors: jdh99 <jdh821@163.com>

#include "tzlist.h"
#include "tzmalloc.h"

#include <string.h>

#pragma pack(1)

// tRoot root node
typedef struct {
    TZListNode* header;
    TZListNode* tail;
    int mid;
} tRoot;

#pragma pack()

// TZListCreateList create list
// mid is tzmalloc mid
// if success return list handle,if failed return 0
intptr_t TZListCreateList(int mid) {
    tRoot* root = TZMalloc(mid, sizeof(tRoot));
    if (root == NULL) {
        return 0;
    }
    memset(root, 0, sizeof(tRoot));
    root->mid = mid;
    return (intptr_t)root;
}

// TZListCreateNode malloc and create node,if failed return NULL
// Notice:Must use this function create node
// Node's data field should malloc by user
TZListNode* TZListCreateNode(intptr_t list) {
    if (list == 0) {
        return NULL;
    }
    tRoot* root = (tRoot*)list;
    TZListNode* node = TZMalloc(root->mid, sizeof (TZListNode));
    return node;
}

// TZListAppend append node
void TZListAppend(intptr_t list, TZListNode* node) {
    if (list == 0 || node == NULL) {
        return;
    }

    tRoot* root = (tRoot*)list;
    if (root->tail == NULL) {
        root->header = node;
        root->tail = node;
        root->header->Last = NULL;
        root->tail->Next = NULL;
        return;
    }
    TZListInsertAfter(list, node, root->tail);
}

// TZListPrepend prepend node
void TZListPrepend(intptr_t list, TZListNode* node) {
    if (list == 0 || node == NULL) {
        return;
    }

    tRoot* root = (tRoot*)list;
    if (root->header == NULL) {
        root->header = node;
        root->tail = node;
        root->header->Last = NULL;
        root->tail->Next = NULL;
        return;
    }
    TZListInsertBefore(list, node, root->header);
}

// TZListInsertBefore insert node before anchor node
void TZListInsertBefore(intptr_t list, TZListNode* node, TZListNode* anchorNode) {
    if (list == 0 || node == NULL || anchorNode == NULL) {
        return;
    }

    tRoot* root = (tRoot*)list;
    node->Last = anchorNode->Last;
    node->Next = anchorNode;

    // 根据锚节点是否首节点处理锚节点下一个节点
    if (anchorNode->Last != NULL) {
        anchorNode->Last->Next = node;
    } else {
        root->header = node;
    }
    anchorNode->Last = node;
}

// TZListInsertAfter insert node after anchor node
void TZListInsertAfter(intptr_t list, TZListNode* node, TZListNode* anchorNode) {
    if (list == 0 || node == NULL || anchorNode == NULL) {
        return;
    }

    tRoot* root = (tRoot*)list;
    node->Last = anchorNode;
    node->Next = anchorNode->Next;

    // 根据锚节点是否尾节点处理锚节点下一个节点
    if (anchorNode->Next != NULL) {
        anchorNode->Next->Last = node;
    } else {
        root->tail = node;
    }
    anchorNode->Next = node;
}

// TZListRemove 删除节点
// 删除节点会释放节点内的开辟的数据空间以及释放节点本身的空间
void TZListRemove(intptr_t list, TZListNode* node) {
    if (list == 0 || node == NULL) {
        return;
    }

    TZListBreakLink(list, node);
    TZFree(node->Data);
    TZFree(node);
}

// TZListBreakLink break node
// Notice:This node will be dangling,so should free data field space and node's space by user
void TZListBreakLink(intptr_t list, TZListNode* node) {
    if (list == 0 || node == NULL) {
        return;
    }

    tRoot* root = (tRoot*)list;
    // deal dangling node
    if (node->Last == NULL && node->Next == NULL && root->header != node && root->tail != node) {
        return;
    }
    do {
        // deal if the node is last
        if (root->header == node && root->tail == node) {
            root->header = NULL;
            root->tail = NULL;
            break;
        }
        // deal header node
        if (root->header == node) {
            root->header = node->Next;
            if (root->header != NULL) {
                root->header->Last = NULL;
            }
            break;
        }
        // deal tail node
        if (root->tail == node) {
            root->tail = node->Last;
            if (root->tail != NULL) {
                root->tail->Next = NULL;
            }
            break;
        }
        // deal normal node
        node->Last->Next = node->Next;
        node->Next->Last = node->Last;
    } while (0);

    node->Last = NULL;
    node->Next = NULL;
}

// TZListDrop drop list
void TZListDrop(intptr_t list) {
    if (list == 0) {
        return;
    }

    tRoot* root = (tRoot*)list;
    TZListClear(list);
    TZFree(root);
}

// TZListClear remove all node
void TZListClear(intptr_t list) {
    if (list == 0) {
        return;
    }

    tRoot* root = (tRoot*)list;
    for (;;) {
        if (root->header == NULL) {
            break;
        }
        TZListRemove(list, root->header);
    }
}

// TZListIsEmpty is list empty
bool TZListIsEmpty(intptr_t list) {
    if (list == 0) {
        return 0;
    }

    tRoot* root = (tRoot*)list;
    return root->header == NULL;
}

// TZListGetNodeNum get node num
int TZListGetNodeNum(intptr_t list) {
    if (list == 0) {
        return 0;
    }

    tRoot* root = (tRoot*)list;
    TZListNode* node = root->header;
    int num = 0;
    for (;;) {
        if (node == NULL) {
            break;
        }
        num++;
        node = node->Next;
    }
    return num;
}

// TZListGetHeader get header node
TZListNode* TZListGetHeader(intptr_t list) {
    if (list == 0) {
        return NULL;
    }

    tRoot* root = (tRoot*)list;
    return root->header;
}

// TZListGetTail get tail node
TZListNode* TZListGetTail(intptr_t list) {
    if (list == 0) {
        return NULL;
    }

    tRoot* root = (tRoot*)list;
    return root->tail;
}
