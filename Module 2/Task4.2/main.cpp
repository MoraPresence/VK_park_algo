//********************************************************************************************************************//
//                                                4.2. Порядковые статистики.                                         //
// Дано число N и N строк. Каждая строка содержит команду добавления или удаления натуральных чисел,                  //
// а также запрос на получение k-ой порядковой статистики.                                                            //
// Команда добавления числа A задается положительным числом A,                                                        //
// команда удаления числа A задается отрицательным числом “-A”.                                                       //
// Запрос на получение k-ой порядковой статистики задается числом k                                                   //
//********************************************************************************************************************//

#include <iostream>
#include <unordered_set>

template<
        typename Key,
        class Compare = std::less<Key>
>
class AVLTree {
public:
    class btNode {
    public:
        btNode() = default;
        btNode(Key data, int32_t height, size_t count,
               btNode *l_child, btNode *r_child);
        explicit btNode(const Key &data)
                : _data(data), _height(1),
                  _l_child(nullptr), _r_child(nullptr) {}
        btNode(const btNode &);
        btNode(btNode &&) noexcept;
        btNode &operator=(const btNode &);
        btNode &operator=(btNode &&) noexcept;
        ~btNode();

    public:
        Key _data{};
        int32_t _height = 1;
        size_t _count = 1;
        btNode *_l_child = nullptr;
        btNode *_r_child = nullptr;
    };

public:
    using value_type = Key;
    using const_ref_type = const value_type &;
    using node_value_type = btNode;
    using const_node_value_type = const btNode;
    using node_pointer = btNode *;
    using const_node_pointer = const node_pointer;

public:
    AVLTree() : _root(nullptr), _header(new node_value_type) {}
    AVLTree(const AVLTree &);
    AVLTree(AVLTree &&) noexcept;
    explicit AVLTree(const AVLTree *);
    AVLTree &operator=(const AVLTree &);
    AVLTree &operator=(AVLTree &&) noexcept;
    ~AVLTree();

    void insert(const_ref_type data);
    void erase(const_ref_type data);
    size_t height(node_pointer node) const;
    size_t count(node_pointer node) const;
    void fixHeight(node_pointer node);
    void fixCount(node_pointer node);
    int32_t balance(node_pointer node);
    void clear(node_pointer);
    bool ge(const Key &l, const Key &r) const;
    bool gt(const Key &l, const Key &r) const;
    bool le(const Key &l, const Key &r) const;
    bool lt(const Key &l, const Key &r) const;
    bool eq(const Key &l, const Key &r) const;
    auto get_root(){return _root;}
    [[nodiscard]] const_ref_type order_statistics(size_t k) const;

    Compare cmp = Compare();

private:
    node_pointer insert_(node_pointer node, const_ref_type data);
    node_pointer erase_(node_pointer node, const_ref_type data);
    node_pointer doBalance_(node_pointer node);
    node_pointer _turn_right_(node_pointer current);
    node_pointer _turn_left_(node_pointer current);
    node_pointer _get_leftest_node_(node_pointer node) const;
    node_pointer _get_rightest_node_(node_pointer node) const;

    node_pointer _header;
protected:
    node_pointer _root;
};

template<typename Key, class Compare>
bool AVLTree<Key, Compare>
::ge(const Key &l, const Key &r) const {
    return !cmp(l, r);
}

template<typename Key, class Compare>
bool AVLTree<Key, Compare>
::gt(const Key &l, const Key &r) const {
    return cmp(r, l);
}

template<typename Key, class Compare>
bool AVLTree<Key, Compare>
::le(const Key &l, const Key &r) const {
    return !gt(l, r);
}

template<typename Key, class Compare>
bool AVLTree<Key, Compare>
::lt(const Key &l, const Key &r) const {
    return cmp(l, r);
}

template<typename Key, class Compare>
bool AVLTree<Key, Compare>
::eq(const Key &l, const Key &r) const {
    return !(cmp(l, r) || cmp(r, l));
}

template<typename Key, class Compare>
bool operator<(typename AVLTree<Key, Compare>
::btNode const &c1,
               typename AVLTree<Key, Compare>
               ::btNode const &c2) {
    return cmp(c1._data, c2._data);
}

template<typename Key, class Compare>
bool operator==(typename AVLTree<Key, Compare>
::btNode const &c1,
                typename AVLTree<Key, Compare>
                ::btNode const &c2) {
    return !((c1 < c2) || (c2 < c1));
}

template<typename Key, class Compare>
bool operator!=(typename AVLTree<Key, Compare>
::btNode const &c1,
                typename AVLTree<Key, Compare>
                ::btNode const &c2) {
    return !(operator==(c1, c2));
}

template<typename Key, class Compare>
bool operator>(typename AVLTree<Key, Compare>
::btNode const &c1,
               typename AVLTree<Key, Compare>
               ::btNode const &c2) {
    return operator<(c2, c1);
}

template<typename Key, class Compare>
bool operator<=(typename AVLTree<Key, Compare>
::btNode const &c1,
                typename AVLTree<Key, Compare>
                ::btNode const &c2) {
    return !(operator>(c1, c2));
}

template<typename Key, class Compare>
bool operator>=(
        typename AVLTree<Key, Compare>
        ::btNode const &c1,
        typename AVLTree<Key, Compare>
        ::btNode const &c2) {
    return !(operator<(c1, c2));
}

template<typename Key, class Compare>
AVLTree<Key, Compare>::AVLTree(const AVLTree &other) : AVLTree() {
    for (auto it : other) {
        insert(it);
    }
}

template<typename Key, class Compare>
AVLTree<Key, Compare>::AVLTree(AVLTree &&other) noexcept
: AVLTree(other) {
    other._size = 0;
    other = nullptr;
}

template<typename Key, class Compare>
AVLTree<Key, Compare>::AVLTree(const AVLTree *other)
        : AVLTree(*other) {
}

template<typename Key, class Compare>
AVLTree<Key, Compare> &AVLTree<Key, Compare>
::operator=(const AVLTree &other) {
    if (&other == this) {
        return *this;
    }

    clear(_root);

    for (auto &it : other) {
        insert(it);
    }
    return *this;
}

template<typename Key, class Compare>
AVLTree<Key, Compare> &AVLTree<Key, Compare>
::operator=(AVLTree &&other) noexcept {
*this = other;

other._root = nullptr;
other._size = 0;
}

template<typename Key, class Compare>
AVLTree<Key, Compare>::~AVLTree() {
    clear(_root);
    delete _header;
}

template<typename Key, class Compare>
void AVLTree<Key, Compare>::insert(const_ref_type data) {
    _root = insert_(_root, data);
}

template<typename Key, class Compare>
typename AVLTree<Key, Compare>::node_pointer AVLTree<Key, Compare>
::insert_(AVLTree::node_pointer node, const_ref_type data) {
    if (!node) {
        return new btNode(data);
    } else if (lt(data, node->_data)) {
        node->_l_child = insert_(node->_l_child, data);
    } else {
        node->_r_child = insert_(node->_r_child, data);
    }
    return doBalance_(node);
}

template<typename Key, class Compare>
void AVLTree<Key, Compare>::erase(const_ref_type data) {
    _root = erase_(_root, data);
}

template<typename Key, class Compare>
typename AVLTree<Key, Compare>::node_pointer AVLTree<Key, Compare>
::erase_(AVLTree::node_pointer node, const_ref_type data) {
    if (!node) {
        return nullptr;
    } else if (lt(data, node->_data)) {
        node->_l_child = erase_(node->_l_child, data);
    } else if (gt(data, node->_data)){
        node->_r_child = erase_(node->_r_child, data);
    }else{
        if (!node->_l_child || !node->_r_child) {
            node_pointer tmp = node->_l_child ? node->_l_child : node->_r_child;
            if (!tmp) {
                tmp = node;
                node = nullptr;
            } else {
                *node = *tmp;
            }
            delete tmp;
        } else {
            node_pointer tmp = _get_leftest_node_(node->_r_child);
            node->_data = tmp->_data;
            node->_r_child = erase_(node->_r_child, tmp->_data);
        }
    }

    if(!node){
        return nullptr;
    }

    return doBalance_(node);
}

template<typename Key, class Compare>
typename AVLTree<Key, Compare>::node_pointer AVLTree<Key, Compare>
::doBalance_(AVLTree::node_pointer node) {
    fixHeight(node);
    fixCount(node);

    switch (balance(node)) {
        case 2: {
            if (balance(node->_r_child) < 0) {
                node->_r_child = _turn_right_(node->_r_child);
            }
            return _turn_left_(node);
        }
        case -2: {
            if (balance(node->_l_child) > 0) {
                node->_l_child = _turn_left_(node->_l_child);
            }
            return _turn_right_(node);
        }
        default:
            return node;
    }
}

template<typename Key, class Compare>
typename AVLTree<Key, Compare>::node_pointer AVLTree<Key, Compare>
::_turn_right_(AVLTree::node_pointer current) {
    auto tmp = current->_l_child;
    current->_l_child = tmp->_r_child;
    tmp->_r_child = current;

    fixHeight(current);
    fixCount(current);
    fixHeight(tmp);
    fixCount(tmp);

    return tmp;
}

template<typename Key, class Compare>
typename AVLTree<Key, Compare>::node_pointer AVLTree<Key, Compare>
::_turn_left_(AVLTree::node_pointer current) {

    auto tmp = current->_r_child;
    current->_r_child = tmp->_l_child;
    tmp->_l_child = current;

    fixHeight(current);
    fixCount(current);
    fixHeight(tmp);
    fixCount(tmp);

    return tmp;
}

template<typename Key, class Compare>
typename AVLTree<Key, Compare>::node_pointer AVLTree<Key, Compare>
::_get_leftest_node_(AVLTree::node_pointer node) const {
    for (; node->_l_child ;) {
        node = node->_l_child;
    }
    return node;
}

template<typename Key, class Compare>
typename AVLTree<Key, Compare>::node_pointer AVLTree<Key, Compare>
::_get_rightest_node_(AVLTree::node_pointer node) const {
    for (; node->_r_child;) {
        node = node->_r_child;
    }
    return node;
}

template<typename Key, class Compare>
size_t AVLTree<Key, Compare>::height(AVLTree::node_pointer node) const {
    return node ? node->_height : 0;
}

template<typename Key, class Compare>
size_t AVLTree<Key, Compare>::count(AVLTree::node_pointer node) const {
    return node ? node->_count : 0;
}

template<typename Key, class Compare>
void AVLTree<Key, Compare>::fixHeight(AVLTree::node_pointer node) {
    node->_height = std::max(height(node->_l_child), height(node->_r_child)) + 1;
}

template<typename Key, class Compare>
void AVLTree<Key, Compare>::fixCount(AVLTree::node_pointer node) {
    node->_count = count(node->_l_child) + count(node->_r_child) + 1;
}


template<typename Key, class Compare>
int32_t AVLTree<Key, Compare>::balance(AVLTree::node_pointer node) {
    return height(node->_r_child) - height(node->_l_child);
}

template<typename Key, class Compare>
void AVLTree<Key, Compare>::clear(node_pointer node) {
    if (node == nullptr) {
        return;
    }
    clear(node->_l_child);
    clear(node->_r_child);
    delete node;
}

template<typename Key, class Compare>
AVLTree<Key, Compare>::btNode::btNode(
        Key data,
        int32_t height,
        size_t count,
        AVLTree::btNode *l_child,
        AVLTree::btNode *r_child):
        _data(data),
        _height(_height),
        _count(count),
        _l_child(l_child),
        _r_child(r_child) {
}

template<typename Key, class Compare>
AVLTree<Key, Compare>::btNode
::btNode(const btNode &other)
        : _data(other._data),
          _height(other._height),
          _count(other._count),
          _l_child(other._l_child),
          _r_child(other._r_child) {
}

template<typename Key, class Compare>
AVLTree<Key, Compare>::btNode
::btNode(btNode &&other) noexcept
: btNode(other) {
        delete other;
        other = nullptr;
}

template<typename Key, class Compare>
AVLTree<Key, Compare>::btNode
::~btNode() {
    _l_child = nullptr;
    _r_child = nullptr;
}

template<typename Key, class Compare>
typename AVLTree<Key, Compare>
::btNode &AVLTree<Key, Compare>
::btNode::operator=(const AVLTree::btNode &other) {
    _data = other._data;
    _height = other._height;
    _count = other._count;
    _l_child = other._l_child;
    _r_child = other._r_child;
}

template<typename Key, class Compare>
typename AVLTree<Key, Compare>
::btNode &AVLTree<Key, Compare>
::btNode::operator=(btNode &&other) noexcept {
*this = other;

other._l_child = nullptr;
other._r_child = nullptr;
}

template<typename Key, class Compare>
std::ostream &operator<<(std::ostream &out, AVLTree<Key, Compare> &tree) {
    auto tmp = tree.get_root();
    std::unordered_set<typename AVLTree<Key, Compare>::btNode *> visited;

    for (; tmp && visited.find(tmp) == visited.end();) {
        if (tmp->_l_child &&
            visited.find(tmp->_l_child) == visited.end()) {
            tmp = tmp->_l_child;
        } else if (tmp->_r_child &&
                   visited.find(tmp->_r_child) == visited.end()) {
            tmp = tmp->_r_child;
        } else {
            out << tmp->_data << " ";
            visited.insert(tmp);
            tmp = tree.get_root();
        }
    }
    return out;
}

template<typename Key, class Compare>
typename AVLTree<Key, Compare>
::const_ref_type AVLTree<Key, Compare>::order_statistics(size_t k) const {
    if (k < count(_root)) {
        node_pointer node = _root;

        for (; ;) {
            if (k > count(node->_l_child)) {
                k -= (count(node->_l_child) + 1);
                node = node->_r_child;
            } else if (k < count(node->_l_child)){
                node = node->_l_child;
            }else{
                break;
            }
        }
        return node->_data;
    } else if(k == 0){
        return _get_leftest_node_(_root)->_data;
    }else{
        return _get_rightest_node_(_root)->_data;
    }
}

int main(){
    AVLTree<int> tree;
    size_t count = 0;

    std::cin >> count;

    for (size_t i = 0; i < count; ++i) {
        int32_t value;
        size_t k;

        std::cin >> value >> k;

        if (value > 0) {
            tree.insert(value);
        } else {
            tree.erase(-value);
        }

        std::cout << tree.order_statistics(k) << std::endl;
    }
}
