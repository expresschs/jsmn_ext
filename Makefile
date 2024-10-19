CC=gcc

example: example.c
	$(CC) -o $@ $^ 

clean:
	rm -f *.o example

