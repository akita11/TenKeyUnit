CC := sdcc
#CC := c:\"Program Files"\SDCC\bin\sdcc

bin_name := TenKeyUnit
BUILD_DIR := ./build/

# CH552 xRAM size 1kB = 0x400, Flash=16k(0x4000)
#C_FLAGS := -V -mmcs51 --model-small --xram-size 0x400 --xram-loc 0x0000 --code-size 0xF000 -o $(BUILD_DIR)
# STC8G1K08 xRAM size 1kB=0x400, Flash=8kB(0x2000)
C_FLAGS := -V -mmcs51 --model-small --xram-size 0x400 --xram-loc 0x0000 --code-size 0x2000 -o $(BUILD_DIR)

all: main
	$(CC) ./build/main.rel -o ./build/$(bin_name).ihx
#	gobjcopy -I ihex -O binary $(join $(BUILD_DIR),$(bin_name).ihx) $(join $(BUILD_DIR),$(bin_name).bin)
	sdobjcopy -I ihex -O binary $(join $(BUILD_DIR),$(bin_name).ihx) $(join $(BUILD_DIR),$(bin_name).bin)
#	c:\"Program Files"\SDCC\bin\sdobjcopy -I ihex -O binary $(join $(BUILD_DIR),$(bin_name).ihx) $(join $(BUILD_DIR),$(bin_name).bin)

main: ./src/main.c 
	$(CC) -c $(C_FLAGS) $^ -I./inc -o $(BUILD_DIR)

clean:
	rm ./build/*
