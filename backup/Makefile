# 资料备份工具 Makefile
# 支持 GCC (MinGW) 和 Clang 编译

CC = gcc
WINDRES = windres
CFLAGS = -Wall -Wextra -O2 -DUNICODE -D_UNICODE
LDFLAGS =

# 如果使用 Clang，取消下面注释
# CC = clang

TARGET = backup.exe
SRC = main.c
RES = resource.rc
RES_OBJ = resource.o

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC) $(RES_OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(RES_OBJ) $(LDFLAGS)

$(RES_OBJ): $(RES)
	$(WINDRES) -o $(RES_OBJ) $(RES)

clean:
	del /Q $(TARGET) $(RES_OBJ) 2>nul || rm -f $(TARGET) $(RES_OBJ)

run: $(TARGET)
	./$(TARGET)

# 调试版本
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)
