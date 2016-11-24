#!/usr/bin/python2

'''
Author: Collin James
Date: 11/24/16
Description: Creates 3 files, writes a string of 10 random characters plus a carriage return to the files and 
also prints to screen. Then prints 2 random numbers between 1 and 42 to the screen. Finally, prints the product 
of the two numbers.

Note: Please run with Python 2.X
'''

import random 	# for random integers
import os 		# for file manipulation

# seed the randomizer (defaults to using current time)
random.seed()

def randomString(alphabet, numLetters=10):
	'''
	Generates a string *numLetters* long from the provided *alphabet*, plus a carriage return. Returns the string.
	'''
	returnStr = ''

	# generate random number (0-25), use to pick a letter, and append letter
	for x in xrange(0,numLetters):
		returnStr += alphabet[random.randint(0, 25)]
	
	# append newline
	returnStr += '\n'

	return returnStr

def printFileError(msg, fname, e):
	'''
	Help printing errors in writeToFile()
	'''
	print "Could not %s file %s because %s" % (msg, fname, e)

def writeToFile(word, fname):
	'''
	Open file *fname*, write *word* string to it, and close file. Return 0 for no error, 1 for error
	'''	
	# open file *fname*, catching errors
	try:
		fd = os.open(fname, os.O_WRONLY | os.O_CREAT | os.O_TRUNC)
	except IOError as e:
		printFileError("open", fname, e)
		return 1
	
	# write *word* to your file, catching errors
	try:
		os.write(fd, word)
	except IOError as e:
		printFileError("write to", fname, e)
		return 1

	# close file, catching errors
	try:
		os.close(fd)
	except IOError as e:
		printFileError("close", fname, e)
		return 1

	return 0

def printIntsAndProduct(low=1, high=42, numInts=2):
	'''
	generate *numInts* random numbers between *low* and *high*, print them, and then print the product of all numbers
	'''
	theProd = 1 	# accumulate product here
	currentNum = 0 	# save your current number
	str1 = "Here are your integers: " # add numbers to this string

	# generate *numInts* random numbers, print them, and accumulate product w/ each pass
	for x in xrange(0,numInts):
		# generate random number and save it
		currentNum = random.randint(low, high)
		
		# add number to string
		str1 += str(currentNum)

		# add a comma and space between numbers (except last one)
		if x < (numInts-1):
			str1 += ", "

		# multiply it by sum
		theProd *= currentNum

	# print the string of numbers
	print str1

	# print the product
	print "The product of your integers is %d" % theProd

def main(filenames=["Dave", "Krist", "Kurt"]):
	'''
	Create 3 files, write a string of 10 random characters plus a carriage return to the files and 
	also print to screen (with nice message). Then print 2 random numbers between 1 and 42 to the screen. 
	Finally, print the product of the two numbers.
	'''
	
	alphabet = 'abcdefghijklmnopqrstuvwxyz' # the alphabet
	stringHolder = '' # holds your random string 
	str1 = "Here are your strings (also in files " # add filenames to this
	flen = len(filenames) # number of files

	# finish the line started in *str1*
	for x in xrange(0,flen):
		# add filenames to string
		str1 += filenames[x]
		# add a comma between names, but not after last name
		if x < (flen-1):
			str1 += ", "
	# finish the string and print it
	str1 += "):"
	print str1

	# get a string for each file, print it, and write to file
	for x in xrange(0,len(filenames)):
		# create your string
		stringHolder = randomString(alphabet)
		# print to screen
		print "\t%s" % stringHolder, # comma afterward prevents newline (python 2)
		# write to file
		writeToFile(stringHolder, filenames[x])

	# newline
	print 

	# generate and print integers and product
	printIntsAndProduct()
	
# run the program
if __name__ == '__main__':
	main()