BASEDIR = ..
include ../Makefile.in
OBJS = Map.o LMException.o Player.o Polygon.o Point.o PacketReader.o PacketWriter.o CommonSDL.o RawPacket.o math.o misc.o \
	PathManager.o #put .o filenames here (DO NOT PUT .cpp!)

all: ../liblmcommon.a

../liblmcommon.a: $(OBJS)
	$(AR) crus ../liblmcommon.a $(OBJS)

clean:
	rm -f $(OBJS) *~

# ==================================== #

CommonSDL.o: $(H_COMMONSDL) $(H_LMEXCEPTION) $(H_SDL)
LMException.o: $(H_LMEXCEPTION)
