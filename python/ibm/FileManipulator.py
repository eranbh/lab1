#!/usr/bin/python

import os
import sys, getopt
import CoreCommands
import imp

def usage(reason):
    print 'program will halt:' +  reason
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
    if (len(sys.argv) < 5) or (len(sys.argv) > 7):
        usage('bad amount of command line arguments')
        exit(1)
    inputFile=''
    outputFile=''
    command=''

    try:
        opts, args = getopt.getopt(argv, "x:i:o") #TODO support long options too
    except getopt.GetoptError:
        usage('bad opt switch encountered') #TODO get exception reason
        exit(1)

    for opt, arg in opts:
        if opt == '-x':
            command = arg
        elif opt == '-i':
            inputFile = arg
        elif opt == '-o':
            outputfile = arg
    # handle a situation where a switch repeated itself.
    # i think this is a bug in the getopt logic. it should
    # trace the fact that a certain switch was already supplied
    if command == '' or inputFile == '':
        usage('mandatory switch was not given')
        exit(1)

    try:
        fileManager = FileManagment()
        fileManager.prepareFiles(inputFile, outputFile)
    except Exception as e:
        print "preparing files for manipulation failed: %s" % e
        exit(2)

    #run command executer
    
    modSearchPath = os.getenv('MODULE_SEARCH_PATH', os.getcwd()+ '/cmd')
    commandModule = imp.load_source(command, modSearchPath)
    if commandModule == None:
        print "module %s was not found" % command
        exit(2)
    from commandModule import command
    cmd = eval(command+"()")
    cmd.__init__(fileManager)
    cmd.execute()

if __name__ == "__main__":
    main(sys.argv[1:])

