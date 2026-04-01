CXX = g++
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
TARGET = app

CXXFLAGS := -std=c++17 -Wall -Wextra -I$(INC_DIR) $(shell pkg-config --cflags opencv4)
LDFLAGS := $(shell pkg-config --libs opencv4)

CPPS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%_cpp.o, $(CPPS))

all: $(BUILD_DIR) $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%_cpp.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: all
	./$(TARGET) $(arg)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean run