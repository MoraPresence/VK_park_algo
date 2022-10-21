#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>

//********************************************************************************************************************//
//                                               7_2. LSD для long long.                                              //
//Дан массив неотрицательных целых 64-битных чисел. Количество чисел не больше 106.                                   //
//Отсортировать массив методом поразрядной сортировки LSD по байтам.                                                  //
//********************************************************************************************************************//
#define RANGE 256

void fillArr(std::vector<int64_t>& arr, uint64_t& size) {
    std::stringstream ss;
    std::string input;
    std::getline(std::cin, input);
    ss << input;

    for (int i = 0; i < size; ++i) {
        uint64_t num = 0;
        ss >> num;
        arr.emplace_back(num);
    }
    ss.clear();
}

uint32_t getByte(int64_t number, size_t n) {
    return number >> (8 * n) & 255;
}

void countingSort(std::vector<int64_t> &arr, size_t size, const size_t n) {
    int64_t count[RANGE];
    memset(count, 0, sizeof(count));

    for(size_t i = 0; i < size; i++)
        count[getByte(arr[i], n)]++;

    for(size_t i = 1; i < RANGE; i++)
        count[i] += count[i - 1];

    std::vector<int64_t> output;
    output.reserve(size);

    for(int64_t i = size - 1; i >= 0 ; i--)
        output[--count[getByte(arr[i], n)]] = arr[i];

    for(size_t i = 0; i < size; i++)
        arr[i] = output[i];

    output.clear();
}

void LSDSort(std::vector<int64_t>& arr, uint64_t& size){
    for (int i = 0; i < sizeof(int64_t); ++i)
        countingSort(arr, size, i);
}

void printArr(std::vector<int64_t>& arr, uint64_t& size){
    for (int i = 0; i < size; ++i)
        std::cout << arr[i] << " ";
}

int main() {
    std::stringstream ss;
    std::string input;

    std::getline(std::cin, input);
    ss << input;

    uint64_t arrSize = 0;
    ss >> arrSize;
    ss.clear();

    std::vector<int64_t> arr;
    arr.reserve(arrSize);

    fillArr(arr, arrSize);

    LSDSort(arr, arrSize);

    printArr(arr, arrSize);

    arr.clear();

    return 0;
}
