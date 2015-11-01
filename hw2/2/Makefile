CC=gcc
LDIR=/usr/X11R6/lib
LIBS= -L $(LDIR) -lX11 -lpthread
all:
	$(CC) mandelGUI.c mandelCore.c -o mandelGUI $(LIBS)
