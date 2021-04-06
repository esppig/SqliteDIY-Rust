//! Sqlite - REPL
//! main loop

use std::{
    io::{self, Write},
    process::exit,
};

enum MetaState {
    SUCCESS,
    UNRECOGNIZED,
}

enum PrepareState {
    SUCCESS,
    UNRECOGNIZED,
}

enum Statement<'a> {
    INSERT(&'a str),
    SELECT(&'a str),
    UNKNOWN(&'a str),
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
        match prepare_stmt(&input_buf) {
            (PrepareState::SUCCESS, stm) => {
                execute_stmt(stm);
                println!("Executed!");
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

fn prepare_stmt(stm: &str) -> (PrepareState, Statement) {
    if stm.starts_with("insert") {
        return (PrepareState::SUCCESS, Statement::INSERT(stm));
    }
    if stm.starts_with("select") {
        return (PrepareState::SUCCESS, Statement::SELECT(stm));
    }
    (PrepareState::UNRECOGNIZED, Statement::UNKNOWN(stm))
}

fn execute_stmt(stm: Statement) {
    match stm {
        Statement::INSERT(s) => {
            println!("insert stm: {}", &s);
        }
        Statement::SELECT(s) => {
            println!("select stm: {}", &s);
        }
        Statement::UNKNOWN(_) => {}
    }
}
