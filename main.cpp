/******************************************/
/*  CSCI5806 OS PROJECT EXT2 Filesystem   */
/*      Tanner Ewing & Brent Prox         */
/******************************************/


#include "Ext2.h"

ext2::ext2()
{
	blockSize    = 1024;
	groups = 1;
	buffer = new char[1024];
}

int main()
{
	ext2 file;
	file.selectFileTest();

	/* display general stats */
	file.readSuperblockInfo();
	file.readGroupDescriptorTable();
	file.readRootInodeInfo();
	file.readDirectoryInfo();

	/* check integrity */
	file.checkSuperblockConsistency();


	return 0;
}
