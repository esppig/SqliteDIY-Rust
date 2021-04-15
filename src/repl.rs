//! Sqlite - REPL
//! main loop

use std::{
    io::{self, Write},
    process::exit,
};

use crate::constants::*;
use crate::table::{Row, Table};

enum MetaState {
    SUCCESS,
    UNRECOGNIZED,
}

enum PrepareState {
    SUCCESS,
    UNRECOGNIZED,
    SyntaxError,
}

enum ExecuteResult {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL,
}

#[derive(Debug)]
enum StatementType<'a> {
    INSERT(&'a str),
    SELECT(&'a str),
    UNKNOWN(&'a str),
    // Empty,
}

#[derive(Debug)]
struct Statement<'a> {
    stm_type: StatementType<'a>,
    row: Row,
}

impl<'a> Statement<'a> {
    fn new(stm_type: StatementType<'a>) -> Statement<'a> {
        Statement {
            // stm_type: StatementType::Empty,
            stm_type,
            row: Row::new(),
        }
    }
}

pub fn looper() -> io::Result<()> {
    let mut table = Table::new();
    let mut input_buf = String::new();
    loop {
        print!("sqlite > ");
        io::stdout().flush().unwrap();
        input_buf.clear();
        let _buf_len = io::stdin().read_line(&mut input_buf)?;
        input_buf.pop(); // pop "\n"
        if input_buf.starts_with(".") {
            match do_meta_cmd(&input_buf) {
                MetaState::SUCCESS => {
                    continue;
                }
                MetaState::UNRECOGNIZED => {
                    println!("无法识别的命令: {}", &input_buf);
                    continue;
                }
            }
        }
        match check_stmt(&input_buf) {
            (PrepareState::SUCCESS, stm_type) => {
                let mut stmt = Statement::new(stm_type);
                execute_stmt(&mut stmt, &mut table);
                println!("Executed!");
                continue;
            }
            (PrepareState::SyntaxError, _) => {
                println!("语法错误: {}", &input_buf);
                continue;
            }
            (PrepareState::UNRECOGNIZED, _) => {
                println!("无法识别的语句: {}", &input_buf);
                continue;
            }
        }
    }
    // Ok(())
}

fn do_meta_cmd(cmd: &str) -> MetaState {
    if cmd == ".exit" || cmd == ".q" {
        exit(0)
    }
    if cmd == ".tables" {
        return MetaState::SUCCESS;
    }
    MetaState::UNRECOGNIZED
}

// fn prepare_stmt(stm_type: StatementType) -> Statement {
//     Statement::new(stm_type)
// }

fn check_stmt(buf: &str) -> (PrepareState, StatementType) {
    if buf.starts_with("insert") {
        let tokens = buf.split_whitespace().collect::<Vec<&str>>();
        if tokens.len() != 4 {
            return (PrepareState::SyntaxError, StatementType::INSERT(buf));
        }
        return (PrepareState::SUCCESS, StatementType::INSERT(buf));
    }
    if buf.starts_with("select") {
        return (PrepareState::SUCCESS, StatementType::SELECT(buf));
    }
    (PrepareState::UNRECOGNIZED, StatementType::UNKNOWN(buf))
}

fn execute_stmt(stmt: &mut Statement, table: &mut Table) {
    // println!("stmt: {:?}", stmt);

    match stmt.stm_type {
        StatementType::INSERT(s) => {
            // println!("insert stm: {}", s);
            prepare_insert(&mut stmt.row, s);
            execute_insert(&stmt.row, table);
        }
        StatementType::SELECT(s) => {
            // println!("select stm: {}", &s);
            execute_select(table);
        }
        StatementType::UNKNOWN(_) => {}
    }
}

fn prepare_insert(row: &mut Row, input: &str) {
    let tokens = input.split_whitespace().collect::<Vec<&str>>();
    if tokens.len() > 4 {
        println!("Expect 4 string, example: insert 1 xiaoming xiaoming@sina.com");
        exit(1);
    }
    row.id = tokens[1].to_string().parse::<u32>().unwrap();

    let name_bytes = tokens[2].as_bytes();
    let namelen = name_bytes.len();
    if namelen > USERNAME_SIZE {
        println!("Username is too long!");
        exit(1);
    }
    row.name[0..namelen].copy_from_slice(name_bytes);

    let email_bytes = tokens[3].as_bytes();
    let emaillen = email_bytes.len();
    if emaillen > EMAIL_SIZE {
        println!("Email is too long!");
        exit(1);
    }
    row.email[0..emaillen].copy_from_slice(email_bytes);
}

fn execute_insert(row: &Row, table: &mut Table) -> ExecuteResult {
    if table.rows_count > TABLE_MAX_ROWS as u32 {
        return ExecuteResult::EXECUTE_TABLE_FULL;
    }
    let (page_num, byte_offsets) = table.row_slot(table.rows_count);
    table.serialize_row(&row, page_num, byte_offsets);
    table.rows_count += 1;
    ExecuteResult::EXECUTE_SUCCESS
}

fn execute_select(table: &mut Table) -> ExecuteResult {
    for i in 0..table.rows_count {
        let (page_num, byte_offsets) = table.row_slot(i);
        &table.deserialize_row(page_num, byte_offsets).show();
    }
    ExecuteResult::EXECUTE_SUCCESS
}
