CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Iinclude
TARGET := library_manager
SOURCES := src/main.cpp src/Book.cpp src/Member.cpp src/Loan.cpp src/LibraryManager.cpp

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(SOURCES) include/Book.h include/Member.h include/Loan.h include/LibraryManager.h
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe
