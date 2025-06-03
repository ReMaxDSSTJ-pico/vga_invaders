#!/usr/bin/env python

##@file wav2hex.py
#  @ingroup util
#	A script for converting PCM 8-bit unsinged 22050 Khz audio to HEX for use in Raspberry Pico PCM sound.
#
#	The PCM format is well publicized. The PCM file is simple. It starts with a HEADER followed by the DATA
#   section. There are several audio formats, but we need the file to be already in 8-bit unsigned 22050Khz samples.
#   See below for more
#
#	Usage: 
#	>>>wav2hex.py [-v] [-n] [-h <bytes>] [-b <size-bytes>] <infile> <tablename>
#	
#	@param infile		The file to convert.
#	@param tablename	The name of the table to create
#	@param n			The name of the named structure
#	@param tablewidth	"-h <bytes>, The number of bytes to skip at the beggining of the file and start converting data without check.
#	@param version		"-v", returns version number
#	
#	@author Pedro Farías 2023-06
#
#	Author:    Pedro Farias (tjactivo@hotmail.com)
#	License:   MIT Opensource License
#
#	Copyright 2023 Pedro Farías 
#   Using part of the code written by Robert Gallup.


import sys, array, os, textwrap, math, random, argparse, struct

class DEFAULTS(object):
	STRUCTURE_NAME = 'GFXMeta'
	VERSION = '2.3.5'

def main ():

	# Default parameters
	infile = ""
	tablename = ""
	tablewidth = 16
	sizebytes = 0
	named = False
	version = False
	pico = True

	# Set up parser and handle arguments
	parser = argparse.ArgumentParser()
	# parser.add_argument ("infile", help="The WAVP file(s) to convert", type=argparse.FileType('r'), nargs='+', default=['-'])
	parser.add_argument ("infile", help="The WAV file(s) to convert", type=argparse.FileType('r'), nargs='*', default=['-'])
	# parser.add_argument ("-p", "--pico", help="generate PICOVGA01 .cpp file for sound with only one array of 1 byte per sample)", action="store_true")
	parser.add_argument ("-v", "--version", help="echo the current wav2hex version", action="store_true")
	args = parser.parse_args()

	# Required arguments
	infile = args.infile

	# Options
	double = False
	
	#if (args.pico):
	#	pico = True
	
		
		
	if args.version:
		print ('// wav2hex version ' + DEFAULTS.VERSION)

	if (pico):
		print ('#include "main.h" ')
		print ('')
		print ('// Sound file for PICOVGA01 examples. Format 8-bit PCM MONO samples 22050 Khz, single array')
		
	# Output named structure, if requested
	if (named):
		print ('struct ' + DEFAULTS.STRUCTURE_NAME + ' {')
		print ('  unsigned   int width;')
		print ('  unsigned   int height;')
		print ('  unsigned   int bitDepth;')
		print ('             int baseline;')
		print ('  ' + getDoubleType(double)[0] + 'pixel_data;')
		print ('};')
		print ('')

	# Do the work
	for f in args.infile:
		if f == '-':
			sys.exit()
		wav2hex(f.name, tablewidth, sizebytes, pico)

def unsupported_file_error(msg):
	"""Exit with unsupported file format message"""
	sys.exit("error: " + msg)

# Main conversion function
def wav2hex(infile, tablewidth, sizebytes, pico):
	"""Convert supported WAV files to hex string output"""
	
	# Set the table name to the uppercase root of the file name
	tablename = os.path.splitext(infile)[0]

	# Convert tablewidth to characters from hex bytes
	tablewidth = int(tablewidth) * 6

	# Initilize output buffer
	outstring =  ''

	# Open File
	fin = open(os.path.expanduser(infile), "rb")
	uint8_tstoread = os.path.getsize(os.path.expanduser(infile))
	valuesfromfile = array.array('B')
	try:
		valuesfromfile.fromfile(fin, uint8_tstoread)
	finally:
		fin.close()

	# Get bytes from file
	values=valuesfromfile.tolist()

	# Exit if it's not a Windows WAV
	if ((values[8] != 0x57) or (values[9] != 0x41) or (values[20] != 0x01)):
		unsupported_file_error("Not a WAV file.")
		
	if ((values[22] != 0x01)):
		unsupported_file_error("Not an uncompressed PCM file")
		
	if ((values[24] != 0x22) or (values[25] != 0x56)):
		unsupported_file_error("Sample rate is not 22050Khz")


	# Unpack header values using struct
	# Note: bytes(bytearray)) is used for compatibility with python < 2.7.3
	# dataOffset,  \
	# headerSize,  \
	# pixelWidth,  \
	# pixelHeight, \
	# colorPlanes, \
	# bitDepth,    \
	# compression, \
	dataOffset= 44
	samplerate   = struct.unpack("L", bytes(bytearray(values[24:28])))
	dataSize     = struct.unpack("L", bytes(bytearray(values[40:44])))
	
	print ('// for PICOVGA01 examples. WAV Sample rate is ' + str(samplerate))
	
	# Calculate line width in bytes and padded byte width (each row is padded to 4-byte multiples)
	byteWidth	= 1

	# For auto (sizebytes = 0), set sizebytes to 1
	sizebytes = 1
	dataByteLength=dataSize[0]
	# Output the hex table declaration
		
	if (pico):
		print ('// format: 8-bit indexed array')
		print ('// audio data size: ' + '[{0}]'.format(dataByteLength))
		print ('\n')
		print ('  const u8 ' + tablename + '[{0}]'.format(dataByteLength) + ' __attribute__ ((aligned(4))) = {')

	else:
		print ('PROGMEM const struct {')
		print ('  unsigned int   width;')
		print ('  unsigned int   height;')
		print ('  unsigned int   bitDepth;')
		print ('  ' + pixelDataType + 'pixel_data[{0}];'.format(dataByteLength)) 
		print ('} ' + tablename + ' = {')
		print ('{0}, {1}, {2}, {{'.format(pixelWidth, pixelHeight, bitDepth))

	# Generate HEX bytes for pixel data in output buffer
	try:
		for i in range(dataByteLength):
			ndx = dataOffset
			v = values[ndx+i]
			outstring += "{0:#04x}".format(v) + ", "

	# Wrap the output buffer. Print. Then, finish.
	finally:
		outstring = textwrap.fill(outstring[:-2], tablewidth)
		print (outstring)
		print ("};")


# Only run if launched from commandline
if __name__ == '__main__': main()
