#include <iostream>
//********************************************************************************************************************//
//                                                1.1. Количество бит                                                 //
//Подсчитать кол-во единичных бит в входном числе , стоящих на четных позициях.                                       //
//Позиции битов нумеруются с 0.                                                                                       //
//                                                                                                                    //
//Необходимо использование битовых операций.                                                                          //
//Использование арифметических операций запрещено.                                                                    //
//                                                                                                                    //
//Формат ввода                                                                                                        //
//Входное число лежит в диапазоне 0..232-1 и вводится в десятичном виде.                                              //
//                                                                                                                    //
//Формат вывода                                                                                                       //
//Целое число - количество бит                                                                                        //
//********************************************************************************************************************//

uint8_t getEvenBitsCount(uint32_t& num) {
    uint8_t result = 0;

    while (num != 0) {
        if (num & 1) result++;
        num >>= 2;
    }
    return result;
}

int main() {
    uint32_t num = 0;
    std::cin >> num;
    std::cout << unsigned(getEvenBitsCount(num)) << std::endl;
    return 0;
}