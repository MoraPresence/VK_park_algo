#include <iostream>
#include <sstream>
#include <cstring>

//********************************************************************************************************************//
//                                         3.1. Очередь с динамическим буфером                                        //
//Реализовать очередь с динамическим зацикленным буфером.                                                             //
//Обрабатывать команды push back и pop front.                                                                         //
//Формат ввода                                                                                                        //
//В первой строке количество команд n. n ≤ 1000000.                                                                   //
//Каждая команда задаётся как 2 целых числа: a b.                                                                     //
//        a = 2 - pop front                                                                                           //
//        a = 3 - push back                                                                                           //
//Если дана команда pop front, то число b - ожидаемое значение. Если команда pop front вызвана для пустой             //
// структуры данных, то ожидается “-1”.                                                                               //
//Формат вывода                                                                                                       //
//Требуется напечатать YES - если все ожидаемые значения совпали. Иначе, если хотя бы одно ожидание не оправдалось,   //
// то напечатать NO.                                                                                                  //
//********************************************************************************************************************//

#define DEFAULT_SIZE 2
#define PUSH_BACK 3
#define POP_FRONT 2

template<typename T>
class MyQueue {
    struct QueueNode {
        T value;
    } typedef QueueNode;

    unsigned capacity;
    int size, front, back;
    QueueNode *cells;

    void Resize();

public:
    MyQueue();

    explicit MyQueue(int);

    ~MyQueue();

    bool Push(T value);

    T Pop();

    bool isEmpty();
};


template<typename T>
MyQueue<T>::MyQueue() {
    this->capacity = DEFAULT_SIZE;
    this->size = this->front = 0;
    this->back = 0;
    this->cells = new QueueNode[DEFAULT_SIZE];
    memset(this->cells, 0, this->capacity * sizeof(QueueNode));
}

template<typename T>
MyQueue<T>::MyQueue(const int size) {
    this->capacity = size;
    this->size = this->front = 0;
    this->back = 0;
    this->cells = new QueueNode[size];
    memset(this->cells, 0, this->capacity * sizeof(QueueNode));
}

template<typename T>
MyQueue<T>::~MyQueue() {
    delete[] cells;
}

template<typename T>
void MyQueue<T>::Resize() {
    auto newCapacity = this->capacity * 2;
    auto toDeleteCells = this->cells;

    this->cells = new QueueNode[newCapacity];
    memset(this->cells, 0, newCapacity * sizeof(QueueNode));

    for (size_t i = 0; this->front != this->back; ++i) {
        this->cells[i] = toDeleteCells[this->front];
        this->front = (this->front + 1) % this->capacity;
    }

    this->back = this->size;
    this->front = 0;
    this->capacity = newCapacity;
    delete[] toDeleteCells;
}


template<typename T>
bool MyQueue<T>::Push(T value) {
    if ((this->back + 1) % this->capacity == this->front) Resize();

    this->cells[this->back] = QueueNode{value};
    this->back = (this->back + 1) % this->capacity;
    this->size++;

    return true;
}

template<typename T>
T MyQueue<T>::Pop() {
    if (isEmpty()) return -1;

    auto item = this->cells[this->front];
    this->front = (this->front + 1) % this->capacity;
    this->size--;

    return ((MyQueue<T>::QueueNode) item).value;
}

template<typename T>
bool MyQueue<T>::isEmpty() {
    return size == 0;
}

bool testingQueue(MyQueue<int> &queue, const unsigned countCommands, std::stringstream &ss) {
    for (size_t i = 0; i < countCommands; ++i) {
        int commandQueue = 0;
        int value = 0;
        std::string input;

        std::getline(std::cin, input);
        ss << input;
        ss >> commandQueue;
        ss >> value;

        int item;
        switch (commandQueue) {
            case PUSH_BACK:
                queue.Push(value);
                break;
            case POP_FRONT:
                if (queue.isEmpty() && value != -1) {
                    return false;
                }
                item = queue.Pop();
                if (item != value) {
                    return false;
                }
                break;
            default:
                return false;
        }
        ss.clear();
    }
    return true;
}


int main() {
    std::stringstream ss;
    std::string input;

    std::getline(std::cin, input);
    ss << input;

    unsigned countCommands = 0;
    ss >> countCommands;
    ss.clear();

    MyQueue<int> queue;

    if (testingQueue(queue, countCommands, ss))
        std::cout << "YES" << std::endl;
    else
        std::cout << "NO" << std::endl;

    return 0;
}
