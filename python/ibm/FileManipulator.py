#!/usr/bin/python

import sys, getopt
import CoreCommands

def usage():
    print 'usage: FileManipulator.py -x <command> -i <input-file> -o <output-file>'



class FileManagment:
    def __init__(self):
        miFile = ''
        moFile = ''
    #TODO handle all io errors
    def prepareFiles(self, inFile, outFile):

        miFile = open(inFile, 'r')
        
        # if we did not get an out-file we use stdout
        moFile = sys.stdout
        if outFile != '':
            moFile = open(outFile, 'w')

def main(argv):
    if len(sys.argv) < 3 or len(sys.argv) > 4:
        usage()
        exit(1)
    inputFile=''
    outputFile=''
    command=''

    try:
        opts, args = getopt.getopt(argv, "x:i:o") #TODO support long options too
    except getopt.GetoptError:
        usage() #TODO also get the reason for the exception

    for opt, arg in opts:
        if opt == -x:
            command = arg
        elif opt == -i:
            inputFile = arg
        elif opt == -o:
            outputfile = arg
    fileManager = FileManagment()
    fileManager.prepareFiles(inputFile, outputFile)

    #run command executer
    

if __name__ == "__main__":
    main(sys.argv[1:])
