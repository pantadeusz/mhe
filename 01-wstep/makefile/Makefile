CPP=g++
OBJS=$(APPNAME).o
LIBS=
CFLAGS=-Wall

APPNAME=mhedemo

all: $(APPNAME)

$(APPNAME): $(OBJS)
	$(CPP) $^ $(CFLAGS) -o $@

%.o : %.c
	@echo $<
	@gcc $(LIBS) -c $<

clean:
	rm -f $(OBJS) $(APPNAME)
