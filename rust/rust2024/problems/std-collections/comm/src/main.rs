#![forbid(unsafe_code)]

use std::{fs::File, io::BufRead, io::BufReader};

fn file_reader(path: &String) -> BufReader<File> {
    let file = File::open(path).unwrap();
    BufReader::new(file)
}

fn main() {
    let args = std::env::args().collect::<Vec<String>>();

    if args.len() != 3 {
        panic!("Not enough arguments");
    }

    let mut lines = std::collections::HashSet::new();
    for line in file_reader(&args[1]).lines() {
        lines.insert(line.expect("Can not read lines from file"));
    }

    for line in file_reader(&args[2]).lines() {
        if let Some(val) = lines.take(&line.expect("Can not read lines from file")) {
            println!("{}", val);
        }
    }
}
