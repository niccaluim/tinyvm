CFLAGS=-O3 -std=c89 -pedantic -Wall

sources=main.c vm.c
objects=$(sources:.c=.o)
deps=$(sources:.c=.d)

vm: $(objects)
	$(CC) $(CFLAGS) $(LDFLAGS) -o vm $(objects)

%.d: %.c
	$(CC) $(CFLAGS) -MM -MF $@ $<

-include $(deps)

clean:
	rm -f vm $(objects) $(deps) config.h

.PHONY: clean
