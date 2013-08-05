OLDDIR_OO = ../hello_oo

CXX = CC
LD = CC
LDFLAGS = -L/usr/X11R6/lib -lX11 -lXt -lm
CPPFLAGS = -g -I$(OLDDIR_OO)

ch2_hello_oo_event:\
  $(OLDDIR_OO)/ch2_hello.o\
  $(OLDDIR_OO)/ch2_app.o\
  $(OLDDIR_OO)/ch2_app_x.o\
  $(OLDDIR_OO)/ch2_appfactory_x.o\
  ch2_factorymanager.o \
  ch2_app_xevent.o \
  ch2_appfactory_xevent.o
	$(LD) $(LDFLAGS) $^ -o $@
