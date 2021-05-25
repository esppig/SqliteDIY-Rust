#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h> // ssize_t
#include <string.h>  // strcmp

// 输入缓冲
typedef struct {
    char * buffer;  // 输入字符串
    size_t buffer_length; // 缓冲大小
    ssize_t input_length;  // 输入字符串长度
} InputBuffer;

// new InputBuffer && init && return
InputBuffer* new_input_buffer() {
    InputBuffer* ib = (InputBuffer*)malloc(sizeof(InputBuffer));
    ib->buffer = NULL;
    ib->buffer_length = 0;
    ib->input_length = 0;
    return ib;
}

// 关闭 InputBuffer
void close_input_buffer(InputBuffer* ib) {
    free(ib->buffer);  // 释放buffer占用的内存
    free(ib);  // 释放 InputBuffer 占用的内存
}

// 输入的提示符
void print_prompt() {
    printf("db> ");
}

// 从标准输入读取到 InputBuffer
void read_input(InputBuffer* ib) {
    // ssize_t getline(char **lineptr, size_t *n, FILE *stream);
    ssize_t bytes_read = getline(&(ib->buffer), &(ib->buffer_length), stdin);
    if (bytes_read < 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    ib->input_length = bytes_read - 1;
    ib->buffer[bytes_read-1] = '\0';
}


int main(int argc, char* argv[]) {
    InputBuffer * ib = new_input_buffer();
    while (true) {
        print_prompt();
        read_input(ib);

        if (strcmp(ib->buffer, ".exit") == 0) {
            close_input_buffer(ib);
            exit(EXIT_SUCCESS);
        } else {
            printf("Unrecoginzed command '%s'. \n", ib->buffer);
        }
    }
}
