use std::fs::File;
use std::io::{self, IsTerminal, Read, Write};
use std::process::exit;

fn colorize(byte: u8) -> i32 {
    if byte.is_ascii_whitespace() {
        return 3;
    }
    if byte == 0 {
        return 7;
    }
    if byte == 255 {
        return 4;
    }
    if byte.is_ascii() && !byte.is_ascii_control() {
        return 2;
    }

    1
}

fn usage() {
    eprintln!("usage: hexcat [-hvncxu] [-o FILE] [FILE]");
    exit(1);
}

fn main() -> io::Result<()> {
    let args: Vec<String> = std::env::args().collect();

    let mut filename = "";
    let mut outfilename = "";
    let mut use_color = 1;
    let mut show_ascii = true;
    let mut uppercase = false;

    let mut skip = false;

    for i in 1..args.len() {
        if skip {
            skip = false;
            continue;
        }
        match args.get(i).expect("Failed to iterate arguments.").as_str() {
            "-h" => usage(),
            "-n" => use_color = 0,
            "-c" => use_color = 2,
            "-x" => show_ascii = false,
            "-u" => uppercase = true,
            "-v" => {
                println!("hexcat v0.1.1");
                exit(0);
            }
            "-o" => {
                outfilename = args.get(i + 1).unwrap().as_str();
                skip = true;
            }
            _ => filename = args.get(i).expect("Failed to get argument").as_str(),
        }
    }

    if filename == "" {
        usage();
    }

    let outfile: &mut dyn Write = match outfilename {
        "" => &mut io::stdout(),
        _ => &mut File::create(outfilename)?,
    };

    if (!io::stdout().is_terminal() && use_color != 2) || (outfilename != "" && use_color != 2) {
        use_color = 0;
    }

    let mut file = File::open(filename)?;

    let mut buf = [0u8; 16];
    let mut offset = 0;

    loop {
        let bytes_read = file.read(&mut buf)?;
        if bytes_read == 0 {
            break;
        }

        if uppercase {
            write!(outfile, "{:08X}: ", offset)?;
        } else {
            write!(outfile, "{:08x}: ", offset)?;
        }

        for &byte in &buf[..bytes_read] {
            if use_color == 0 {
                if uppercase {
                    write!(outfile, "{:02X} ", byte)?;
                } else {
                    write!(outfile, "{:02x} ", byte)?;
                }
            } else {
                if uppercase {
                    write!(outfile, "\x1b[1;3{}m{:02X} \x1b[0m", colorize(byte), byte)?;
                } else {
                    write!(outfile, "\x1b[1;3{}m{:02x} \x1b[0m", colorize(byte), byte)?;
                }
            }
        }

        for _ in bytes_read..16 {
            write!(outfile, "   ")?;
        }

        if show_ascii {
            write!(outfile, " | ")?;
            for &byte in &buf[..bytes_read] {
                if use_color != 0 {
                    write!(outfile, "\x1b[1;3{}m", colorize(byte))?;
                }
                if byte.is_ascii() && !byte.is_ascii_control() {
                    write!(outfile, "{}", byte as char)?;
                } else {
                    write!(outfile, ".")?;
                }
                if use_color != 0 {
                    write!(outfile, "\x1b[0m")?;
                }
            }
        }

        write!(outfile, "\n")?;

        offset += bytes_read;
    }

    Ok(())
}
