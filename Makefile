all: jlox

jlox: *.cpp *.h
	g++ -std=c++17 -o jlox *.cpp -Wall -Werror -O0;
clean:
	rm jlox;