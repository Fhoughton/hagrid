use clap::{Parser, Subcommand};

#[derive(Parser)]
#[command(author, version, about, long_about = None)]
struct Args {
    #[command(subcommand)]
    cmd: Commands
}

#[derive(Subcommand, Debug, Clone)]
enum Commands {
    Extract {
    },
    Pack {
    }
}


#[derive(Debug)]
enum FileMode {
    Extract,
    Pack,
}

fn main() {
    let args = Args::parse();

    match args.cmd {
        Commands::Extract{} => println!("Extract!"),
        Commands::Pack{} => println!("Pack!")
    }
}
