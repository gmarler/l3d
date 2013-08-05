LD = g++
LDFLAGS = -L/usr/X11R6/lib -lX11 -lXt -lm
CPPFLAGS = -O3

hello: hello.o
	$(LD) $(LDFLAGS) $^ -o $@
