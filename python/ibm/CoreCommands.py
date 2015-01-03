""" this module contains the out-of-the-box modules
    supported by the system. these modules are supplied
    as part of the package and are not / can not be
    subjected to any customizations. 

    any client of this mechanism who wishes create 
    a customized command, needs to adhere to the FileManipulationCmd
    interface defined here """

from abc import ABCMeta, abstractmethod

class FileManipulationCmd:
    
    __metaclass__ = ABCMeta
    mFileManager = ''
    def __init__(fileManager):
      mFileManager = fileManager  
    
    @abstractmethod
    def execute(self):
        pass


class Sort(FileManipulationCmd):
    def __init__(self, fileManager):
        Base.__init__(self, fileManager)

    def execute(self, fileManager):
        print "Sort [Execute]"


class Reverse(FileManipulationCmd):
    
    def __init__(self, fileManager):
        Base.__init__(self, fileManager)
    
    def execute(self):
        Base.__init__(self, fileManager)
        print "Reverse [Execute]"
