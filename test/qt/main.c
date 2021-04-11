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
