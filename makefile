all:helloFF
CC=g++
CLIBSFLAGS=-lavformat  -lavdevice -lavcodec  -lavutil -pthread \
	-ldl -lswscale -lbz2  -lasound -lz -lm -lswresample -llzma#  -lX11 -lva \
	#-lvdpau -lva-drm -lva-x11
helloFF:helloFF.o 
	$(CC) -o helloFF helloFF.o $(CLIBSFLAGS)
helloFF.o:helloFF.cpp
	$(CC) -o helloFF.o -c helloFF.cpp  $(CLIBSFLAGS)
clean:
	rm helloFF helloFF.o