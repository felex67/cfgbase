all:test
test:cfgbase.o test.o
	g++ cfgbase.cpp test.cpp -o test
	./test
	rm -rf *.o
cfgbase.o:
	g++ -c cfgbase.cpp -o cfgbase.o
test.o:
	g++ -c test.cpp -o test.o

clean:
	rm -rf *.o