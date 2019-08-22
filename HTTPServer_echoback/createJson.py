# coding: UTF-8

import os

 
def getChangedDirectory(commandStr):
    """
    Return directory moved in makefile command.

    Parameters
    ----------
    commandStr : str
        makefile compile command string
    """

    if str(commandStr).find('-C') >= 0:
        commandArray = str(commandStr).split(' ')
        
        index = commandArray.index('-C')

        return commandArray[index + 1]

def isCompilerCommands(commandStr, compilerNames):
    """
    Check if the command is compile message.
    Command must be started with compiler name.

    Parameters
    ----------
    commandStr : str
        makefile compile command string
    
    ----------
    compilerNames : list
        compiler names
    """
    result = False
    for compiler in compilerNames:
        if str(commandStr).find(compiler, 0) == 0:
            result = True
            break
    
    return result
    
    

#file readout
path = './Make.log'
logFile = open(path, mode='r')

lines = logFile.readlines()

jsonPath = 'compile_commands.json'
jsonFile = open(jsonPath, mode='w')
jsonFile.write('[')

#compiler
compilers = ['arm-none-eabi-gcc', 'arm-none-eabi-g++']

#extensions
extensions = ['.c', '.cpp', '.s']

#currentDirectory
currentDirectory = str(os.getcwd()).replace('\\', '/')

cnt = 0
for line in lines:
    #check the direcory change
    if getChangedDirectory(line) != None:
        currentDirectory = getChangedDirectory(line)

    #check if the line starts with compiler name
    if isCompilerCommands(line, compilers) == True:
        #if not first comma
        if cnt > 0:
            jsonFile.write(',')

        #start json database
        jsonFile.write('\n\t{\n')

        #directory
        jsonFile.write('\t\t"directory": ')
        jsonFile.write('"' + currentDirectory + '",')
        jsonFile.write('\n')

        #start compile commands
        jsonFile.write('\t\t"arguments": [\n')

        #targetFilePath
        targetFilePath = ''

        #split compile commands
        commandsCount = 0
        commands = line.split(' ')
        for option in commands:
            if commandsCount > 0:
                jsonFile.write(',\n')

            jsonFile.write('\t\t\t' + '"' + str(option).strip('\r\n') + '"')
            commandsCount+=1

            #save file fullpath
            isSourceFileName = False
            for extension in extensions:
                if str(option).strip('\r\n').endswith(extension) == True:
                    targetFilePath = str(option).strip('\r\n')
                    break



        #end compile commands
        jsonFile.write('\n')
        jsonFile.write('\t\t],\n')
        
        #target sourcename
        jsonFile.write('\t\t"file": ')
        jsonFile.write('"' + targetFilePath + '"')
        jsonFile.write('\n')

        #end json database
        jsonFile.write('\t}')

        cnt+=1
#end jsonfile
jsonFile.write('\n')        
jsonFile.write(']\n')
