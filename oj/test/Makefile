# # VERSION 1
# app: main.cpp Proxy.cpp RealSubject.cpp
# 	g++ -o app main.cpp Proxy.cpp RealSubject.cpp

# # VERSION 2
# CXX = g++
# TARGET = app 
# OBJ = main.o Proxy.o RealSubject.o

# $(TARGET): $(OBJ)
# 	$(CXX) -o $(TARGET) $(OBJ)

# main.o: main.cpp
# 	$(CXX) -c main.cpp

# Proxy.o: Proxy.cpp
# 	$(CXX) -c Proxy.cpp

# RealSubject.o: RealSubject.cpp
# 	$(CXX) -c RealSubject.cpp

# # VERSION 3
# CXX = g++
# TARGET = app 
# OBJ = main.o Proxy.o RealSubject.o

# CXXFLAGS = -c -Wall

# $(TARGET): $(OBJ)
# 	$(CXX) -o $@ $^

# %.o: %.cpp
# 	$(CXX) $(CXXFLAGS) $< -o $@

# .PHONY: clean
# clean:
# 	rm -f *.o $(TARGET)

# # VERSION 4
# CXX = g++
# TARGET = app 
# SRC = $(wildcard *.cpp)
# OBJ = $(patsubst %.cpp, %.o, $(SRC))

# CXXFLAGS = -c -Wall

# $(TARGET): $(OBJ)
# 	$(CXX) -o $@ $^

# %.o: %.cpp
# 	$(CXX) $(CXXFLAGS) $< -o $@

# .PHONY: clean
# clean:
# 	rm -f *.o $(TARGET)