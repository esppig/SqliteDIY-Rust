mod constants;
mod repl;
mod table;

use repl::looper;

fn main() {
    let args: Vec<String> = std::env::args().collect();

    if args.len() < 2 {
        println!("Must supply database filename.");
        println!("cargo run <filename>");
        std::process::exit(1);
    }

    let filename = &args[1];

    looper(filename).unwrap();
}
