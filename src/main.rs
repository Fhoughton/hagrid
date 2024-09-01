use std::process;

#[derive(Debug)]
enum FileMode {
    Extract,
    Pack,
}

fn get_arguments() {
    let args: Vec<_> = std::env::args().collect(); // get all arguments passed to app
    println!("{:?}", args);
}

fn main() {
    get_arguments();
    println!("Hello, world!");
}
