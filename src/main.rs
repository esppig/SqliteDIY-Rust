mod repl;
mod table;

use repl::looper;

fn main() {
    looper().unwrap();
}
