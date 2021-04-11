# tzlist

## 介绍
tzlist是标准双向链表TZList，支持节点数据是任意数据类型。

tzlist依赖内存管理包tzmalloc,使用cip可以安装依赖的包:[cip：C/C++包管理工具](https://blog.csdn.net/jdh99/article/details/115590099)


## 开源
- [github上的项目地址](https://github.com/jdhxyy/tzlist)
- [gitee上的项目地址](https://gitee.com/jdhxyy/tzlist)

## API
```c
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
```

## 常用链表操作
链表的所有操作都进行了封装，使用起来更安全，不容易内存泄漏。

### 创建链表
```c
intptr_t list = TZListCreateList(gMid);
```
创建链表会返回一个句柄，创建失败则返回值是NULL。

### 创建节点
#### 节点数据结构
```c
// tNode 链表节点
typedef struct tagNode
{
    uint8_t *Data;
    int Size;

    struct tagNode *Next;
    struct tagNode *Last;
} TZListNode;
```

字段含义：

字段|含义
-|-
Data|可存储任意格式的数据
Size|存储数据的字节数
Next|下一个节点地址
Last|上一个节点地址

如果下一个节点不存在，则Next为NULL。此时节点为尾节点。
如果上一个节点不存在，则Last为NULL。此时节点为首节点。

#### 创建操作
```c
TZListNode* node = TZListCreateNode(list);
```
如果创建失败则返回值是NULL。

一般来说，会在节点中存储数据，假如存储的是一个结构体，则完整操作如下：
```c
struct Test1
{
     int a;
     int b;
};

TZListNode* node = TZListCreateNode(list);
if (node == NULL) {
    return;
}
node->Data = TZMalloc(gMid, sizeof(struct Test1));
if (node->Data == NULL) {
    TZFree(node);
    return;
}
node->Size = sizeof(struct Test1);
struct Test1* test1 = (struct Test1*)node->Data;
test1->a = i;
test1->b = i;
TZListPrepend(list, node);
```
TZListPrepend操作将节点压入链表。

### 删除节点
删除节点的操作是：
```c
// TZListRemove 删除节点
// 删除节点会释放节点内的开辟的数据空间以及释放节点本身的空间
void TZListRemove(intptr_t list, TZListNode* node);
```

比如删除首节点的操作：
```c
TZListNode* node = TZListGetHeader(list);
if (node == NULL) {
    break;
}
struct Test1* test1 = (struct Test1*)node->Data;
printf("a = %d b = %d\n", test1->a, test1->b);
TZListRemove(list, node);
```

### 遍历链表
链表是双向的，可以从首节点开始遍历，也可以从尾节点开始遍历。
首节点遍历并打印数据：
```c
printf("从链表首部开始遍历:\n");
TZListNode* node = TZListGetHeader(list);
for (;;) {
    if (node == NULL) {
        break;
    }

    for (int j = 0; j < node->Size; j++) {
        printf("%d\t", node->Data[j]);
    }
    printf("\n");
    node = node->Next;
}
```

## 测试
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tzlist.h"
#include "tzmalloc.h"

#define RAM_INTERVAL 0

static int gMid = -1;

static void printMallocInfo(void);
static void testCase0(void);
static void testCase1(void);
static void testCase2(void);
static void testCase3(void);
static void testCase4(void);

int main() {
    TZMallocLoad(RAM_INTERVAL, 10, 100 * 1000, malloc(100 * 1000));
    gMid = TZMallocRegister(RAM_INTERVAL, "listTest", 100 * 1000);

    testCase0();
    testCase1();
    testCase2();
    testCase3();
    testCase4();
    return 0;
}

static void printMallocInfo(void) {
    TZMallocUser* user;
    int num = TZMallocGetUserNum(RAM_INTERVAL);
    for (int i = 0; i < num; i++) {
        user = TZMallocGetUser(i);
        printf("mid:%d tag:%s total:%d used:%d mallocNum:%d freeNum:%d\n", i,
               user->Tag, user->Total, user->Used, user->MallocNum, user->FreeNum);
    }
}

static void testCase0(void) {
    printf("-------------------->case0:尾部插入数组测试开始\n");

    uint8_t buffer[10] = {0};
    intptr_t list = TZListCreateList(gMid);

    printf("尾部插入数据:\n");
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 10; j++) {
            buffer[j] = (uint8_t)i;
            printf("%d\t", buffer[j]);
        }
        printf("\n");
        TZListNode* node = TZListCreateNode(list);
        if (node == NULL) {
            continue;
        }
        node->Data = TZMalloc(gMid, 10);
        if (node->Data == NULL) {
            TZFree(node);
            continue;
        }
        memcpy(node->Data, buffer, 10);
        node->Size = 10;
        TZListAppend(list, node);
    }

    printf("遍历读取第一个节点并删除:\n");
    for (;;) {
        TZListNode* node = TZListGetHeader(list);
        if (node == NULL) {
            break;
        }
        for (int i = 0; i < node->Size; i++) {
            printf("%d\t", node->Data[i]);
        }
        TZListRemove(list, node);
        node = NULL;
        printf("\n");
    }
    TZListDrop(list);
    printMallocInfo();

    printf("-------------------->case0:测试结束\n");
}

static void testCase1(void) {
    printf("-------------------->case1:首部插入结构体测试开始\n");

#pragma pack(1)
    struct Test1
    {
        int a;
        int b;
    };
#pragma pack()

    intptr_t list = TZListCreateList(gMid);

    printf("首部插入数据:\n");
    for (int i = 0; i < 5; i++) {
        TZListNode* node = TZListCreateNode(list);
        if (node == NULL) {
            continue;
        }
        node->Data = TZMalloc(gMid, sizeof(struct Test1));
        if (node->Data == NULL) {
            TZFree(node);
            continue;
        }
        node->Size = sizeof(struct Test1);
        struct Test1* test1 = (struct Test1*)node->Data;
        test1->a = i;
        test1->b = i;
        printf("a = %d b = %d\n", test1->a, test1->b);

        TZListPrepend(list, node);
    }

    printf("遍历读取第一个节点并删除:\n");
    for (;;) {
        TZListNode* node = TZListGetHeader(list);
        if (node == NULL) {
            break;
        }
        struct Test1* test1 = (struct Test1*)node->Data;
        printf("a = %d b = %d\n", test1->a, test1->b);
        TZListRemove(list, node);
    }
    TZListDrop(list);
    printMallocInfo();

    printf("-------------------->case1:测试结束\n");
}

static void testCase2(void) {
    printf("-------------------->case2:遍历队列测试开始\n");

    intptr_t list = TZListCreateList(gMid);

    printf("插入数据:\n");
    for (int i = 0; i < 5; i++) {
        TZListNode* node = TZListCreateNode(list);
        if (node == NULL) {
            break;
        }
        node->Data = TZMalloc(gMid, 10);
        if (node->Data == NULL) {
            TZFree(node);
            break;
        }
        node->Size = 10;
        for (int j = 0; j < 10; j++) {
            node->Data[j] = (uint8_t)i;
            printf("%d\t", node->Data[j]);
        }
        printf("\n");
        TZListAppend(list, node);
    }

    printf("从链表首部开始遍历:\n");
    TZListNode* node = TZListGetHeader(list);
    for (;;) {
        if (node == NULL) {
            break;
        }

        for (int j = 0; j < node->Size; j++) {
            printf("%d\t", node->Data[j]);
        }
        printf("\n");

        node = node->Next;
    }

    printf("从链表尾部开始遍历:\n");
    node = TZListGetTail(list);
    for (;;) {
        if (node == NULL) {
            break;
        }

        for (int j = 0; j < node->Size; j++) {
            printf("%d\t", node->Data[j]);
        }
        printf("\n");

        node = node->Last;
    }
    TZListDrop(list);
    printMallocInfo();

    printf("-------------------->case2:测试结束\n");
}

static void testCase3(void)
{
    printf("-------------------->case3:1000000次写入然后清除列表测试开始\n");

    intptr_t list = TZListCreateList(gMid);
    int num = 1000000;

    while (num--) {
        for (int i = 0; i < 5; i++) {
            TZListNode* node = TZListCreateNode(list);
            if (node == NULL) {
                break;
            }
            node->Data = TZMalloc(gMid, 10);
            if (node->Data == NULL) {
                TZFree(node);
                break;
            }
            node->Size = 10;
            for (int j = 0; j < node->Size; j++) {
                node->Data[j] = (uint8_t)i;
            }
            TZListAppend(list, node);
        }

        TZListClear(list);
        if (TZListIsEmpty(list) == false) {
            printf("测试失败.检测到列表非空!\n");
            break;
        }
    }
    TZListDrop(list);
    printMallocInfo();
    printf("-------------------->case3:测试结束\n");
}

static void testCase4(void) {
    printf("-------------------->case4:1000000次创建然后删除列表测试开始\n");

    uint32_t num = 1000000;

    while (num--) {
        intptr_t list = TZListCreateList(gMid);
        for (int i = 0; i < 5; i++) {
            TZListNode* node = TZListCreateNode(list);
            if (node == NULL) {
                break;
            }
            node->Data = TZMalloc(gMid, 10);
            if (node->Data == NULL) {
                TZFree(node);
                break;
            }
            node->Size = 10;
            for (int j = 0; j < node->Size; j++) {
                node->Data[j] = (uint8_t)i;
            }
            TZListAppend(list, node);
        }

        TZListDrop(list);
        list = 0;
    }
    printMallocInfo();
    printf("-------------------->case4:测试结束\n");
}
```

输出:
```text
-------------------->case0:尾部插入数组测试开始
尾部插入数据:
0       0       0       0       0       0       0       0       0       0
1       1       1       1       1       1       1       1       1       1
2       2       2       2       2       2       2       2       2       2
3       3       3       3       3       3       3       3       3       3
4       4       4       4       4       4       4       4       4       4
遍历读取第一个节点并删除:
0       0       0       0       0       0       0       0       0       0
1       1       1       1       1       1       1       1       1       1
2       2       2       2       2       2       2       2       2       2
3       3       3       3       3       3       3       3       3       3
4       4       4       4       4       4       4       4       4       4
mid:0 tag:listTest total:100000 used:0 mallocNum:11 freeNum:11
-------------------->case0:测试结束
-------------------->case1:首部插入结构体测试开始
首部插入数据:
a = 0 b = 0
a = 1 b = 1
a = 2 b = 2
a = 3 b = 3
a = 4 b = 4
遍历读取第一个节点并删除:
a = 4 b = 4
a = 3 b = 3
a = 2 b = 2
a = 1 b = 1
a = 0 b = 0
mid:0 tag:listTest total:100000 used:0 mallocNum:22 freeNum:22
-------------------->case1:测试结束
-------------------->case2:遍历队列测试开始
插入数据:
0       0       0       0       0       0       0       0       0       0
1       1       1       1       1       1       1       1       1       1
2       2       2       2       2       2       2       2       2       2
3       3       3       3       3       3       3       3       3       3
4       4       4       4       4       4       4       4       4       4
从链表首部开始遍历:
0       0       0       0       0       0       0       0       0       0
1       1       1       1       1       1       1       1       1       1
2       2       2       2       2       2       2       2       2       2
3       3       3       3       3       3       3       3       3       3
4       4       4       4       4       4       4       4       4       4
从链表尾部开始遍历:
4       4       4       4       4       4       4       4       4       4
3       3       3       3       3       3       3       3       3       3
2       2       2       2       2       2       2       2       2       2
1       1       1       1       1       1       1       1       1       1
0       0       0       0       0       0       0       0       0       0
mid:0 tag:listTest total:100000 used:0 mallocNum:33 freeNum:33
-------------------->case2:测试结束
-------------------->case3:1000000次写入然后清除列表测试开始
mid:0 tag:listTest total:100000 used:0 mallocNum:10000034 freeNum:10000034
-------------------->case3:测试结束
-------------------->case4:1000000次创建然后删除列表测试开始
mid:0 tag:listTest total:100000 used:0 mallocNum:21000034 freeNum:21000034
-------------------->case4:测试结束
```
