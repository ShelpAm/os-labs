// Author: ShelpAm
//
// This file simulates the following problem:
// 1. (Short Answer)
// 桌上有一水果盘，最多可容纳两个水果，每次只能放入或取出一个水果。
// 爸爸专向盘中放苹果，妈妈专向盘中放桔子，儿子专等吃盘中的桔子，女儿专等吃盘中的苹果。
// 请用P、V操作实现爸爸、妈妈、儿子和女儿四个并发进程的同步与互斥算法。

#include <stdlib.h>

// The following should be defined and provided by the operating system.
struct Semaphore {};
void P(Semaphore);
void V(Semaphore);

Semaphore s_apple;
Semaphore s_orange;

size_t num_apples = 0;
size_t num_oranges = 0;

void father(size_t apples)
{
    P(s_apple);
    num_apples += apples;
    V(s_apple);
}

void daughter(size_t apples)
{
    P(s_apple);
    if (num_apples < apples) {
        // Bad here
    }
    num_apples -= apples;
    V(s_apple);
}

void mother(size_t oranges)
{
    P(s_orange);
    num_oranges += oranges;
    V(s_orange);
}

void son(size_t oranges)
{
    P(s_orange);
    if (num_oranges < oranges) {
        // Bad here
    }
    num_oranges -= oranges;
    V(s_orange);
}
