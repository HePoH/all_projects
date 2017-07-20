vpath %.h ../include/

SOURCES = add.c sub.c mul.c div.c
LIBS = $(addprefix lib, $(SOURCES:.c=.so))
OBJECTS = $(SOURCES:.c=.o)

all: $(LIBS)

lib%.so: %.o
		gcc -shared $< -o ../plugins/$@

$(OBJECTS): $(SOURCES)
		gcc -c $(@:.o=.c) -fPIC

$(SOURCES): cmplx_num.h
