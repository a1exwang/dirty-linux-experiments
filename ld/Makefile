lib1.so:
	gcc -fPIC -o lib1.so --shared lib1.c

fakelib2.so:
	gcc -fPIC -o fakelib2.so --shared fakelib2.c

main: lib1.so
	gcc -fPIC -o main lib1.so main.c

all: main

clean:
	rm -f *.so main
