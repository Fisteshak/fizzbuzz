// второй вариант с многопоточностью

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <charconv>
#include <thread>
#include <vector>
#include <array>

const int LIMIT = 1'000'000'000;

// 8 * 9 + 6 * 5 + 1 * 9
const int BUFF_SIZE = 121;

const int THREAD_NUM = 16;
const int NUM_PER_THREAD = 3'000'000;
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

inline void process_chunk(char* &buf, int start_num) {
    
    
    write_num(buf, start_num);  //1
    write_num(buf, start_num);  //2
    write_fizz(buf, start_num); //3
    write_num(buf, start_num);  //4
    write_buzz(buf, start_num);  //5
    write_fizz(buf, start_num); //6
    write_num(buf, start_num);  //7
    write_num(buf, start_num);  //8
    write_fizz(buf, start_num); //9
    write_buzz(buf, start_num); //10
    write_num(buf, start_num); //11 
    write_fizz(buf, start_num);//12
    write_num(buf, start_num); //13
    write_num(buf, start_num); //14
    write_fizzbuzz(buf, start_num);//15
    
    
    
}
inline void worker(char* buf, const int start_num, const int nums_to_process, int* buflen) {
    //data.buf.assign(THREAD_BUF_SIZE, 0);
    const char* start_buf = buf;
    for (int i = start_num; i < start_num + nums_to_process; i+=15) {
        process_chunk(buf, i);
    }

    (*buflen) += (buf - start_buf);

}

void init_and_start_thread_pool(void) {
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
