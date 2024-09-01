use clap::{Parser, Subcommand, ValueEnum};
use std::str;
use std::path::Path;
use std::fs;

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

        // 12 byte filename
        let filename_bytes = &dir_contents[dir_offset..dir_offset+12];

        let filename_string = match str::from_utf8(filename_bytes) {
            Ok(v) => v.trim_matches(char::from(0)), // Trims the trailing null terminators used to pad to 12 bytes
            Err(e) => panic!("Invalid UTF-8 sequence: {}", e),
        };

        // 4 byte filesize (in bytes)
        let filesize_bytes = &dir_contents[dir_offset+12..dir_offset+16];
        let filesize = u32::from_le_bytes(filesize_bytes.try_into().unwrap()) as usize;

        // 4 byte file offset
        let offset_bytes = &dir_contents[dir_offset+16..dir_offset+20];
        let dat_offset = u32::from_le_bytes(offset_bytes.try_into().unwrap()) as usize;

        println!("Name: {}, Size: {}, Offset: {}", filename_string, filesize, dat_offset);

        // Resolve the output file path and write the data to the file
        let out_file_path = Path::new(&out_path).join(filename_string);
        fs::write(out_file_path, &dat_contents[dat_offset..dat_offset+filesize]).unwrap();
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
