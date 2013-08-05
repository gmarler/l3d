CXX = CC
LD = CC
LDFLAGS = -L/usr/X11R6/lib -lX11 -lXt -lm
CPPFLAGS = # -xO3

hello: hello.o
	$(LD) $(LDFLAGS) $^ -o $@
