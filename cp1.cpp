#include <vector>
#include <thread>
#include <condition_variable>
#include <queue>
#include <functional>
#include <algorithm>
#include <string>
#include <iostream>
#include <array>
#include <cstdio>
#include <cstring>
#include <charconv>

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
class ThreadPool {
public:
    ThreadPool(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            threads.emplace_back([this] {
                while (true) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->mutex);
                        this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty()) return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task();
                }
            });
        }
    }

    template<class F>
    void enqueue(F&& f) {
        {
            std::unique_lock<std::mutex> lock(mutex);
            tasks.emplace(std::forward<F>(f));
        }
        condition.notify_one();
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &thread : threads) {
            thread.join();
        }
    }

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex mutex;
    std::condition_variable condition;
    bool stop = false;
};

struct thread_data {
    std::thread t;
    std::array <char, THREAD_BUF_SIZE> buf;
    int buflen;

};

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

std::array <thread_data, THREAD_NUM> thread_pool;


int main(void) {
    ThreadPool pool(THREAD_NUM);

    for (int i = 0; i < LIMIT; i += NUM_PER_THREAD) {
        pool.enqueue([i]() {
            thread_data data;
            data.buflen = 0;
            worker(data.buf.data(), i, NUM_PER_THREAD, &(data.buflen));
            fwrite(data.buf.data(), data.buflen, 1, stdout);
        });
    }

    int i = first_non_processed_num;
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