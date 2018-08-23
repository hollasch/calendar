!if defined(debug) && ("$(debug)" != "0") && ("$(debug)" != "false")
ccopts = -Zi
!else
ccopts = -Ox
!endif

all: calendar

calendar: calendar.exe

calendar.exe: calendar.cpp
	cl -nologo $(ccopts) -D_CRT_SECURE_NO_DEPRECATE calendar.cpp

clean:
    -del 2>nul *.obj *.pdb *.ilk

clobber: clean
	-del 2>nul /q calendar.exe

fresh: clobber all

$(BINDIR)\calendar.exe: calendar.exe
    if defined BINDIR copy /y $? %%BINDIR%%

install: $(BINDIR)\calendar.exe
