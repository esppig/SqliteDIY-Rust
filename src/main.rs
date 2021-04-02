mod repl;

use repl::looper;

fn main() {
    looper().unwrap();
}
