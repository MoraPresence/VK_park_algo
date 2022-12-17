//********************************************************************************************************************//
//                                               Задача 4. «Пятнашки»                                                 //
// Написать алгоритм для решения игры в “пятнашки”. Решением задачи является приведение к виду:                       //
// [ 1  2  3  4 ]                                                                                                     //
// [ 5  6  7  8 ]                                                                                                     //
// [ 9  10 11 12]                                                                                                     //
// [ 13 14 15 0 ]                                                                                                     //
// где 0 задает пустую ячейку.                                                                                        //
// Достаточно найти хотя бы какое-то решение. Число перемещений костяшек не обязано быть минимальным.                 //                                                                                      //
//********************************************************************************************************************//

#include <array>
#include <vector>
#include <cassert>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <cmath>

static const size_t LINE_SIZE = 4;
static const size_t FIELD_SIZE = LINE_SIZE * LINE_SIZE;

using FieldArray = std::array<char, FIELD_SIZE>;

class FieldState {
public:
    FieldState() = default;
    FieldState(const FieldArray& arr, const size_t& zero_pos);
    FieldState(const FieldState &other) noexcept;
    FieldState(FieldState &&other) noexcept;

    FieldState &operator=(const FieldState &);
    FieldState &operator=(FieldState &&) noexcept;

    bool operator==(const FieldState &other) const;
    bool operator!=(const FieldState &other) const;
    bool operator<(const FieldState &pos) const;
    bool operator>(const FieldState &other) const;

    [[nodiscard]] bool IsGOAL() const;
    [[nodiscard]] std::vector<FieldState> GetNextVertices() const;

    [[nodiscard]] FieldArray getField() const { return _arr; }
    [[nodiscard]] size_t getZeroPos() const { return _zero_pos; }

private:
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();

    FieldArray _arr;
    size_t _zero_pos;
};

FieldState::FieldState(const FieldState &other) noexcept {
_zero_pos = other._zero_pos;
_arr = other._arr;
}

FieldState::FieldState(FieldState &&other) noexcept {
std::swap(_zero_pos, other._zero_pos);
std::swap(_arr, other._arr);
}

FieldState &FieldState::operator=(const FieldState &other) {
    if (&other == this) {
        return *this;
    }

    _zero_pos = other._zero_pos;
    _arr = other._arr;
    return *this;
}

FieldState &FieldState::operator=(FieldState &&other) noexcept {
std::swap(_zero_pos, other._zero_pos);
std::swap(_arr, other._arr);
return *this;
}

FieldState::FieldState(const FieldArray &arr, const size_t &zero_pos) : _arr(arr), _zero_pos(zero_pos) {}

const FieldState GOAL_STATE = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0}, FIELD_SIZE - 1};

namespace std {
    template<>
    struct hash<FieldState> {
        size_t operator()(const FieldState &other) const {
            size_t hash = 0;
            for (const auto &it: other.getField()) {
                hash = hash * 13 + it;
            }
            return hash;
        }
    };
}

bool FieldState::operator==(const FieldState &other) const { return _arr == other._arr;}
bool FieldState::operator!=(const FieldState &other) const { return !operator==(other);}
bool FieldState::operator<(const FieldState &pos) const { return _arr < pos._arr;}
bool FieldState::operator>(const FieldState &other) const { return _arr > other._arr;}
bool FieldState::IsGOAL() const {return operator==(GOAL_STATE);}

std::vector<FieldState> FieldState::GetNextVertices() const  {
    std::vector<FieldState> next;
    size_t it = 0;

    if (_zero_pos >= LINE_SIZE) {
        next.push_back(*this);
        next[it].moveUp();
        ++it;
    }
    if (_zero_pos < FIELD_SIZE - LINE_SIZE) {
        next.push_back(*this);
        next[it].moveDown();
        ++it;
    }
    if ((_zero_pos + 1) % LINE_SIZE != 0) {
        next.push_back(*this);
        next[it].moveRight();
        ++it;
    }
    if (_zero_pos % LINE_SIZE > 0) {
        next.push_back(*this);
        next[it].moveLeft();
        ++it;
    }
    return next;
}

void FieldState::moveUp() {
    size_t new_zero_pos = _zero_pos - LINE_SIZE;
    std::swap(_arr[_zero_pos], _arr[new_zero_pos]);
    _zero_pos = new_zero_pos;
}

void FieldState::moveDown() {
    size_t new_zero_pos = _zero_pos + LINE_SIZE;
    std::swap(_arr[_zero_pos], _arr[new_zero_pos]);
    _zero_pos = new_zero_pos;
}

void FieldState::moveLeft() {
    size_t new_zero_pos = _zero_pos - 1;
    std::swap(_arr[_zero_pos], _arr[new_zero_pos]);
    _zero_pos = new_zero_pos;
}

void FieldState::moveRight() {
    size_t new_zero_pos = _zero_pos + 1;
    std::swap(_arr[_zero_pos], _arr[new_zero_pos]);
    _zero_pos = new_zero_pos;
}

char GetSymbol(const FieldState &from, const FieldState &to) {
    const int32_t zero_position_diff = to.getZeroPos() - from.getZeroPos();
    switch (zero_position_diff) {
        case 4:
            return 'U';
        case -4:
            return 'D';
        case 1:
            return 'L';
        case -1:
            return 'R';
        default:
            assert(false);
    }
}

int heuristic(FieldState v) {
    int32_t heuristic = 0;
    for (int32_t i = 0; i < v.getField().size(); ++i) {
        if (!v.getField()[i]) {
            heuristic += 20 * (std::abs(3 - i % 4) + std::abs(3 - std::floor(i / 4)));
        } else {
            heuristic += 20 *
                         (std::abs(i % 4 - (v.getField()[i] - 1) % 4) +
                          std::abs(std::floor(i / 4) - std::floor((v.getField()[i] - 1) / 4)));
        }
    }
    return heuristic;
}

bool BFS(const FieldState &start, std::unordered_map<FieldState, FieldState> &parents) {
    std::priority_queue<std::pair<int32_t, FieldState>,
            std::vector<std::pair<int32_t, FieldState>>,
            std::greater<>> queue;

    queue.emplace(0, start);

    for (; !queue.empty();) {
        FieldState current = queue.top().second;
        queue.pop();

        for (const auto &pos: current.GetNextVertices()) {
            if (parents.find(pos) == parents.end()) {
                parents.emplace(pos, current);
                if (pos.IsGOAL()) {
                    return true;
                }
                queue.emplace(heuristic(pos), pos);
            }
        }
    }
    return false;
}

std::string GetPath(const FieldState &start,
                    const std::unordered_map<FieldState, FieldState> &parents) {
    std::string path;
    FieldState current = GOAL_STATE;
    for (; current != start;) {
        FieldState parent = parents.at(current);
        path.push_back(GetSymbol(parent, current));
        current = parent;
    }
    std::reverse(path.begin(), path.end());
    return path;
}

std::pair<int32_t, std::string> Solver(const FieldState &other) {
    std::unordered_map<FieldState, FieldState> parents;
    if (!BFS(other, parents)) {
        return {-1, std::string()};
    }
    auto path = GetPath(other, parents);
    return {path.size(), path};
}

int main() {
    uint32_t num;
    size_t pos = -1;
    FieldArray field;

    for (size_t i = 0; i < FIELD_SIZE; ++i) {
        std::cin >> num;
        if (num == 0) {
            pos = i;
        }
        field[i] = num;
    }
    if (pos < 0) {
        std::cout << -1;
        return 0;
    }
    FieldState start{field, pos};
    const auto result = Solver(start);
    std::cout << result.first << std::endl << result.second << std::endl;
    return 0;
}
