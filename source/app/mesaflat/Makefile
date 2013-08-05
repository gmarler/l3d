CURRENT_PATH_TO_TOPLEVEL=../../..
include $(CURRENT_PATH_TO_TOPLEVEL)/Makefile.vars

.SUFFIXES: .cc .o

GLIDECCFLAGS = -DFX -DXMESA
CCFLAGS =-g -fomit-frame-pointer -m486 -Wall -pipe -I$(MESADIR)/include $(GLIDECCFLAGS)

# LINKOPTS does not use LIBS from Makefile.vars because it doesn't use the
# l3d library, thus it defines explicitly which libs it wants

LINKOPTS=$(GLIDE_LIBS) $(MESA_LIBS) $(X_LIBS) $(PROFILE_FLAGS)

.cc.o:
	$(CC) -c $(CCFLAGS) $< -o $@

mesaflat: mesaflat.o
	$(LD) $(LINKOPTS) $^ -o $@

test_ogl: test_ogl.o
	$(LD) $(LINKOPTS) $^ -o $@

test_3d: test_3d.o
	$(LD) $(LINKOPTS) $^ -o $@

