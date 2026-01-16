CXX ?= g++
TDZDD_HOME ?= ./tdzdd

CXXFLAGS ?= -O2 -std=c++17 -DNDEBUG -I$(TDZDD_HOME)/include
LDFLAGS ?=

TARGET = hamilton
SRCS = main.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: run clean

run: $(TARGET)
	./$(TARGET) grid12x12.grh

clean:
	rm -f $(TARGET)
