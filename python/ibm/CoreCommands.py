""" this module contains the out-of-the-box modules
    supported by the system. these modules are supplied
    as part of the package and are not / can not be
    subjected to any customizations.

    any client of this mechanism who wishes create
    a customized command, needs to adhere to the FileManipulationCmd
    interface defined here """

from FileManipulator import FileManagment
from abc import ABCMeta, abstractmethod

class FileManipulationCmd:

    __metaclass__ = ABCMeta
    def __init__(self,fileManager):
      self.mFileManager = fileManager  
    
    @abstractmethod
    def execute(self):
        pass


class Sort(FileManipulationCmd):
    def __init__(self, fileManager):
        FileManipulationCmd.__init__(self, fileManager)

    def execute(self):
        print ("Sort [Execute]")

class Reverse(FileManipulationCmd):

    def __init__(self, fileManager):
        FileManipulationCmd.__init__(self, fileManager)

    def execute(self):
        print "Reverse [Execute]"
        inBuff=''
        inBuff = self.mFileManager.readFile()
        rev=""
        for i in range (0, len(inBuff)):
            rev += inBuff[(len(inBuff)-1) -i]
        self.mFileManager.writeFile(rev)
