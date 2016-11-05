import os
from sets import Set

DIR = 'images/'
files = [i for i in os.listdir(DIR)]
files = list(Set(files))
for file in files:
	print file
	try:
		print "\nSTART "+file+"\n"
		if os.system("./run.sh "+DIR+file+" BSR_train_out/"+file.split('.')[0]+"_.ppm")==0:
			print "\nSUCCESS "+file+"\n"
		else :
			print "ERRRRRRRRRRRRRRRRRRRRRRRRRRRR"
	except Exception as inst:
		print (file, ipfile, inst)
