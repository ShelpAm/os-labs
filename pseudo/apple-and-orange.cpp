// Author: ShelpAm
//
// This file simulates the following problem:
// 1. (Short Answer)
// 桌上有一水果盘，最多可容纳两个水果，每次只能放入或取出一个水果。
// 爸爸专向盘中放苹果，妈妈专向盘中放桔子，儿子专等吃盘中的桔子，女儿专等吃盘中的苹果。
// 请用P、V操作实现爸爸、妈妈、儿子和女儿四个并发进程的同步与互斥算法。

#include <stdlib.h>

// The following definition about Semaphore should be defined and provided by
// the operating system.
struct Semaphore {
    Semaphore(int n);
};
void P(Semaphore);
void V(Semaphore);

Semaphore apple = 0;  // Available apples
Semaphore orange = 0; // Available oranges
Semaphore empty = 2;  // Any position to put
Semaphore pan = 1;

size_t num_apples = 0;
size_t num_oranges = 0;

void father()
{
    P(pan);
    P(empty);
    num_apples += 1;
    V(apple);
    V(pan);
}

void daughter()
{
    P(pan);
    P(apple);
    if (num_apples < 1) {
        // Bad here
    }
    num_apples -= 1;
    V(empty);
    V(pan);
}

void mother()
{
    P(pan);
    P(empty);
    num_oranges += 1;
    V(orange);
    V(pan);
}

void son()
{
    P(pan);
    P(orange);
    if (num_oranges < 1) {
        // Bad here
    }
    num_oranges -= 1;
    V(empty);
    V(pan);
}
