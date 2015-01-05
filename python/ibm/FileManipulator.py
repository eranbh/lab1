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


class CmdLookupMng:

    mFileManager = FileManagment()
    def __init__(self, fileManager):
        mFileManager=fileManager

    def load_module_from_dir(self, dirPath):
        for fileNm in os.listdir(dirPath):
            mod_name,file_ext = os.path.splitext(os.path.split(fileNm)[-1])
            
            fileNm = dirPath + '/' + fileNm
            print fileNm

            if file_ext.lower() == '.py':
                py_mod = imp.load_source(mod_name, fileNm)

            elif file_ext.lower() == '.pyc':
                py_mod = imp.load_compiled(mod_name, fileNm)
        return py_mod

    def cmdLookup(self, path, command):
        py_mod = self.load_module_from_dir(path)
        if (py_mod != None):
            if hasattr(py_mod, command):
                class_inst = getattr(py_mod, command)(self.mFileManager)
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
    custSearchPath = os.getenv('MODULE_SEARCH_PATH', '')
    localSearchPath= os.getcwd() + '/cmds'
    cmdLookupMng = CmdLookupMng(fileManager)
    
    # first we look into customized modules
    if custSearchPath != '':
        cmd = cmdLookupMng.cmdLookup(custSearchPath, command)
        # this is a one shot utility - of we have found what we need we term
        if cmd != None:
            cmd.execute()
            exit(0)
    # we fall back on local modules
    cmd = cmdLookupMng.cmdLookup(localSearchPath, command)
    if cmd != None:
        cmd.execute()

if __name__ == "__main__":
    main(sys.argv[1:])

