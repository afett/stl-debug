CXX ?= g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++11 -g

TARGET = stl-debug

SRC = $(wildcard *.cc)
OBJ = $(SRC:%.cc=%.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $(OBJ)

clean:
	rm -rf $(OBJ) $(TARGET)

.PHONY: all
