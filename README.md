## Random-Bit-Steganography

**What is it?**

The goal of this project is simple: to create a method of steganography that hides data differently in a file for any given key. Most steganography implementations have no built in entropy (randomness) and each time the steganography algorithm is ran on the same data, the data will appear in the same parts of the file.

This is where Random Bit Steganography comes in. R.B.S hides each bit of data at a location within the file that is random, while not effecting the quality of the carrier to much, wether it be an image, an audio file, etc. This is very beneficial as steganylsis software could no longer retrieve the data purely by reversing the algorithm. 

**Dependencies**

The following dependencies are required:

* [mpg123](http://www.mpg123.de/)
* [LAME (libmp3lame)](https://github.com/VFR-maniac/lame)

**Installation**

To install just clone the git repository or download the zip repository.
Then run the makefile, which will compile the program.

```
  cd /projecthead
  make
```

**Usage**

Example output of ./rb-stegano -help:

```
usage: random-bit stegano [-t filetype] -m encryption | decryption -k encryptionkey -i inputfile [< | > datastream]
	-m mode: encryption or decryption
	-k encryptionkey: 3 32-bit unsigned integers as an encryption key
		example: -k 3000 4294967296 230213
	-t filetype: the type of file of the inputfile
		supported file types: gif png jpeg jpg mp3 (attempts to detect filetype based off extension if not specified)
	-i inputfile: the file the data will be hidden inside of if encyp
	datastream
		during encryption a pipe containing the data to encrypt
		during decryption a pipe to the output of the decryption

typical usage example:
	encrypt data from inputdata.txt in png test.png: random-bit stegano -m encryption -k 112323 109 12 -i test.png < inputdata.txt
	decrypt data from test.png and put output in output.txt: random-bit stegano -m decryption -k 112323 109 12 -i test.png > output.txt
```

More usage documentation will be coming soon.

**Developement**

Currently the project is being developed solely by one person, myself. Currently I just finished PNG support and I am going to work on other image formats.

Progress:

* Random Number Generator Implementation (Complete)
* Usage Information (Complete)
* PNG file support (Complete)
* MP3 file support (Not currently working)
* JPG/JPEG file support (Not started)
* GIF file support (Not started)
* More to come
