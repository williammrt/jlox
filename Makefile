all: jlox

jlox: *.cpp *.h
	g++ -std=c++17 -o jlox *.cpp -Wall -Werror -g;
clean:
	rm jlox;