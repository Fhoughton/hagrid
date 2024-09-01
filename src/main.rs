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

fn datdir_pack(dat_file: String, dir_file: String, in_path: String) {
    let mut files: Vec<_> = fs::read_dir(&in_path).unwrap()
                                              .map(|r| r.unwrap())
                                              .collect();
    files.sort_by_key(|dir| dir.path());

    let files_count = files.len() as u32;

    println!("Packing {} files", files_count);

    let mut dir_data: Vec<u8> = vec![];

    // Repack dir file
    dir_data.extend(files_count.to_le_bytes()); // Write file count first

    let mut dir_offset : u32 = 0;

    // Then for each file write the name as 12 bytes, size as 4 bytes and offset in the dat file as 4 bytes
    for entry in &files {
        let path = entry.path();

        if path.is_file() {
            let metadata = fs::metadata(&path).unwrap();
            let file_size = metadata.len() as u32;
            let file_name = path.file_name().unwrap().to_string_lossy(); // Have to format to fill it to 12 bytes with null characters

            // First write 12 byte filename
            dir_data.extend(file_name.as_bytes());

            for _ in 0..12-file_name.len() {
                dir_data.push(0x00); // Pad with null terminated bytes
            }


            // Then write 4 byte file size
            dir_data.extend(file_size.to_le_bytes());

            // Then 4 bytes file offset
            dir_data.extend(dir_offset.to_le_bytes());

            // Increment the offset
            dir_offset += file_size; // Move the offsets in the file each time
        }
    }

    // Then write the dat data
    let mut dat_data: Vec<u8> = vec![];

    for entry in &files {
        let path = entry.path();

        if path.is_file() {
            let file_contents = std::fs::read(path).unwrap();

            dat_data.extend(file_contents);
        }
    }

    // Now write the files to their destinations
    fs::write(dat_file, &dat_data).unwrap();
    fs::write(dir_file, &dir_data).unwrap();
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
                    datdir_pack(dat_file, dir_file, path);
                }
            }
        }
    }
}
