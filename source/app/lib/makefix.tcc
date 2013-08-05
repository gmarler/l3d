# DOS Borland Turbo C++ makefile for "S3D" 3D library code

MATHFLAG = -DFIXED_POINT_MATH
OUTPUT   = ..\..\..\binaries\tccdos\fixed\app\lib

CXX	= tcc
CC	= tcc
# TCDIR MUST BE DEFINED EITHER HERE OR ELSEWHERE
.SUFFIXES: .cc .obj

AR = tlib
CPPFLAGS =	-DBORLAND16_DOS $(MATHFLAG) -G -mhuge -P \
		-I$(TCDIR)\INCLUDE -L$(TCDIR)\LIB

.cc.obj:
	$(CXX) -c $(CPPFLAGS) $<

TARGET  =  $(OUTPUT)\lib3d.lib
SRCS    =  camera.cc rasteriz.cc screen.cc world.cc object3d.cc sincos.cc math_fix.cc matrix.cc
OBJS    =  camera.obj rasteriz.obj screen.obj world.obj object3d.obj sincos.obj math_fix.obj matrix.obj

ALL	= $(TARGET)

all : $(ALL)

$(TARGET) : $(OBJS)
	$(AR) $(TARGET) +-camera.obj
	$(AR) $(TARGET) +-rasteriz.obj
	$(AR) $(TARGET) +-screen.obj
	$(AR) $(TARGET) +-world.obj
	$(AR) $(TARGET) +-object3d.obj
	$(AR) $(TARGET) +-sincos.obj
	$(AR) $(TARGET) +-math_fix.obj
	$(AR) $(TARGET) +-matrix.obj
	copy camera.obj $(OUTPUT)
	copy rasteriz.obj $(OUTPUT)
	copy screen.obj $(OUTPUT)
	copy world.obj $(OUTPUT)
	copy object3d.obj $(OUTPUT)
	copy sincos.obj $(OUTPUT)
	copy math_fix.obj $(OUTPUT)
	copy matrix.obj $(OUTPUT)
	del camera.obj
	del rasteriz.obj
	del screen.obj
	del world.obj
	del object3d.obj
	del sincos.obj
	del math_fix.obj
	del matrix.obj
