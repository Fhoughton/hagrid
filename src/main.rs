use clap::{builder::StringValueParser, Parser, Subcommand, ValueEnum};
use std::fs::read;
use std::str;

struct DirFileEntry {
    name: String,
    filesize: u32,
    offset: u32
}

fn datdir_extract(dat_file: String, dir_file: String, out_path: String) {
    let dir_contents = std::fs::read(dir_file).unwrap();
    let file_count = u32::from_le_bytes(dir_contents[0..4].try_into().unwrap());

    let dat_contents = std::fs::read(dat_file).unwrap();

    // Read each file entry from the dat file
    // To do so we read the dir for the file data
    // (name, pointer, size) and then use these to
    // Extract the file from the .dat
    for i in 0..file_count {
        let dir_offset : usize = 4 + (i * 20) as usize; // entry Each is 20 bytes long
        let filename_bytes = &dir_contents[dir_offset..dir_offset+12];
        println!("{:?}", filename_bytes);

        let filename_string = match str::from_utf8(filename_bytes) {
            Ok(v) => v,
            Err(e) => panic!("Invalid UTF-8 sequence: {}", e),
        };

        println!("{}", filename_string);
    }
}

#[derive(Parser)]
#[command(name = "hagrid", about = "A file extractor for the PS1 Harry Potter games")]
struct Args {
    #[command(subcommand)]
    cmd: Commands
}

#[derive(Subcommand, Debug, Clone)]
enum Commands {
    DatDir {
        #[clap(short, long, value_enum)]
        mode: FileMode,
        #[clap(long)]
        dat_file: String,
        #[clap(long)]
        dir_file: String,
        #[clap(long)]
        path: String
    },
}

#[derive(ValueEnum, Debug, Clone)]
enum FileMode {
    Extract,
    Pack,
}

fn main() {
    let args = Args::parse();

    match args.cmd {
        Commands::DatDir{mode, dat_file, dir_file, path} => {
            match mode {
                // e.g. hagrid dat-dir --mode extract --dat-file potter01_extracted/POTTER.DAT --dir-file potter01_extracted/POTTER.DIR --path out
                FileMode::Extract => {
                    datdir_extract(dat_file, dir_file, path);
                }

                FileMode::Pack => {

                }
            }
        }
    }
}
