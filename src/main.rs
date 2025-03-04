use std::io::{self, Read};
use std::fs::File;

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

fn main() -> io::Result<()> {
    let mut file = File::open("hexcat.1")?;

    let mut buf = [0u8; 16];
    let mut offset = 0;

    loop {
        let bytes_read = file.read(&mut buf)?;
        if bytes_read == 0 {
            break;
        }

        print!("{:08x}: ", offset);

        for &byte in &buf[..bytes_read] {
            print!("\x1b[1;3{}m{:02x} \x1b[0m", colorize(byte), byte);
        }

        for _ in bytes_read..16 {
            print!("   ");
        }

        print!(" | ");
        for &byte in &buf[..bytes_read] {
            print!("\x1b[1;3{}m", colorize(byte));
            if byte.is_ascii() && !byte.is_ascii_control() {
                print!("{}", byte as char);
            } else {
                print!(".");
            }
            print!("\x1b[0m");
        }
        println!();

        offset += bytes_read;
    }

    Ok(())
}
