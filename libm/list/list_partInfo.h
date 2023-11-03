#pragma once
#include "./list.h"



#include "../../kernel/diskStuff/Partition_Interfaces/generic/partitionStructs.h"

template class Array<PartitionInterface::PartitionInfo*>;
template class  List<PartitionInterface::PartitionInfo*>;

