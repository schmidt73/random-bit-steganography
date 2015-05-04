## Random-Bit-Steganography

**What is it?**

The idea of this program is simple: to combine steganography and encryption together, thus creating an advanced and powerful steganography implementation. Most steganography implementations have no built in encryption and putting your unencrypted data inside of them is really dangerous if the intercepter realizes that you are hiding data inside of the carrier. A simple workaround would be to encrypt your data beforehand and then use steganography to hide your data inside of the carrier, but then it can still be obvious that you are sending encrypted data.

That is where R.B.S. comes in. The goal behind this project is to create an advanced steganography implementation that can take raw data and safely hide it inside of a carrer, even when the carrier is known to hidden data inside of it. Random Bit Steganography accomplishes this by using a 96-bit key which is used to generate a unique sequence of random bits that is used to hide data. Random Bit Steganography hides each bit of data at a location within the file that is random, while not effecting the quality of the carrier to much, wether it be an image, an audio file, etc.

**Installation**

To install just clone the git repository or download the zip repository.
Then run the makefile, which will compile the program.

```
  cd /projecthead
  make
```

**Usage**

Output of ./rb-stegano -help:

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

Currently the project is being developed solely by one person, myself. Currently I am not very far and am currently working on steganography through PNG files.

Progress:

* Random Number Generator Implementation (Complete)
* Usage Information (Complete for now)
* PNG file support (Working)
* MP3 file support (Not started)
* JPG/JPEG file support (Not started)
* GIF file support (Not started)
* More to come
