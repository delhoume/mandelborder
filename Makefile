# Makefile for Mandelbrot SDL2 renderer

CXX = g++
CXXFLAGS = -std=c++14 -Wall -O3
LDFLAGS = -lSDL2

# macOS specific flags
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    CXXFLAGS += -I/opt/homebrew/include -I/usr/local/include
    LDFLAGS += -L/opt/homebrew/lib -L/usr/local/lib
endif

# Linux specific flags
ifeq ($(UNAME_S),Linux)
    LDFLAGS += -lm
endif

TARGET = mandelbrot_sdl2
SOURCES = main.cpp mandelbrot_app.cpp mandelbrot_calculator.cpp
OBJS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
