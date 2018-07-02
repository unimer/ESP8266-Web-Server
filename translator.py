#!/usr/bin/python2
import sys
import os

html = ""

def translate(fname):
	new = "\""

	with open(fname, 'r') as f:
		while True:
			c=f.readline()

			if not c:
				break
	
			for i in range(0, len(c)):
				if c[i] == "\"":
					new += "\\"
				elif c[i] == "\'":
					new += "\\"
				elif c[i] == "\n":
					continue
				elif c[i] == "\t":
					continue
				new += c[i]

		new += "\""
	f.close()
	return new

def write(argv, content):

	if len(argv) == 3:
		oname = argv[2]
	else:
		oname = "translated"

	file = open(oname, "w")
	file.write(content)
	file.close()
	print("		filename: " + oname)	


if os.path.exists(sys.argv[1]): 
	result = translate(sys.argv[1])
	write(sys.argv, result)
else: 
	print("		translator: Input file does not exists")
	exit(0)


print("		translator: Translation Done")
		
