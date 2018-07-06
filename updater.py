#!/usr/bin/python2
import sys
import os
import pycurl
import urllib
try:
    from io import BytesIO
except ImportError:
    from StringIO import StringIO as BytesIO

buff = BytesIO()

def progress(count, total, status=''):

	CURSOR_UP_ONE = '\x1b[1A'
	ERASE_LINE = '\x1b[2K'
	print(CURSOR_UP_ONE + ERASE_LINE)
	print(CURSOR_UP_ONE + ERASE_LINE)
	bar_len = 60
	filled_len = int(round(bar_len * count / float(total)))
	percents = round(100.0 * count / float(total), 1)
	bar = '=' * filled_len + '-' * (bar_len - filled_len)
	sys.stdout.write('[%s] %s%s ...%s\r' % (bar, percents, '%', status))
	sys.stdout.flush()

def ftostr(fname):
	new = ""
	with open(fname, 'r') as f:
		while True:
			c=f.readline()

			if not c:
				break

			for i in range(0, len(c)):
				new += c[i]

	f.close()
	return new


def sendPost(url, args ):
	c = pycurl.Curl()
	postfields = urllib.urlencode(args)
	c.setopt(c.URL, url)
	c.setopt(c.POSTFIELDS, postfields)
	c.setopt(c.WRITEFUNCTION, buff.write)
	c.perform()

	c.close()



def getMemory(url):
	buffer = {'memory':'0'}
	sendPost(url, buffer)

def clearEEPROM(url):
	buffer = {'clear':'1'}
	sendPost(url, buffer)

def storeToEEPROM(url):
	buffer = {'store':'1'}
	sendPost(url, buffer)

def loadFromEEPROM(url):
	buffer = {'restore':'1'}
	sendPost(url, buffer)



def update(fname, url, size):

	data = ftostr(fname)
	address = 0
	length = len(data)
	size = int(size)

	buffer = {'update':length}
	sendPost(url, buffer)

	while address < length:
		if (address + size) < length:
			buffer = {'address': address, 'value' : data[address:address + size], 'size': str(size)}
		else:
			buffer = {'address': address, 'value' : data[address:(address + (length - address))], 'size': length - address}
		address += size
		sendPost(url, buffer)
		print(buff.getvalue())
		#buff.clear()




def getUsage():
	print( "Usage :")
	print( "Send file via HTTP post:  updater <file name> <\"IP address\">" )
	print( "Clear server memory : updater clear <\"IP address\">")

	print( "example: updater index.html \"192.168.0.1\" ")


#------ Main

if len(sys.argv) == 1:
	print(" too few arguments")


elif len(sys.argv) == 2:

	try:
		if sys.argv[1] == "-h":
			getUsage()
		else:
			print ( " updater: too few arguments")
			print ( " updater -h will give you informations about right usage")
	except:
		pass
elif len(sys.argv) == 3:
    if sys.argv[1] == "store":
    	storeToEEPROM(sys.argv[2]);
    elif sys.argv[1] == "restore":
        loadFromEEPROM(sys.argv[2])
    elif sys.argv[1] == "clear":
		clearEEPROM(sys.argv[2])

elif len(sys.argv) == 4:

	if not os.path.exists(sys.argv[1]):
		print(" updater: file " + sys.argv[1] + " does not exist")


	else:
		update(sys.argv[1], sys.argv[2], sys.argv[3])
