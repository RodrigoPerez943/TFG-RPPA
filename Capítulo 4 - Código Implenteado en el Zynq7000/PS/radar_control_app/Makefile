DEPS =  
SYSROOT = ${CURDIR}/../../env/toolchain/sysroots/cortexa9t2hf-neon-xilinx-linux-gnueabi/

CFLAGS = -I$(SYSROOT)/usr/include -I. -I../ipc/Bin/ -I./include/ --sysroot=$(SYSROOT) -ftree-vectorize -mvectorize-with-neon-quad -ffast-math  -Wall
#SFLAGS = -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a9
LFLAGS = -L$(CURDIR)/../ipc/Bin/ -lrt -lpthread -lIPC -lm 

OBJ = ipc_client.o adc.o radar_control_app.o 

#%.o: %.s $(DEPS)
#	  $(AS) $(SFLAGS) -c $< -o $@

%.o: %.c $(DEPS)
	  $(CC) $(CFLAGS) -fPIC -c $< -o $@

radar_control_app: $(OBJ)
	  $(CC) -o radar_control_app $(CFLAGS) $(OBJ) $(LFLAGS)
	  rm *.o 

clean:
	  rm *.o radar_control_app


