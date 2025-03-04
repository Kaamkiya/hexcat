use std::io::{self, Read};
use std::fs::File;

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
            print!("{:02x} ", byte);
        }

        for _ in bytes_read..16 {
            print!("   ");
        }

        print!(" | ");
        for &byte in &buf[..bytes_read] {
            if byte.is_ascii() && !byte.is_ascii_control() {
                print!("{}", byte as char);
            } else {
                print!(".");
            }
        }
        println!();

        offset += bytes_read;
    }

    Ok(())
}
