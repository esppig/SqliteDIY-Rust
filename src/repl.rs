//! Sqlite - REPL
//! main loop

use std::{
    io::{self, Write},
    process::exit,
};

use crate::table::Row;

enum MetaState {
    SUCCESS,
    UNRECOGNIZED,
}

enum PrepareState {
    SUCCESS,
    UNRECOGNIZED,
    SyntaxError,
}

enum StatementType<'a> {
    INSERT(&'a str),
    SELECT(&'a str),
    UNKNOWN(&'a str),
    // Empty,
}

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
                let stmt = Statement::new(stm_type);
                execute_stmt(stmt);
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

fn prepare_stmt(stm_type: StatementType) -> Statement {
    Statement::new(stm_type)
}

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

fn execute_stmt(stmt: Statement) {
    match stmt.stm_type {
        StatementType::INSERT(s) => {
            println!("insert stm: {}", &s);
        }
        StatementType::SELECT(s) => {
            println!("select stm: {}", &s);
        }
        StatementType::UNKNOWN(_) => {}
    }
}
