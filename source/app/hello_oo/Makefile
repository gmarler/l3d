LD = g++
LDFLAGS = -L/usr/X11R6/lib -lX11 -lXt -lm
CPPFLAGS = -g

ch2_hello_oo:\
  ch2_hello.o\
  ch2_app.o\
  ch2_app_x.o\
  ch2_appfactory_x.o\
  ch2_factorymanager.o
	$(LD) $(LDFLAGS) $^ -o $@
