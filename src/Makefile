ueye-objs = UeyeCamera.o UeyeInterface.o UeyeDetInfoCtrlObj.o UeyeVideoCtrlObj.o UeyeSyncCtrlObj.o

SRCS = $(ueye-objs:.o=.cpp) 

CXXFLAGS += -I../include -I../../../hardware/include -I../../../common/include \
	-lueye_api -D__LINUX__ -Wall -pthread -fPIC -g

all:	Ueye.o

Ueye.o:	$(ueye-objs)
	$(LD) -o $@ -r $+

clean:
	rm -f *.o *.P

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)
