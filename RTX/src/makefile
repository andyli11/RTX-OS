CC = gcc       # compiler
CFLAGS = -Wall -g -std=c99 # compilation flags
LD = gcc       # linker
LDFLAGS = -g   # debugging symbols in build
LDLIBS = 
LIBDIR = lib
BUILDDIR = build

LIB_UTIL = $(BUILDDIR)/doubly_linked_list.o

OBJ_LIB_TEST = $(BUILDDIR)/lib_test.o $(LIB_UTIL)

TARGETS = 
TESTS = lib_test

default: all

all: $(TARGETS)
tests: $(TESTS)

lib_test: $(OBJ_LIB_TEST)
	$(CC) -o $@ $^ $(LDLIBS) $(LDFLAGS)

$(BUILDDIR)/%.o : %.c
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.o : $(LIBDIR)/%.c
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) $(BUILDDIR)/*.o *.o
