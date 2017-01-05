OBJ = main.o button.o turn.o action.o playsound.o process.o capture.o bmp.o raspicam.o private/mutex.o private/private_impl.o private/threadcondition.o private/yuv_conversion.o

DEPS =

LDFLAGS = 	-L/opt/vc/lib
INC = 	 -Iprivate -I. 
LIBS = -lbcm2835 -lmmal_core -lmmal_util -lmmal_vc_client -lvcos -lbcm_host -lrt -lGLESv2 -lEGL
CFLAGS =  -g -std=c++0x  -lrt

CC = g++

EXTENSION = .cpp

%.o:	%$(EXTENSION) $(DEPS)
	$(CC)  $(INC) -c -o $@ $< $(CFLAGS)
	
all:	camera_gpio

camera_gpio:	$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(LDFLAGS)
	
.PHONY:	clean

clean:
	rm -f *.o private/*.o mmal/*.o *~ core *~
