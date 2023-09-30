//неудачный эксперимент со сменой только последней цифры в числе 123 -> 124
//                                                                 ^      ^
//работает слегка медленнее main2

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <charconv>
#include <thread>

const int LIMIT = 1e9;

// 8 * 9 + 6 * 5 + 1 * 9
const int BUFF_SIZE = 121;

using namespace std;

char* prev_num_begin;
char* prev_num_end;
bool needs_rebuild = true;
int diff = 1;
inline void write_num(char*& cur, int& num)
{
    if (needs_rebuild) {
        needs_rebuild = false;
        prev_num_begin = cur;
        cur = to_chars(cur, cur + 9, num).ptr;
        prev_num_end = cur - 1;
        *cur = '\n';
        cur++;
        num++;
    }
    else {
        int len = prev_num_end - prev_num_begin + 1;
        memcpy(cur, prev_num_begin, len + 1);   //скопировали с \n
        prev_num_end = cur + len - 1;             //передвинули конец на последнюю цифру
        (*prev_num_end) += diff;                 //увеличили последнюю цифру
        prev_num_begin = cur;                   //передвинули начало на первую цифру
        cur += len + 1;                         //текущий указатель на \n + 1
        num++;
        diff = 1;
    }
    
}

inline void write_fizz(char*& cur, int& num)
{
    memcpy(cur, "Fizz\n", 5);
    cur += 5;
    num++;
}

inline void write_buzz(char*& cur, int& num)
{
    memcpy(cur, "Buzz\n", 5);
    cur += 5;
    num++;
}

inline void write_fizzbuzz(char*& cur, int& num)
{
    memcpy(cur, "FizzBuzz\n", 9);
    cur += 9;
}



static char wrkbuf[BUFF_SIZE];
char* cur;
inline void print(int num)
{
    cur = wrkbuf;
    write_num(cur, num);      // 1
    diff = 1;
    write_num(cur, num);      // 2
    write_fizz(cur, num);     // 3
    diff = 2;
    write_num(cur, num);      // 4
    needs_rebuild = true;
    write_buzz(cur, num);     // 5
    write_fizz(cur, num);     // 6
    write_num(cur, num);      // 7
    write_num(cur, num);      // 8
    write_fizz(cur, num);     // 9
    needs_rebuild = true;
    write_buzz(cur, num);     // 10
    write_num(cur, num);      // 11
    write_fizz(cur, num);     // 12
    diff = 2;
    write_num(cur, num);      // 13
    write_num(cur, num);      // 14
    needs_rebuild = true;
    write_fizzbuzz(cur, num); // 15
    diff = 2;

    fwrite(wrkbuf, cur - wrkbuf, 1, stdout);
}

int main(void)
{
    wrkbuf[0] = '0';
    wrkbuf[1] = '\n';
    cur = wrkbuf;
    prev_num_begin = wrkbuf;
    prev_num_end = wrkbuf;
    int i;
    for (i = 1; i < LIMIT - 15; i += 15)
    {
        print(i);
    }
    while (i <= LIMIT)
    {
        if (i % 3 == 0)
        {
            printf("Fizz\n");
        }
        else if (i % 5 == 0)
        {
            printf("Buzz\n");
        }
        else
        {
            printf("%d\n", i);
        }
        i++;
    }

    return 0;
}
