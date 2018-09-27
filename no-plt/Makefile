all:
	mkdir -p build
	gcc -o build/test.plt test.c
	gcc -o build/test.noplt -fno-plt test.c
	ltrace build/test.plt
	ltrace build/test.noplt
	objdump -d build/test.plt
	objdump -d build/test.noplt

clean:
	rm -r build
