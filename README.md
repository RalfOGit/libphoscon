# libtasmota
A simplistic C++ library to access zigbee devices through a phoscon gateway.

This library is work in progress and will always be. It provides just the functionality that I need for my own applications. For a complete implementation of the phoscon rest api, please refer to the official github repository https://github.com/dresden-elektronik/deconz-rest-plugin.

libphoscon is self-contained, i.e. it does not have any external library dependencies. Cudos to the very small footprint json parser written by James McLaughlin: https://github.com/udp/json-parser, which is included in the library.

The simplest way to build this library together with your code is to checkout this library into a separate folder and use unix symbolic links (ln -s ...) or ntfs junctions (mklink /J ...) to integrate it as a sub-folder within your projects folder.

For example, if you are developing on a Windows host and your projects reside in C:\workspaces:

    cd C:\workspaces
    mkdir libphoscon
    git clone https://github.com/RalfOGit/libphoscon
    cd ..\YOUR_PROJECT_FOLDER
    mklink /J libphoscon ..\libphoscon
    Now you can start Visual Studio
    And in Visual Studio open folder YOUR_PROJECT_FOLDER

And if you are developing on a Linux host and your projects reside in /home/YOU/workspaces:

    cd /home/YOU/workspaces
    mkdir libphoscon
    git clone https://github.com/RalfOGit/libphoscon
    cd ../YOUR_PROJECT_FOLDER
    ln -s ../libphoscon
    Now you can start VSCode
    And in VSCode open folder YOUR_PROJECT_FOLDER

The source code contains doxygen comments, so that you can generate documentation for the library.

For now, libphoscon supports just plain http as the underlying network protocol. There is neither authentication nor encryption support.

Keep in mind, the software comes as is. No warrantees whatsoever are given and no responsibility is assumed in case of failure or damage being caused.

The code has been tested against the following environment:

        OS: CentOS 8(TM), IDE: VSCode (TM)
        OS: Windows 10(TM), IDE: Visual Studio Community Edition 2019 (TM)
