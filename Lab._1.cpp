#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace chrono;

// Функция для генерации случайного числа в диапазоне [min, max]
int randomInRange(int min, int max, mt19937& gen) {
    uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

// Функция для вывода первых и последних элементов строк матрицы
void printMatrixEdges(const vector<vector<string>>& matrix, int numElements = 2) {
    int A = matrix.size();
    int B = matrix[0].size();

    auto printRowEdges = [&](const vector<string>& row) {
        for (int i = 0; i < numElements; ++i) {
            cout << row[i] << " ";
        }
        cout << "... ";
        for (int i = B - numElements; i < B; ++i) {
            cout << row[i] << " ";
        }
        cout << endl;
    };

    int numRows = 3; // Количество выводимых строк с начала и конца матрицы

    cout << "Первые " << numRows << " строки матрицы:\n";
    for (int i = 0; i < numRows; ++i) {
        printRowEdges(matrix[i]);
    }
    cout << "\n"; // Для обозначения пропущенных строк
    cout << "Последние " << numRows << " строки матрицы:\n";
    for (int i = A - numRows; i < A; ++i) {
        printRowEdges(matrix[i]);
    }
    cout << endl;
}

// Функция для перестановки строк в многопоточном режиме
void swapRows(vector<vector<string>>& matrix, int start, int end, int step) {
    for (int i = start; i < end; i += step) {
        swap(matrix[i], matrix[matrix.size() - 1 - i]);
    }
}

int main() {
    // Генерация случайных нечетных размеров матрицы
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1000, 1500);
    int A = dis(gen) * 2 + 1;
    int B = dis(gen) * 2 + 1;

    // Создание матрицы размером AxB, заполненной случайными числами от 1 до 1000
    vector<vector<string>> matrix(A, vector<string>(B));
    for (int i = 0; i < A; ++i) {
        for (int j = 0; j < B; ++j) {
            matrix[i][j] = to_string(randomInRange(1, 1000, gen));
        }
    }

    // Вывод размеров матрицы
    cout << "Размер матрицы: " << A << " x " << B << endl;

    cout << "Матрица до перестановки:\n";
    printMatrixEdges(matrix);

    // Перестановка строк с использованием 1, 2 и 4 потоков
    for (int numThreads : {1, 2, 4}) {
        vector<thread> threads;
        auto start = high_resolution_clock::now();

        int step = 2; // Переставляем нечетные строки

        for (int t = 0; t < numThreads; ++t) {
            int startIdx = t * (A / (2 * numThreads));
            int endIdx = (t == numThreads - 1) ? A / 2 : (t + 1) * (A / (2 * numThreads));
            threads.push_back(thread(swapRows, ref(matrix), startIdx, endIdx, step));
        }

        for (auto& th : threads) {
            th.join();
        }

        auto end = high_resolution_clock::now();
        duration<double> elapsed = end - start;

        // Измерение времени только для перестановки
        cout << "Матрица после перестановки с " << numThreads << " потоками:\n";
        printMatrixEdges(matrix);
        cout << fixed << setprecision(6) << scientific;
        cout << "Время выполнения с " << numThreads << " потоками: " << elapsed.count() << " с." << endl;
        cout << "\n";
    }
    return 0;
}