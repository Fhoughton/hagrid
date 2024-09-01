use clap::{ValueEnum, Parser, Subcommand};

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
                FileMode::Extract => {

                }

                FileMode::Pack => {

                }
            }
        }
    }
}
