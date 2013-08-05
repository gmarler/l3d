OLD_DIR_OO = ../hello_oo
OLD_DIR_XEVENT = ../hello_oo_event

LD = g++
LDFLAGS = -L/usr/X11R6/lib -lX11 -lXt -lm
CPPFLAGS = -g -I$(OLD_DIR_OO) -I$(OLD_DIR_XEVENT)

ch2_hello_oo_image:\
  $(OLD_DIR_OO)/ch2_hello.o\
  $(OLD_DIR_OO)/ch2_app.o\
  $(OLD_DIR_OO)/ch2_app_x.o\
  $(OLD_DIR_OO)/ch2_appfactory_x.o\
  $(OLD_DIR_XEVENT)/ch2_app_xevent.o \
  $(OLD_DIR_XEVENT)/ch2_appfactory_xevent.o \
  ch2_factorymanager.o \
  ch2_app_ximage.o \
  ch2_appfactory_ximage.o
	$(LD) $(LDFLAGS) $^ -o $@

