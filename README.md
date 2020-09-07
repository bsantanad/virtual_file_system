# File System in C

A fast simulator of a file system written from scratch in C.

## Getting Started

If you just want to run it you have to download the project, decompress it, and run:

```bash
    foo@bar~:$ ./FileSystem
```

### Prerequisites

If you want to compile it yourself you can just do

```bash
    foo@bar~:$ make
```
And then to use it you can now use.

```bash
    foo@bar~:$ ./FileSystem
```
## How to use it

Once you run it, a simulation of a terminal will be started, here are the commands you can use,

```bash
    foo@bar~:$ create <name> <size>MB <blockSize>K
```
Creates a new File System.

```bash
    foo@bar~:$ unmount <name>
```
Save the File System. 

**NOTE:** You must use this command the first time you create a disk, in order to use it

```bash
    foo@bar~:$ mount <name>
```
Load a File System into the program in order to upload and download files.

```bash
    foo@bar~:$ upload <fileName>
```
Upload a File into the File System.

```bash
    foo@bar~:$ download <fileName>
```
Download a File from the File System

**NOTE:** The name of the file will be the same in the File System and in your PC. You CANNOT change the name of the file.

```bash
    foo@bar~:$ ls
```
List of all the Files in the File System

```bash
    foo@bar~:$ info
```
Information of the File System

```bash
    foo@bar~:$ details <fileName>
```
Information of a File in the File System.

```bash
    foo@bar~:$ exit
```
Exit the File System.

### Tips and Advices

You must unmount a File System once you uploaded the files you want in order to save it.

Be careful when downloading a file, the name will be the same as the one you upload so it may overwrite it if it's in the same directory.

## Versioning

This is the version 1.0.

## Authors

* **Benjamin Santana** - [blog](https://benjasantana.github.io)

## Licence

This project is licensed under the GNU General Public License

## Acknowledgments

The hash function used was based on [this](http://www.cse.yorku.ca/~oz/hash.html) one.
