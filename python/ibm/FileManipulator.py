#!/usr/bin/python

import os
import sys, getopt
import CoreCommands
import imp


def usage(reason):
    print 'program will halt:' +  reason
    print 'usage: FileManipulator.py -x <command> -i <input-file> [-o <output-file>]'


"""
This class provides __very basic__ io handling
the idea is to hide the entire i/o related work
from the rest of the work. in a proper system, a
lib full of i/o related services would be supplied
with many other services in its APIs, and a much
more robust implementation at its base
"""
class FileManagment:
    
    def __init__(self, inFile, outFile):
        self.miFile = inFile
        self.moFile = outFile
      
    # this *very* naiive function simply reads
    # the entire content of a file to memory
    # do not use for large files
    # todo handle errors
    def readFile(self):
        iFile = open(self.miFile, 'r')
        return iFile.read()

    # this *very* naiive function simply writes
    # the entire content of a memory buff to a file
    # do not use for large files
    # todo handle errors
    def writeFile(self, outBuffer):
        # if we did not get an out-file we use stdout
        oFile = sys.stdout
        if self.moFile != '':
            oFile = open(self.moFile, 'w')
        oFile.write(outBuffer+'\n')


"""
This class handles the command search logic
The following algorithm guides its actions:
Module-Search-Path:
can be customized via the MODULE_SEARCH_PATH env variable
if that is not used, modules are searched in the cwd
each module is loaded, and scanned for the command requested
if found - it is returned.
if not found, a None is returned back to the caller
This assumes python manages the module loading efficiently
"""
class CmdLookupMng:

    def __init__(self):
        self.mPathList = []
        custSearchPath = os.getenv('MODULE_SEARCH_PATH', '')
        if custSearchPath != '':
            self.mPathList.append(custSearchPath)
        self.mPathList.append(os.getcwd())

    def load_module_from_dir(self, dirPath):
        for fileNm in os.listdir(dirPath):
            mod_name,file_ext = os.path.splitext(os.path.split(fileNm)[-1])
            
            fileNm = dirPath + '/' + fileNm
            try:
                if file_ext.lower() == '.py':
                    py_mod = imp.load_source(mod_name, fileNm)

                elif file_ext.lower() == '.pyc':
                    py_mod = imp.load_compiled(mod_name, fileNm)
            except Exception as e:
                print 'module was not found for requested command. check search path'
                return None
        return py_mod

    def cmdLookup(self, command, fileMng):
        for path in self.mPathList:
            py_mod = self.load_module_from_dir(path)
            if (py_mod != None):
                if hasattr(py_mod, command):
                    class_inst = getattr(py_mod, command)(fileMng)
                    if class_inst != None:
                        return class_inst
        return None

def main(argv):
    if (len(sys.argv) < 5) or (len(sys.argv) > 7):
        usage('bad amount of command line arguments')
        exit(1)
    inputFile=''
    outputFile=''
    command=''

    try:
        opts, args = getopt.getopt(argv, "x:i:o:") #TODO support long options too
    except getopt.GetoptError:
        usage('bad opt switch encountered') #TODO get exception reason
        exit(1)

    for opt, arg in opts:
        if opt == '-x':
            command = arg
        elif opt == '-i':
            inputFile = arg
        elif opt == '-o':
            outputFile = arg
            
    # handle a situation where a switch repeated itself.
    # i think this is a bug in the getopt logic. it should
    # trace the fact that a certain switch were not supplied
    if command == '' or inputFile == '':
        usage('mandatory switch was not given')
        exit(1)

    try:
        fileManager = FileManagment(inputFile, outputFile)
    except Exception as e:
        print "preparing files for manipulation failed: %s" % e
        exit(2)

    #run command executer
    cmdLookupMng = CmdLookupMng()
    cmd = cmdLookupMng.cmdLookup(command, fileManager)
    # this is a one shot utility - if we have found what we need we term
    if cmd != None:
        cmd.execute()
        exit(0)
    else:
        print 'was not able to find command %s. check speling and module path' % command
    
    exit(4)
if __name__ == "__main__":
    main(sys.argv[1:])

