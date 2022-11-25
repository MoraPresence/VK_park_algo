#include <iostream>
#include <sstream>
#include <vector>

//********************************************************************************************************************//
//                                           5.3. Закраска прямой                                                     //
//На числовой прямой окрасили N отрезков.                                                                             //
//Известны координаты левого и правого концов каждого отрезка [Li, Ri]. Найти длину окрашенной части числовой прямой. //
//N ≤ 10000. Li, Ri — целые числа в диапазоне [0, 10^9].                                                               //
//                                                                                                                    //
//Формат ввода                                                                                                        //
//В первой строке записано количество отрезков.                                                                       //
//В каждой последующей строке через пробел записаны координаты левого и правого концов отрезка.                       //
//                                                                                                                    //
//Формат вывода                                                                                                       //
//Выведите целое число — длину окрашенной части.                                                                      //
//********************************************************************************************************************//
#define START true
#define END false

class Point {
    uint32_t num;
    bool pointType;
public:
    Point() = default;

    Point(const uint32_t _num, const bool _pointType) : num(_num), pointType(_pointType) {};

    Point(const Point &ref_Point);

    static bool compare(const Point &a, const Point &b);

    [[nodiscard]] uint32_t getNum() const { return num; }

    [[nodiscard]] bool isStart() const { return pointType; }

    Point &operator=(const Point &p);
};

bool Point::compare(const Point &a, const Point &b) {
    return a.num < b.num;
}

Point &Point::operator=(const Point &p) {
    this->num = p.num;
    this->pointType = p.pointType;
    return *this;
}

Point::Point(const Point &ref_Point) {
    this->num = ref_Point.num;
    this->pointType = ref_Point.pointType;
}

void fillPointArr(const uint32_t &operationCount, std::vector<Point> &arr) {
    std::stringstream ss;
    std::string input;
    for (int i = 0; i < operationCount; ++i) {
        std::getline(std::cin, input);
        ss << input;
        uint32_t startSegment = 0;
        uint32_t endSegment = 0;

        ss >> startSegment >> endSegment;

        arr.emplace_back(startSegment, START);
        arr.emplace_back(endSegment, END);
        ss.clear();
    }
}

template<typename T>
void mergeSort(T arr[], std::size_t size, bool (*cmp)(const T &l, const T &r)) {
    if (size > 1) {
        size_t leftSize = size / 2;
        size_t rightSize = size - leftSize;

        mergeSort(&arr[0], leftSize, cmp);
        mergeSort(&arr[leftSize], rightSize, cmp);

        size_t leftIndex = 0;
        size_t rightIndex = leftSize;
        size_t index = 0;

        auto tmpArr = new T[size];

        for (; leftIndex < leftSize || rightIndex < size;) {
            if (cmp(arr[leftIndex], arr[rightIndex])) {
                tmpArr[index] = std::move(arr[leftIndex]);
                ++index;
                ++leftIndex;
            } else {
                tmpArr[index] = std::move(arr[rightIndex]);
                ++index;
                ++rightIndex;
            }

            if (leftIndex == leftSize) {
                std::copy(std::make_move_iterator(&arr[rightIndex]), std::make_move_iterator(&arr[size]),
                          &tmpArr[index]);
                break;
            }

            if (rightIndex == size) {
                std::copy(std::make_move_iterator(&arr[leftIndex]), std::make_move_iterator(&arr[leftSize]),
                          &tmpArr[index]);
                break;
            }
        }
        std::copy(std::make_move_iterator(tmpArr), std::make_move_iterator(&tmpArr[size]),
                  arr);
    }
}

uint32_t getSizeLine(std::vector<Point> &arr) {
    uint32_t depth = 0;
    uint32_t result = 0;

    for (int i = 0; i < arr.size() - 1; ++i) {
        if (arr[i].isStart())depth++;
        else depth--;

        if (depth > 0) result += arr[i + 1].getNum() - arr[i].getNum();
    }
    return result;
}


int main() {
    std::stringstream ss;
    std::string input;

    std::getline(std::cin, input);
    ss << input;

    uint32_t operationCount = 0;
    ss >> operationCount;
    ss.clear();

    std::vector<Point> arr;
    arr.reserve(2 * operationCount);

    fillPointArr(operationCount, arr);

    mergeSort(&arr[0], arr.size(), Point::compare);

    std::cout << getSizeLine(arr);

    return 0;
}
