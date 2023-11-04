#pragma once
#include "./list.h"


#include <libm/fsStuff/fsInfo/fileSystemStructs.h>

template class Array<FilesystemInterface::BaseInfo*>;
template class  List<FilesystemInterface::BaseInfo*>;

template class Array<FilesystemInterface::FileInfo*>;
template class  List<FilesystemInterface::FileInfo*>;

template class Array<FilesystemInterface::FolderInfo*>;
template class  List<FilesystemInterface::FolderInfo*>;

