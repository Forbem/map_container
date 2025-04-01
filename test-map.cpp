#include <iostream>
#include <string>
#include "map_rb.h"
#include "map_avl.h"

using namespace std;

// Шаблонная функция для тестирования любого типа map
template<typename MapType>
void testMap(MapType& map, const string& name) {
    cout << "===== Testing " << name << " =====\n";

    // Тест вставки элементов
    cout << "\nInserting elements...\n";
    map[1] = "one";
    map[2] = "two";
    map[3] = "three";
    map[4] = "four";

    // Тест доступа к элементам
    cout << "map[1] = " << map[1] << endl;
    cout << "map[2] = " << map[2] << endl;
    cout << "map[3] = " << map[3] << endl;

    // Тест поиска
    cout << "\nSearching for key 2...\n";
    auto it = map.find(2);
    if (it != map.end()) {
        cout << "Found key 2: " << it->second << endl;
    } else {
        cout << "Key 2 not found\n";
    }

    // Тест удаления
    cout << "\nErasing key 2...\n";
    map.erase(2);
    cout << "After erase(2), map[2] exists? " << (map.find(2) != map.end() ? "Yes" : "No") << endl;

    // Тест итерации
    cout << "\nIterating through map:\n";
    for (auto& p : map) {
        cout << p.first << ": " << p.second << endl;
    }

    // Тест очистки
    cout << "\nClearing map...\n";
    map.clear();
    cout << "Map is now " << (map.empty() ? "empty" : "not empty") << endl;
    cout << "========================\n\n";
}

int main() {
    // Тестирование красно-черного дерева
    MapRB<int, string> rbMap;
    testMap(rbMap, "Red-Black Tree");

    // Тестирование AVL дерева
    MapAVL<int, string> avlMap;
    testMap(avlMap, "AVL Tree");

    return 0;
}