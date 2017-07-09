all:
	cd src; qmake; make

clean:
	cd src; qmake; make clean
	rm -f src/Makefile
	rm -f obj/*.o
	rm -f bin/CDoc
