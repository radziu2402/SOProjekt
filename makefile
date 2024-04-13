all: office

office: src/main.cpp
		mkdir -p build
		g++ $(shell find src/ -type f -name '*.cpp') -MMD -MP -o build/office -lncurses -pthread

clean:
	rm -r build