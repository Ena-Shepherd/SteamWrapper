all: build-static clean example

clean:
	rm -rf *.o

build-static:
	g++ -c ./src/*.cpp -I"./SteamAPI/include"
	ar rcs libeasysteam.a *.o

example:
	g++ Example/main.cpp -L"./" -leasysteam -L"./SteamAPI" -lsteam_api64 -o example

fclean: clean
	rm -f libeasysteam.a
	rm -f *.exe

.PHONY: clean fclean example build-static