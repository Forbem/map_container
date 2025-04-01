CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

all: map_rb map_avl

map_rb: test-map.cpp map_rb.h map.h
	$(CXX) $(CXXFLAGS) -o map_rb test-map.cpp

map_avl: test-map.cpp map_avl.h map.h
	$(CXX) $(CXXFLAGS) -o map_avl test-map.cpp

clean:
	rm -f map_rb map_avl

test: all
	./map_rb
	./map_avl

.PHONY: all clean test