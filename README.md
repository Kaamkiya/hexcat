# hexcat

An extremely small hexdump tool.

## Screenshots

[![The command showing its own binary](assets/demo.png)](assets/demo.png)

## Usage

First, clone the repo:

```bash
$ git clone https://codeberg.org/Kaamkiya/hexcat
```

Then build from source:

```bash
$ cd hexcat
$ make
```

If you want to install it system-wide:

```
$ sudo make install
```

Help:

```
$ hexcat -h
```

If you don't specify a file to read from, hexcat will read from stdin, so you
can also pipe to it:

```
$ cat some-file | hexcat
```

It will write the data to stdout, or you can specify a file using redirection:

```
$ echo "Let's go fly a kite" | hexcat > hello.txt
```

Try `hexcat -h` for all available flags.

## License

This project is licensed under the MIT license.
