#!/bin/bash
# The script copies related source files from DSP\source
# project to ProsoftSystems project

#retrieve source and target directories
sourceDirectory=$"C:/Qt_projects/DSP"
targetDirectory=$"C:/Qt_projects/ProsoftSystems"
sourceFilesPath=$"source"
libsFilesPath=$"libs"

#save default field separators
defaultIFS=$IFS;

#check source and target directories
if [ ! -d $sourceDirectory/$sourceFilesPath ]
then
	echo "name $sourceDirectory is not a directory"
	exit
fi

if [ ! -d $targetDirectory/$sourceFilesPath ]
then
	echo "name $targetDirectory is not a directory"
	exit
fi

path=$sourceDirectory/$targetDirectory
if [ -d $path ]
then
	echo "$path is subdirectory of $sourceDirectory. Cannot copy files there"
	exit
fi

#copy relevant files from source to target directory
for path in $targetDirectory/$sourceFilesPath/*
do
	#parse filename from target directory
	IFS="/"
	read -a pathName <<< $path
	filename=${pathName[${#pathName[*]}-1]};
	IFS=$defaultIFS;
	
	#check if source directory has such a file
	#if so, we copy it to target directory
	if [ -f $sourceDirectory/$sourceFilesPath/$filename ]
	then
		echo "copy: $filename from $sourceDirectory to $targetDirectory"
		cp $sourceDirectory/$sourceFilesPath/$filename $targetDirectory/$sourceFilesPath
	fi
done

# copy libs
echo "copy related libs"

rm $targetDirectory/$libsFilesPath
cp -r $sourceDirectory/$libsFilesPath/ $targetDirectory/$libsFilesPath

read -p "Press any key to continue" x