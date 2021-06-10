#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h> // ssize_t
#include <string.h>  // strcmp

#include "cursor.h"
#include "btree.h"

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
    printf("db > ");
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
    PREPARE_SYNTAX_ERROR, // 语法错误
    PREPARE_STRING_TOO_LONG, // 字段超过最大长度
    PREPARE_NEGATIVE_ID // ID数值为负数
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

// 打印常量
void print_constants() {
    printf("ROW_SIZE: %lu\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
    printf("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
    printf("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}

// Btree打印，方便调试
// void print_leaf_node(void* node) {
//     uint32_t num_cells = *leaf_node_num_cells(node);
//     printf("leaf (size %d)\n", num_cells);
//     for (uint32_t i = 0; i < num_cells; i++) {
//         uint32_t key = *leaf_node_key(node, i);
//         printf("  - %d : %d\n", i, key);
//     }
// }

void indent(uint32_t level) {
    for (uint32_t i = 0; i < level; i++) {
        printf("  ");
    }
}

// BplusTree 多级树 调试打印
void print_tree(Pager* pager, uint32_t page_num, uint32_t indentation_level) {
    // 获取页码对应的节点
    void* node = get_page(pager, page_num);
    uint32_t keys_count, child;

    switch (get_node_type(node))
    {
    case NODE_LEAF:
        // 获取key数量
        keys_count = *leaf_node_num_cells(node);
        indent(indentation_level);
        printf("- leaf (size %d)\n", keys_count);
        for (uint32_t i = 0; i < keys_count; i++) {
            indent(indentation_level + 1);
            printf("- %d\n", *leaf_node_key(node, i));
        }
        break;
    case NODE_INTERNAL:
        keys_count = *internal_node_num_keys(node);
        indent(indentation_level);
        printf("- internal (size %d)\n", keys_count);
        for (uint32_t i = 0; i < keys_count; i++) {
            child = *internal_node_child(node, i);
            // 递归调用
            print_tree(pager, child, indentation_level + 1);

            indent(indentation_level + 1);
            printf("- key %d\n", *internal_node_key(node, i));
        }
        child = *internal_node_right_child(node);
        print_tree(pager, child, indentation_level + 1);
        break;
    }
}

// 处理元命令
MetaCommandResult do_meta_command(InputBuffer* ib, Table* table) {
    if (strcmp(ib->buffer, ".exit") == 0) {
        close_input_buffer(ib);
        db_close(table);
        exit(EXIT_SUCCESS);
        // return META_COMMAND_SUCCESS;
    }
    if (strcmp(ib->buffer, ".constants") == 0) {
        printf("Constants:\n");
        print_constants();
        return META_COMMAND_SUCCESS;
    }
    // Btree 打印
    if (strcmp(ib->buffer, ".btree") == 0) {
        printf("Tree:\n");
        // print_leaf_node(get_page(table->pager, 0));
        print_tree(table->pager, 0, 0);
        return META_COMMAND_SUCCESS;
    }
    return META_COMMAND_UNRECOGNIZED;
}

PrepareResult prepare_insert(InputBuffer* ib, Statement* stm) {
    stm->stm_type = STATEMENT_INSERT;

    // 以 " " 分割字符串, strtok(NULL, " ") 逐个获取分割后的子字符串
    strtok(ib->buffer, " ");
    char* id_string = strtok(NULL, " ");
    char* username = strtok(NULL, " ");
    char* email = strtok(NULL, " ");

    if (id_string == NULL || username == NULL || email == NULL) {
        return PREPARE_SYNTAX_ERROR;
    }

    int id = atoi(id_string);
    if (id < 0) {
        return PREPARE_NEGATIVE_ID;
    }
    if (strlen(username) > COLUMN_USERNAME_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }
    if (strlen(email) > COLUMN_EMAIL_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    stm->row_to_insert.id = id;
    strcpy(stm->row_to_insert.username, username);
    strcpy(stm->row_to_insert.email, email);

    return PREPARE_SUCCESS;
}

// prepare语句
PrepareResult prepare_statement(InputBuffer* ib, Statement* stm) {
    // 比较前 n 个字符
    if (strncmp(ib->buffer, "insert", 6) == 0) {
        // stm->stm_type = STATEMENT_INSERT;
        // * scanf 缺陷：如果输入大于缓冲区，会造成 缓冲区溢出
        // int args_assigned = sscanf(ib->buffer, "insert %d %s %s",
        //     &(stm->row_to_insert.id), stm->row_to_insert.username, stm->row_to_insert.email);
        // if (args_assigned < 3) {
        //     return PREPARE_SYNTAX_ERROR;
        // }
        // return PREPARE_SUCCESS;
        return prepare_insert(ib, stm);
    }

    if (strncmp(ib->buffer, "select", 6) == 0) {
        stm->stm_type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED;
}

typedef enum {
    EXECUTE_SUCCESS,  // 执行成功
    EXECUTE_TABLE_FULL,  // 表空间满了
    EXECUTE_DUPLICATE_KEY // 键重复
} ExecuteResult;


// 执行插入语句, 使用光标
// 改进插入方法，搜索正确的插入位置[而不是直接加到末尾]
ExecuteResult execute_insert(Statement* stm, Table* table) {
    // -- 目前采用单节点[单页]
    void* node = get_page(table->pager, table->root_page_no);
    uint32_t num_cells = *leaf_node_num_cells(node);

    //if (num_cells >= LEAF_NODE_MAX_CELLS) {
    //   return EXECUTE_TABLE_FULL;
    //}

    // 要插入的行记录
    Row* row = &(stm->row_to_insert);

    // 要插入的cell键
    uint32_t key_to_insert = row->id;

    // 返回给定cell键在树中的位置光标
    Cursor* cursor = table_find(table, key_to_insert);
    
    if (cursor->cell_no < num_cells) {
        uint32_t key_at_index = *leaf_node_key(node, cursor->cell_no);
        if (key_at_index == key_to_insert) {
            return EXECUTE_DUPLICATE_KEY;
        }
    }
    // 在Btree节点中插入row
    leaf_node_insert(cursor, row->id, row);

    // 释放光标
    free(cursor);
    return EXECUTE_SUCCESS;
}

// 执行查询语句, 使用光标
ExecuteResult execute_select(Statement* stm, Table* table) {
    // @ 获取表头光标
    Cursor* cursor = table_start(table);
    Row row;
    // for (uint32_t i = 0; i < table->num_rows; i++) {
    //     deserialize_row(row_slot(table, i), &row);
    //     print_row(&row);
    // }
    while (!(cursor->end_of_table)) {
        // 未达到表尾
        deserialize_row(cursor_value(cursor), &row);
        print_row(&row);
        cursor_advance(cursor);
    }
    // 释放光标
    free(cursor);
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
    if (argc < 2) {
        printf("Must supply a database filename.\n");
        exit(EXIT_FAILURE);
    }

    char* filename = argv[1];
    Table* table = db_open(filename);

    // 新建输入buf
    InputBuffer * ib = new_input_buffer();
    while (true) {
        print_prompt();
        read_input(ib);

        if (ib->buffer[0] == '.') {
            switch (do_meta_command(ib, table))
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
        case PREPARE_STRING_TOO_LONG:
            printf("String is too long.\n");
            continue;
        case PREPARE_NEGATIVE_ID:
            printf("ID must be positive.\n");
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
        case EXECUTE_DUPLICATE_KEY:
            printf("Error: Duplicate key.\n");
            break;
        default:
            break;
        }
    }
}
