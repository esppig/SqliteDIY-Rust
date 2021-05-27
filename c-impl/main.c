#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h> // ssize_t
#include <string.h>  // strcmp

#include "table.h"

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

// 元命令的执行结果
typedef enum {
    META_COMMAND_SUCCESS,  // 元命令执行成功
    META_COMMAND_UNRECOGNIZED // 无法识别的元命令
} MetaCommandResult;

// Prepare语句的执行结果
typedef enum {
    PREPARE_SUCCESS,  // prepare语句成功
    PREPARE_UNRECOGNIZED, // 无法识别的prepare语句
    PREPARE_SYNTAX_ERROR // 语法错误
} PrepareResult ;

// 语句类型
typedef enum {
    STATEMENT_SELECT,
    STATEMENT_INSERT
} StatementType;

// 语句
typedef struct {
    StatementType stm_type;
    Row row_to_insert; // 插入行
} Statement;

// 处理元命令
MetaCommandResult do_meta_command(InputBuffer* ib) {
    if (strcmp(ib->buffer, ".exit") == 0) {
        close_input_buffer(ib);
        exit(EXIT_SUCCESS);
        // return META_COMMAND_SUCCESS;
    }
    return META_COMMAND_UNRECOGNIZED;
}

// prepare语句
PrepareResult prepare_statement(InputBuffer* ib, Statement* stm) {
    // 比较前 n 个字符
    if (strncmp(ib->buffer, "insert", 6) == 0) {
        stm->stm_type = STATEMENT_INSERT;
        int args_assigned = sscanf(ib->buffer, "insert %d %s %s", 
            &(stm->row_to_insert.id), stm->row_to_insert.username, stm->row_to_insert.email);
        if (args_assigned < 3) {
            return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;
    }

    if (strncmp(ib->buffer, "select", 6) == 0) {
        stm->stm_type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED;
}

typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
} ExecuteResult;

// 执行插入语句
ExecuteResult execute_insert(Statement* stm, Table* table) {
    if (table->num_rows > TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }
    Row* row = &(stm->row_to_insert);
    serialize_row(row, row_slot(table, table->num_rows));
    table->num_rows += 1;
    return EXECUTE_SUCCESS;
}

// 执行查询语句
ExecuteResult execute_select(Statement* stm, Table* table) {
    Row row;
    for (uint32_t i = 0; i < table->num_rows; i++) {
        deserialize_row(row_slot(table, i), &row);
        print_row(&row);
    }
    return EXECUTE_SUCCESS;
}

// 执行语句
ExecuteResult execute_statement(Statement* stm, Table* table) {
    switch (stm->stm_type) {
    case (STATEMENT_INSERT):
      return execute_insert(stm, table);
    case (STATEMENT_SELECT):
      return execute_select(stm, table);
  }
}

int main(int argc, char* argv[]) {
    // 创建一张表
    Table *table = new_table();
    // 新建输入buf
    InputBuffer * ib = new_input_buffer();
    while (true) {
        print_prompt();
        read_input(ib);

        if (ib->buffer[0] == '.') {
            switch (do_meta_command(ib))
            {
            case META_COMMAND_SUCCESS:
                continue;
            case META_COMMAND_UNRECOGNIZED:
                printf("Unrecognized command '%s'\n", ib->buffer);
                continue;
            default:
                break;
            }
        }

        Statement stm;
        switch (prepare_statement(ib, &stm))
        {
        case PREPARE_SUCCESS:
            break;
        case PREPARE_SYNTAX_ERROR:
            printf("Syntax error. Could not parse statement.\n");
            continue;
        case PREPARE_UNRECOGNIZED:
            printf("Unrecognized keyword at start of '%s'.\n", ib->buffer);
            continue;
        default:
            break;
        }

        switch (execute_statement(&stm, table))
        {
        case EXECUTE_SUCCESS:
            printf("Executed.\n");
            break;
        case EXECUTE_TABLE_FULL:
            printf("Error: Table full.\n");
            break;
        default:
            break;
        }
    }
}
