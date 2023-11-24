// стандартный вариант с ручной сборкой строки
#define FMT_HEADER_ONLY
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <charconv>
#include <thread>

const int LIMIT = 1'000'000'000;
//const int LIMIT = 24'000'000;

// 8 * 9 + 6 * 5 + 1 * 9
const int BUFF_SIZE = 121;

using namespace std;


inline void write_num(char* &cur, int &num)
{
    cur = to_chars(cur, cur + 9, num).ptr;    
    *cur = '\n';
    cur++;
    num++;
}

inline void write_fizz(char* &cur, int &num)
{
    memcpy(cur, "Fizz\n", 5); 
    cur += 5; 
    num++; 
}

inline void write_buzz(char* &cur, int &num)
{
    memcpy(cur, "Buzz\n", 5); 
    cur += 5; 
    num++; 
}

inline void write_fizzbuzz(char* &cur, int &num)
{
    memcpy(cur, "FizzBuzz\n", 9); 
    cur += 9; 
}

inline void print(int num) {
    static char wrkbuf[BUFF_SIZE];

    char *cur = wrkbuf;
    write_num(cur, num);  //1
    write_num(cur, num);  //2
    write_fizz(cur, num); //3
    write_num(cur, num);  //4
    write_buzz(cur, num);  //5
    write_fizz(cur, num); //6
    write_num(cur, num);  //7
    write_num(cur, num);  //8
    write_fizz(cur, num); //9
    write_buzz(cur, num); //10
    write_num(cur, num); //11 
    write_fizz(cur, num);//12
    write_num(cur, num); //13
    write_num(cur, num); //14
    write_fizzbuzz(cur, num);//15
    
    
    fwrite(wrkbuf, cur - wrkbuf, 1, stdout);
}

int main(void) {
    int i;


    for (i = 1; i < LIMIT / 15 * 15; i += 15) {
        print(i);
    }
    while (i <= LIMIT) {
        if (i % 3 == 0) {
            printf("Fizz\n");
        } else if (i % 5 == 0) {
            printf("Buzz\n");
        } else {
            printf("%d\n", i);
        }
        i++;
    }

    return 0;
}
