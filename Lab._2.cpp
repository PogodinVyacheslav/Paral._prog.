#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>

using namespace std;

mutex mtx;
condition_variable cv;
atomic<bool> exit_signal(false);
int num_animal = 0; // Номер зверя

// Функ. 1 потока
void lion(int id) {
    unique_lock<mutex> lock(mtx);
    cout << id << " лев ждёт выхода\n";
    cv.wait(lock, [] {return num_animal != 1;}); // Выход льва
    cout << id << " лев выход. на арену\n";
    num_animal = 1; // Устан. льва
    cv.notify_all(); // Увед. потоков
}

// Функ. 2 потока
void tiger(int id) {
    unique_lock<mutex> lock(mtx);
    cout << id << " тигр ждёт выхода\n";
    cv.wait(lock, [] {return num_animal != 2;}); // Выход тигра
    cout << id << " тигр выход. на арену\n";
    num_animal = 2; // Устан. тигра
    cv.notify_all(); // Увед. потоков
}

int main() {
    vector<thread> threads;
    
    int n = 10; // Кол-во зверей
    cout << "\n";

    // Запуск львов
    for (int i=0; i<n; ++i) {
        threads.emplace_back(lion, i+1);
    }
    // Запуск тигров
    for (int i=0; i<n-1; ++i) {
        threads.emplace_back(tiger, i+1);
    }
    // Вывод резул.
    thread exit_t([]() {
        unique_lock<mutex> exit_lock(mtx); // Измен. сигн.
        cv.wait(exit_lock, [] {return exit_signal.load();});
    });

    this_thread::sleep_for(chrono::seconds(2));
    exit_signal.store(true); // Устан. сигн.
    cv.notify_all(); // Увед. потоков

    for (auto& t:threads) {
        if (t.joinable()) {
            t.join();
        }
    } // Завер. раб. потоков
    if (exit_t.joinable()) {
        exit_t.join();
    }
    cout << "\n";
    return 0;
}