// слегка улучшенное лобовое

#include <iostream>

using namespace std;


int main()
{

    int n = 1'000'000;

    for (int i = 1; i <= n; i+=15) {
        printf( "%d\n"          // 1
                "%d\n"          // 2
                "Fizz\n"        // 3
                "%d\n"          // 4
                "Buzz\n"        // 5
                "Fizz\n"        // 6
                "%d\n"          // 7
                "%d\n"          // 8
                "Fizz\n"        // 9
                "Buzz\n"        // 10
                "%d\n"          // 11
                "Fizz\n"        // 12
                "%d\n"          // 13
                "%d\n"          // 14
                "FizzBuzz\n",   // 15
                i, i+1, i+3, i+6, i+7, i+10, i+12, i+13);

    }
}