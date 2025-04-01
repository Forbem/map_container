#pragma once
#include "map.h"

template <typename Key, typename Value, typename Compare = less<Key>>
class MapAVL : public Map<Key, Value, Compare> {
public:
    using Base = Map<Key, Value, Compare>;
    using Node = typename Base::NodeType;
    using Iterator = typename Base::Iterator;

protected:
    // Высота поддерева
    int height(Node* node) { return node ? node->height : 0; }
    
    // Коэффициент баланса
    int balanceFactor(Node* node) { 
        return node ? height(node->left) - height(node->right) : 0; 
    }
    
    // Обновление высоты узла
    void updateHeight(Node* node) { 
        node->height = 1 + max(height(node->left), height(node->right)); 
    }

    // Левый поворот
    void leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        
        if (!x->parent) {
            this->root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        
        y->left = x;
        x->parent = y;
        
        updateHeight(x);
        updateHeight(y);
    }

    // Правый поворот
    void rightRotate(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        if (x->right) x->right->parent = y;
        x->parent = y->parent;
        
        if (!y->parent) {
            this->root = x;
        } else if (y == y->parent->left) {
            y->parent->left = x;
        } else {
            y->parent->right = x;
        }
        
        x->right = y;
        y->parent = x;
        
        updateHeight(y);
        updateHeight(x);
    }

    // Балансировка дерева
    void balance(Node* node) {
        while (node) {
            updateHeight(node);
            int bf = balanceFactor(node);
            
            if (bf > 1) {
                if (balanceFactor(node->left) < 0) 
                    leftRotate(node->left);
                rightRotate(node);
            } else if (bf < -1) {
                if (balanceFactor(node->right) > 0) 
                    rightRotate(node->right);
                leftRotate(node);
            }
            node = node->parent;
        }
    }

public:
    // Удаление по ключу
    void erase(const Key& key) override {
        Node* node = this->root;
        while (node) {
            if (this->comp(key, node->data.first)) {
                node = node->left;
            } else if (this->comp(node->data.first, key)) {
                node = node->right;
            } else {
                break;
            }
        }
        if (!node) return;

        if (!node->left) {
            Node* right = node->right;
            if (node->parent) {
                if (node == node->parent->left) 
                    node->parent->left = right;
                else 
                    node->parent->right = right;
                if (right) right->parent = node->parent;
                balance(node->parent);
            } else {
                this->root = right;
                if (right) right->parent = nullptr;
            }
            delete node;
        } else if (!node->right) {
            Node* left = node->left;
            if (node->parent) {
                if (node == node->parent->left) 
                    node->parent->left = left;
                else 
                    node->parent->right = left;
                if (left) left->parent = node->parent;
                balance(node->parent);
            } else {
                this->root = left;
                if (left) left->parent = nullptr;
            }
            delete node;
        } else {
            Node* successor = node->right;
            while (successor->left) successor = successor->left;
            node->data = successor->data;
            erase(successor->data.first);
        }
    }

    // Удаление по итератору
    void erase(Iterator pos) override { erase(pos->first); }
};