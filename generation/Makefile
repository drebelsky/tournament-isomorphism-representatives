default: main combine

debug: main_debug

clean:
	rm -rf *.dSYM
	rm main main_debug

main: main.cpp
	g++ -o main main.cpp -std=c++17 -O3 -Wall -Wextra -Wconversion -pedantic

combine: combine.cpp
	g++ -o combine combine.cpp -std=c++17 -O3 -Wall -Wextra -Wconversion -pedantic

main_debug: main.cpp
	g++ -o main_debug main.cpp -std=c++17 -Og -g -Wall -Wextra -Wconversion -pedantic

.PHONY: default debug clean
