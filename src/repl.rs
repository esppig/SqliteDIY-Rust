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
            PrepareState::SUCCESS => {
                execute_stmt(&input_buf);
                println!("Executed!");
                continue;
            }
            PrepareState::UNRECOGNIZED => {
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

fn prepare_stmt(stm: &str) -> PrepareState {
    if stm == "query;" {
        return PrepareState::SUCCESS;
    }
    PrepareState::UNRECOGNIZED
}

fn execute_stmt(stm: &str) {}
