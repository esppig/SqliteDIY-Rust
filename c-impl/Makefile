CC = clang
# -Wall 表示输出所有类型的 warning；-g 会创建符号表，方便调试；
CFLAGS += -Wall -g
TARGET = db

$(TARGET):
	$(CC) $(CFLAGS) main.c table.c cursor.c btree.c -o $(TARGET) -I .

clean:
	-$(RM) $(TARGET)

test:
	ruby test.rb 
