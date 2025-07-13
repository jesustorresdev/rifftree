# A console application to show the tree structure of a RIFF file.

This is a Qt5/Qt6 console application showing the structure of a [RIFF file](https://en.wikipedia.org/wiki/Resource_Interchange_File_Format).
Instead of reading and parsing the file (which may be quite large), it is [memory mapped](https://en.wikipedia.org/wiki/Memory-mapped_file) and should be very efficient.

## Usage

```
    $ ./rifftree -h
    Usage: ./rifftree [options] file
    
    Options:
      -h, --help     Displays help on commandline options.
      --help-all     Displays help including Qt specific options.
      -v, --version  Displays version information.
    
    Arguments:
      file           RIFF file
```

## Sample Output

```
    $ ./rifftree WIND2.wav
                  0 RIFF(WAVE) -> size=156582
                 12    fmt  size=16
                 36    data size=156546
```

## License

Copyright (C) 2013 Jesús Torres <jmtorres@ull.es>  
Copyright (C) 2025 Pedro López-Cabanillas <plcl@users.sf.net>  
SPDX-License-Identifier: Apache-2.0
