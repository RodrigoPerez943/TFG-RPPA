DEPS =  
SYSROOT = ${CURDIR}/../../env/toolchain/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi/

CFLAGS = -I$(SYSROOT)/usr/include -I. -I../ipc/Bin/ -I./include/ --sysroot=$(SYSROOT)
LFLAGS = -L$(CURDIR)/../ipc/Bin/ -L$(CURDIR)/../decoder_reg/  -lrt -lpthread -lIPC -lm

OBJ = uartReader.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

uartReader: $(OBJ)
	$(CC) -o uartReader $(CFLAGS) $(OBJ) $(LFLAGS)

clean:
	rm *.o uartReader
