default:
	gcc -Wall -Wextra -std=c11 -g -c bitmap.c
	gcc -Wall -Wextra -std=c11 -g -c memory_manager.c
	ar rcs memory_manager.a bitmap.o memory_manager.o

test:
	gcc -o test test_main.c memory_manager.a
	./test
	rm test

clean:
	rm -f *.o *.a test