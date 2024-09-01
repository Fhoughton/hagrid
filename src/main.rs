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
        mode: String
    },
    Pack {
        mode: String
    }
}


#[derive(Debug, Clone)]
enum FileMode {
    Extract,
    Pack,
}

fn main() {
    let args = Args::parse();

    match args.cmd {
        Commands::Extract{mode} => println!("Extract!"),
        Commands::Pack{mode} => println!("Pack!")
    }
}
