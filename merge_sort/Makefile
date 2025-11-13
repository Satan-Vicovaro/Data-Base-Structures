CXX = g++
CXXFLASGS += -std=c++17 -Wall -O2 -D_FORTIFY_SOURCE=2

SRC_DIR = .
BUILD_DIR = build

TARGET = ./output/SortProgram

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLASGS) $^ -o $@
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLASGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean: 
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
