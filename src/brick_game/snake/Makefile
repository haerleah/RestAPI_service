CXX = g++
CXXFLAGS = -std=c++17 -Wall -Werror -Wextra -pedantic -lpthread
LIBFLAGS = -shared -fPIC

OUTPUT = libs21_snake.so

SRC_FILES = s21_controller.cpp \
			s21_snake_facade.cpp \
			s21_snake.cpp

all: compile_library

compile_library: $(SRC_FILES)
	$(CXX) $(CXXFLAGS) $(LIBFLAGS) $(SRC_FILES) -o $(OUTPUT)

clean:
	rm -rf $(OUTPUT)