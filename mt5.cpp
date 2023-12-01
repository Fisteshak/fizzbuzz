// второй вариант с многопоточностью

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <charconv>
#include <thread>
#include <array>
#include "immintrin.h"

const int LIMIT = 1'000'000'000;

// 8 * 9 + 6 * 5 + 1 * 9
const int BUFF_SIZE = 121;

const int THREAD_NUM = 16;
const int NUM_PER_THREAD = 90000;
const int THREAD_BUF_SIZE = (BUFF_SIZE * NUM_PER_THREAD / 15);
const int NUM_PER_THREAD_MUL15 = NUM_PER_THREAD / 15 * 15;

int active_threads_num = 0;
int worker_cycles_done = 0;
int first_non_processed_num = 1;

using namespace std;


struct thread_data {
    std::thread t;
    std::array <char, THREAD_BUF_SIZE> buf;
    int buflen;

};

std::array <thread_data, THREAD_NUM> thread_pool;

// don't use itoa() because it is non-standard and more generic
__always_inline static char *myitoa(int number, char *cur) {
    do {
        *--cur = number % 10 + '0';
        number /= 10;
    } while (number != 0);
    return cur;
}

__always_inline static void myitoa_diff(int number, char *cur, const int diff_len) {
    char *end = cur - diff_len;
    do {
        *--cur = number % 10 + '0';
        number /= 10;
    } while (cur > end);
}

__always_inline static char *myitoa14(int number, char *cur, char* const old) {
    *--cur = number % 10 + '0';
    number /= 10;
    *--cur = number % 10 + '0';
    for (;;) {
        number /= 10;
        if (number == 0) {
            return cur;
        }
        char digit = number % 10 + '0';
        if (*--cur == digit) {
            return old;
        }
        *cur = digit;
    }
}


__always_inline void process_chunk(char* &buf, int &num, char* wrkbuf, char* pos[8], int &buf_len, int &digit_num) {

    memcpy(wrkbuf + BUFF_SIZE - 10, "\nFizzBuzz\n", 10);   // 15

    // always output num + 13 to check number of digits
    char *cur = myitoa14(num + 13, wrkbuf + BUFF_SIZE - 10, pos[7]);    // 14

    if (digit_num != wrkbuf + BUFF_SIZE - 10 - cur) {
        // there are more digits in the number - create buffer from the scratch
        pos[7] = cur;
        *--cur = '\n';                      // 13
        cur = myitoa(num + 12, cur) - 6;
        memcpy(cur, "\nFizz\n", 6);         // 12
        pos[6] = cur;                       // 11
        cur = myitoa(num + 10, cur) - 11;
        memcpy(cur, "\nFizz\nBuzz\n", 11);  // 9, 10
        pos[5] = cur;                       // 8
        cur = myitoa(num + 7, cur) - 1;
        *cur = '\n';                        // 7
        pos[4] = cur;
        cur = myitoa(num + 6, cur) - 11;
        memcpy(cur, "\nBuzz\nFizz\n", 11);  // 5, 6
        pos[3] = cur;                       // 4
        cur = myitoa(num + 3, cur) - 6;
        memcpy(cur, "\nFizz\n", 6);         // 3
        pos[2] = cur;                       // 2
        cur = myitoa(num + 1, cur) - 1;
        *cur = '\n';                        // 1
        pos[1] = cur;
        cur = myitoa(num + 0, cur);
        pos[0] = cur;
        digit_num = pos[1] - cur;
        buf_len = wrkbuf + BUFF_SIZE - cur;
    } else {
        // find out how many digits actually changed - use SSE2 instruction for comparing 8-byte buffers
        unsigned int diff = ~_mm_movemask_epi8(_mm_cmpeq_epi8(
                                               _mm_loadl_epi64((__m128i const *)pos[0]),
                                               _mm_loadl_epi64((__m128i const *)cur)));
        // lower zero bits indicate unchanged bytes
        unsigned int diff_len = digit_num - _tzcnt_u32(diff);   // number of changed digits

        myitoa_diff(num + 12, cur - 1, diff_len);   // 13
        myitoa_diff(num + 10, pos[6] , diff_len);   // 11
        myitoa_diff(num +  7, pos[5] , diff_len);   // 8
        myitoa_diff(num +  6, pos[4] , diff_len);   // 7
        myitoa_diff(num +  3, pos[3] , diff_len);   // 4
        myitoa_diff(num +  1, pos[2] , diff_len);   // 2
        myitoa_diff(num +  0, pos[1] , diff_len);   // 1
    }
    memcpy(buf, pos[0], buf_len);
    buf += buf_len;

}
__always_inline void worker(char* buf, const int start_num, const int nums_to_process, int* buflen) {
    char wrkbuf[BUFF_SIZE] {};
    char *pos[8] {};     // past the end positions of numbers within the buffer
    int buf_len = 0;     // output length
    int digit_num = 0;   // number of digits within the number

    const char* start_buf = buf;
    for (int i = start_num; i < start_num + nums_to_process; i+=15) {
        process_chunk(buf, i, wrkbuf, pos, buf_len, digit_num);
    }

    (*buflen) += (buf - start_buf);

}

inline void init_and_start_thread_pool(void) {
    for (int i = 0; i < THREAD_NUM and first_non_processed_num + NUM_PER_THREAD < LIMIT; i++) {
        //thread_pool[i].buf.assign(THREAD_BUF_SIZE, 0);
        thread_pool[i].buflen = 0;

        active_threads_num++;
        worker_cycles_done++;

        thread_pool[i].t = std::thread(worker, thread_pool[i].buf.data(), first_non_processed_num, NUM_PER_THREAD, &(thread_pool[i].buflen));
        first_non_processed_num += NUM_PER_THREAD;
    }
}

int main(void) {
    int i;

    init_and_start_thread_pool();
    //auto f = fopen("mt3.txt", "w");
    for (int i = 0; active_threads_num != 0; i = (i + 1) % THREAD_NUM) {

        thread_pool[i].t.join();

        fwrite(thread_pool[i].buf.data(), thread_pool[i].buflen, 1, stdout);

        if (first_non_processed_num + NUM_PER_THREAD < LIMIT) {
            thread_pool[i].buflen = 0;
            thread_pool[i].t = std::thread(worker, thread_pool[i].buf.data(), first_non_processed_num, NUM_PER_THREAD, &(thread_pool[i].buflen));
            first_non_processed_num += NUM_PER_THREAD;
        } else if (first_non_processed_num < LIMIT / 15 * 15) {
            thread_pool[i].buflen = 0;
            thread_pool[i].t = std::thread(worker, thread_pool[i].buf.data(), first_non_processed_num, (LIMIT - first_non_processed_num) / 15 * 15, &(thread_pool[i].buflen));
            first_non_processed_num += (LIMIT - first_non_processed_num) / 15 * 15;
        } else {
            active_threads_num--;
        }
    }

    i = first_non_processed_num;
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
