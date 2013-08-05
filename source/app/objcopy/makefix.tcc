CXX	= tcc
CC	= tcc
.SUFFIXES: .cc .obj
OUTPUT	= ..\..\..\binaries\tccdos\fixed\app\objcopy

AR = tlib
CPPFLAGS = -DBORLAND16_DOS -DFIXED_POINT_MATH -mhuge -P -G \
	-I$(TCDIR)\INCLUDE;..\..\..\LIB -L$(TCDIR)\LIB -n$(OUTPUT)
LD = tlink
LDFLAGS = /c /x /L$(TCDIR)\LIB;..\..\..\binaries\tccdos\fixed\app\lib C0H
LIBS = CH EMU MATHH LIB3D

.cc.obj:
	$(CXX) -c $(CPPFLAGS) $<

TARGET  =  objcopy.exe
OBJS    =  main.obj shapes.obj

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $**, $<,  , $(LIBS)
	copy $(TARGET) $(OUTPUT)
	copy *.obj $(OUTPUT)
	del $(TARGET)
	del $(OUTPUT)
