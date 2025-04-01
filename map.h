#pragma once
#include <iostream>
#include <utility>
#include <stack>
#include <algorithm>

using namespace std;

// Узел дерева, хранящий пару ключ-значение
template <typename Key, typename Value>
struct Node {
    pair<Key, Value> data;  // Ключ и значение
    Node* parent;           // Указатель на родителя
    Node* left;             // Указатель на левого потомка
    Node* right;            // Указатель на правого потомка
    int height;             // Высота поддерева (для AVL)
    bool color;             // Цвет узла (для красно-черного дерева)

    // Конструктор узла
    Node(const Key& key, const Value& value, Node* p = nullptr, Node* l = nullptr, Node* r = nullptr)
        : data(make_pair(key, value)), parent(p), left(l), right(r), height(1), color(true) {}
};

// Базовый класс Map с общими функциями
template <typename Key, typename Value, typename Compare = less<Key>>
class Map {
public:
    using NodeType = Node<Key, Value>;  // Псевдоним для типа узла
    
protected:
    NodeType* root;   // Корень дерева
    Compare comp;     // Функтор сравнения

    // Рекурсивное удаление всех узлов
    void clear(NodeType* node) {
        if (node) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

    // Рекурсивное копирование узлов
    NodeType* copy(NodeType* node, NodeType* parent) {
        if (!node) return nullptr;
        NodeType* newNode = new NodeType(node->data.first, node->data.second, parent);
        newNode->left = copy(node->left, newNode);
        newNode->right = copy(node->right, newNode);
        return newNode;
    }

public:
    // Класс итератора для обхода дерева
    class Iterator {
    private:
        NodeType* current;           // Текущий узел
        stack<NodeType*> nodeStack;   // Стек для inorder-обхода

        // Заполнение стека левыми потомками
        void pushLeft(NodeType* node) {
            while (node) {
                nodeStack.push(node);
                node = node->left;
            }
        }

    public:
        Iterator(NodeType* root = nullptr) {
            pushLeft(root);
            if (!nodeStack.empty()) {
                current = nodeStack.top();
                nodeStack.pop();
            } else {
                current = nullptr;
            }
        }

        // Доступ к данным текущего узла
        pair<Key, Value>& operator*() { return current->data; }
        pair<Key, Value>* operator->() { return &current->data; }

        // Переход к следующему узлу
        Iterator& operator++() {
            if (current->right) {
                pushLeft(current->right);
            }
            if (nodeStack.empty()) {
                current = nullptr;
            } else {
                current = nodeStack.top();
                nodeStack.pop();
            }
            return *this;
        }

        // Сравнение итераторов
        bool operator!=(const Iterator& other) const { return current != other.current; }
        bool operator==(const Iterator& other) const { return current == other.current; }
    };

    // Конструкторы и деструктор
    Map() : root(nullptr) {}
    Map(const Map& other) { root = copy(other.root, nullptr); }
    ~Map() { clear(); }

    // Оператор присваивания
    Map& operator=(const Map& other) {
        if (this != &other) {
            clear();
            root = copy(other.root, nullptr);
        }
        return *this;
    }

    // Проверка на пустоту
    bool empty() const { return root == nullptr; }
    
    // Очистка дерева
    void clear() { clear(root); root = nullptr; }

    // Итераторы
    Iterator begin() { return Iterator(root); }
    Iterator end() { return Iterator(nullptr); }

    // Поиск по ключу
    Iterator find(const Key& key) {
        NodeType* node = root;
        while (node) {
            if (comp(key, node->data.first)) {
                node = node->left;
            } else if (comp(node->data.first, key)) {
                node = node->right;
            } else {
                return Iterator(node);
            }
        }
        return end();
    }

    // Доступ по ключу с автоматическим созданием
    Value& operator[](const Key& key) {
        NodeType* node = root;
        NodeType* parent = nullptr;

        while (node) {
            parent = node;
            if (comp(key, node->data.first)) {
                node = node->left;
            } else if (comp(node->data.first, key)) {
                node = node->right;
            } else {
                return node->data.second;
            }
        }

        // Создание нового узла, если ключ не найден
        node = new NodeType(key, Value(), parent);
        if (!parent) {
            root = node;
        } else if (comp(key, parent->data.first)) {
            parent->left = node;
        } else {
            parent->right = node;
        }

        return node->data.second;
    }

    // Виртуальные методы для удаления (реализуются в производных классах)
    virtual void erase(const Key& key) = 0;
    virtual void erase(Iterator pos) = 0;
};