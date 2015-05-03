/******************************************/
/*  CSCI5806 OS PROJECT EXT2 Filesystem   */
/*      Tanner Ewing & Brent Prox         */
/******************************************/

#include <string>
#include <fstream>
#include <stdio.h>
#include <cstring>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <iterator>
#include <iostream>

#define EXT2_SUPER_MAGIC 61267

using namespace std;

class ext2
{
	public:
		ext2();		
		
		void selectFileTest();               
		void readSuperblockInfo();		 
		void readGroupDescriptorTable();      
		void readRootInodeInfo();            
		void readInodeInfo();                
		void readDirectoryInfo();
		void ChooseDirectory();
		void ChooseToDirectory();	
		void fetchBlock();               
		void fetchBlockType();
		void fetchDirectBlock();
		void fetchSingleIndirectBlock();
		void fetchDoubleIndirectBlock();
		void fetchTripleIndirectBlock();
		void OutputInodeTableBlock();                        
		
	private:

		struct Directory 
		{
		
			uint32_t inode;                         
			uint16_t rec_len;                       
			uint8_t  name_len;                      
			uint8_t  file_type;                     
			char     name[1];                      

	};


		struct Superblock 
		{
		
       		uint32_t s_inodes_count;                        
			uint32_t s_blocks_count;                        
			uint32_t s_reserved_blocks_count;                      
			uint32_t s_free_blocks_count;                   
			uint32_t s_free_inodes_count;                   
			uint32_t s_first_data_block;                    
			uint32_t s_log_block_size;                      
			uint32_t s_log_frag_size;                       
			uint32_t s_blocks_per_group;                    
			uint32_t s_frags_per_group;                     
			uint32_t s_inodes_per_group;                    
			uint32_t s_mtime;                               
			uint32_t s_wtime;                               
			uint16_t s_mnt_count;                           
			uint16_t s_max_mnt_count;                       
			uint16_t s_magic;                               
			uint16_t s_state;                               
			uint16_t s_errors;                              
			uint16_t s_minor_rev_level;                     
			uint32_t s_lastcheck;                           
			uint32_t s_checkinterval;                       
			uint32_t s_creator_os;                          
			uint32_t s_rev_level;                           
			uint16_t s_def_resuid;                          
			uint16_t s_def_resgid;                          
			uint32_t s_first_ino;                           
			uint16_t s_inode_size;                          
			uint16_t s_block_group_nr;                      
			uint32_t s_feature_compat;                      
			uint32_t s_feature_incompat;                    
			uint32_t s_feature_ro_compat;                   
			uint8_t  s_uuid[16];                            
			char     s_volume_name[16];                     
			char     s_last_mounted[64];                    
			uint32_t s_algo_bitmap;                         
			uint8_t  s_prealloc_blocks;                     
			uint8_t  s_prealloc_dir_blocks;                 
			uint16_t s_padding;                            
			uint32_t s_reserved[204];                       
		};

		struct Groupblock
		{

			uint32_t bg_block_bitmap;               
			uint32_t bg_inode_bitmap;               
			uint32_t bg_inode_table;                
			uint16_t bg_free_blocks_count;          
			uint16_t bg_free_inodes_count;          
			uint16_t bg_used_dirs_count;            
			uint16_t bg_pad;                        
			uint8_t  bg_reserved[12];               

		};

		struct Inode
		{

			uint16_t i_mode;                        
			uint16_t i_uid;                         
			uint32_t i_size;                        
			uint32_t i_atime;                       
			uint32_t i_ctime;                       
			uint32_t i_mtime;                       
			uint32_t i_dtime;                       
			uint16_t i_gid;                         
			uint16_t i_links_count;                 
			uint32_t i_blocks;                      
			uint32_t i_flags;                       
			uint32_t i_osd1;                        
			uint32_t i_block[15];                   
			uint32_t i_generation;                  
			uint32_t i_file_acl;                    
			uint32_t i_dir_acl;                     
			uint32_t i_faddr;                       
			uint32_t i_osd2[3];                     
		
		};

			int        blockSize;                       	                  	
			int        count;
			int        size;                     		
			int        groups;   
			int        InodeTableBlock;   
			int        IndirectType; 
			int        usedinodes;               	
		    
		    char *     buffer;
		    char *     outputDirectory;
		    char *     tableBlock; 
		    char *     filename;               
	       
	       unsigned char *     dirArray;                                  
		   unsigned char *     IndexPointer;                
		   Directory *     DirectoryPointer;
	       
		    uint32_t *     singleIndirect;               
		    uint32_t *     doubleIndirect;                    
		    uint32_t *     tripleIndirect;                    
		      
		    string       tempname;               
		    ifstream       filestream;
			char *	   Type;

		    Superblock theSuperblock;
			Groupblock gblock;
			Directory  currentDir;
			Inode      currentInode;
			
		
};

//this function is used to select the test file and lets the use know if the file successfully 
//opened or not. 
void ext2::selectFileTest()
{	
	cout << "Enter file name: ";
	getline(cin, tempname);
	
	filename = new char[tempname.length()];
	
	for(int i=0; i<tempname.length(); i++)
	{
		filename[i] = tempname[i];
	}
	 
	filestream.open(filename, fstream::in | fstream::binary);	
	
	if(filestream.is_open())
	{
		cout << "File opened successfully."
		     << "\n\n";
	}
	else
	{
		cout << "File cannot be opened."
		     << "\n";
	}
}

//function reads in the superblock of the file system and parses the data
void ext2::readSuperblockInfo()
{ 
	
	filestream.seekg(1024, ios_base::beg); 
	filestream.read(buffer, 1024); 
	memcpy(&theSuperblock, buffer, 1024); 
	
	blockSize = 1024 << theSuperblock.s_log_block_size; 
	groups = theSuperblock.s_blocks_count / theSuperblock.s_blocks_per_group; 
	buffer = new char[blockSize];
	

	cout << "File System Information from the Superblock:"
	     << "\n    System size:\t\t"   
	     << ((theSuperblock.s_blocks_count * blockSize) + 2048) 
	     
	     << "\n    Used space:\t\t\t"        
	     << ((theSuperblock.s_blocks_count - theSuperblock.s_free_blocks_count)*blockSize) 
	     
	     << "\n    Free space:\t\t\t"        
	     << (theSuperblock.s_free_blocks_count*blockSize) 
	     
	     << "\n    Block size:\t\t\t"        
	     << blockSize
	     
	     << "\n    Total blocks:\t\t"          
	     << theSuperblock.s_blocks_count 
	     
	     << "\n    Free blocks:\t\t"       
	     << theSuperblock.s_free_blocks_count 
	     
	     << "\n    Total inodes:\t\t"    
	     << theSuperblock.s_inodes_count 
	     
	     << "\n    Used inodes:\t\t"          
	     << theSuperblock.s_inodes_count-theSuperblock.s_free_inodes_count 
	     
	     << "\n    Free inodes:\t\t"       
	     << theSuperblock.s_free_inodes_count 
	     
	     << "\n    Block groups:\t\t"    
	     << groups
	     
	     << "\n    Inodes per group:\t\t"    
	     << theSuperblock.s_inodes_per_group 
	     
	     << "\n    Superblock Magic Number:\t"    
	     << theSuperblock.s_magic
	     << "\n    Superblock Magic Number is Correct:\t";	
	     if (theSuperblock.s_magic == EXT2_SUPER_MAGIC) {
		cout << "YES";
		} else {
		cout << "NO";
		}

	cout << "\n    File System State:\t";	
	     if (theSuperblock.s_state == 1) {
		cout << "CLEAN\n\n";
		} else {
		cout << "HAS ERRORS\n\n";
		}
	    
}

//function reads in the data from the inode table and parses it
void ext2::readInodeInfo()
{

	filestream.seekg((blockSize*gblock.bg_inode_table) + (DirectoryPointer->inode*128) - 128, ios_base::beg);
	filestream.read(buffer, blockSize);
	
	memcpy(&currentInode, buffer, 128);
	
	cout << "\nInode Information from the Inode Table:"
	     << "\n    User ID: \t\t\t"         
	     << currentInode.i_uid
	  
	     << "\n    Group ID: \t\t\t"        
	     << currentInode.i_gid

	     << "\n    File size: \t\t\t"                 
	     << currentInode.i_size
	    
         << "\n    Number of reserved blocks: \t"     
	     << currentInode.i_blocks

	     << "\n    Last access (seconds): \t"     
	     << currentInode.i_atime
	     << "\n\n";
}

//function reads in the root inode information from the inode table
void ext2::readRootInodeInfo()
{
	filestream.seekg((blockSize * gblock.bg_inode_table) + 128, ios_base::beg);
	filestream.read(buffer, blockSize);
	memcpy(&currentInode, buffer, 128);
	
	cout << "Root Inode Information from Inode Table:"
	     << "\n    User ID: \t\t\t"         
	     << currentInode.i_uid
	     
	     << "\n    File size: \t\t\t"                 
	     << currentInode.i_size
	   
	     << "\n    Format and access rights: \t" 
	     << currentInode.i_mode
	   
	     << "\n    Last access (seconds): \t"     
	     << currentInode.i_atime
	     
	     << "\n    Group ID: \t\t\t"        
	     << currentInode.i_gid
	     
	     << "\n    Number of reserved blocks: \t"     
	     << currentInode.i_blocks
	     << "\n\n";
}

//function reads the group descriptor table and parses the data
void ext2::readGroupDescriptorTable()
{
	if(blockSize != 1024)
	{
	filestream.seekg(blockSize, ios_base::beg);
	}
	
	filestream.read(buffer, blockSize);
	
	memcpy(&gblock, buffer, 32);
	
	
	/* # of block groups on disk */
	unsigned int groups = theSuperblock.s_blocks_count / theSuperblock.s_blocks_per_group;

	/* size of group descriptor list in bytes */
	unsigned int desc_list_size = groups * sizeof(struct Groupblock);

	usedinodes = theSuperblock.s_inodes_per_group - gblock.bg_free_inodes_count;
	
	cout << "Group Information from the Group Descriptor Table:"
	     << "\n    Block Bitmap: \t\t"    
	     << gblock.bg_block_bitmap
	    
	     << "\n    Free group blocks: \t\t"     
	     << gblock.bg_free_blocks_count
	    
	     << "\n    Inode Bitmap: \t\t"    
	     << gblock.bg_inode_bitmap
	    
	     << "\n    Inode Table: \t\t"     
	     << gblock.bg_inode_table
	    
	     << "\n    Free group inodes: \t\t"     
	     << gblock.bg_free_inodes_count
	    
	     << "\n    Used directories: \t\t"  
	     << gblock.bg_used_dirs_count
	     << "\n\n";
	    
	     InodeTableBlock = gblock.bg_inode_table;
}

//function reads current directory 
void ext2::readDirectoryInfo()
{
	string node  = ".";
	size   = ceil(currentInode.i_size/blockSize);
	int dirSize   = blockSize*size;
	dirArray = new unsigned char[dirSize];

	cout << "\nDirectory Size: " 
	     << dirSize
        
         << "\nFile size: \t" 
	     << currentInode.i_size
	    
	     << "\nBlocks Needed: \t" 
	     << size
         << "\n";
        
	
	tripleIndirect = new uint32_t[blockSize/4];
	doubleIndirect = new uint32_t[blockSize/4];
	singleIndirect = new uint32_t[blockSize/4];
	count    = 0;

	fetchBlockType();
	fetchBlock();

	for (IndexPointer = dirArray; dirArray - IndexPointer < currentInode.i_size;)
	{
		DirectoryPointer = (Directory *)IndexPointer;
		if(DirectoryPointer -> inode == 0){break;}

		cout << DirectoryPointer->inode << "\t" << DirectoryPointer->rec_len << "\tFile/Directory: ";
		for (int i=0;i<DirectoryPointer->name_len;i++)
		{
			cout << DirectoryPointer->name[i];
		}

		cout << "\n";
		IndexPointer += DirectoryPointer->rec_len;
	}
	
	OutputInodeTableBlock();
}

//function displays the output of a block
void ext2::OutputInodeTableBlock()
{
	cout << "\n\nOutput of block " 
	     << InodeTableBlock << ":\n\n";
	
	for (int i = 0; i < 1024; i++)
	{
		cout << buffer[i + InodeTableBlock + 1024];
	}
}

//function to choose directory to move file to 
void ext2::ChooseDirectory()
{
	string node  = ".";

	cout << "\nSelect File/Directory: ";
	getline(cin, node);
	
	if( node == "exit")
	{	
		cout << "\n";
		exit(0);
	}

	else
	{			

		bool same;
		for(IndexPointer = dirArray; dirArray - IndexPointer <currentInode.i_size;)
		{
			DirectoryPointer = (Directory *)IndexPointer;
			if(DirectoryPointer -> inode == 0){break;}

			for(int i=0; i<node.length(); i++)
			{
				if(node[i] == DirectoryPointer->name[i]){same = true;}
				else{same = false; break;}
			}

			IndexPointer += DirectoryPointer->rec_len;
			if(same == true){break;}
		}
		cout << "\nSelected Directory/File: " << DirectoryPointer->inode <<  "\t";
		for(int i=0; i<DirectoryPointer->name_len; i++){cout << DirectoryPointer->name[i];}
		cout << "\n";
		
		tripleIndirect = new uint32_t[blockSize/4];
		doubleIndirect = new uint32_t[blockSize/4];
		singleIndirect = new uint32_t[blockSize/4];
		count    = 0;

		readInodeInfo();

		
		filestream.seekg((blockSize * gblock.bg_inode_table) + 128, ios_base::beg);
		filestream.read(buffer, blockSize);
		memcpy(&currentInode, buffer, 128);
		readDirectoryInfo();
		
	}
}

 
void ext2::ChooseToDirectory()
{

}

//function that gets the block type
void ext2::fetchBlockType()
{

	
	if(size <= 12)
	{
	IndirectType = 0;
	Type = "Direct";
	}


	if(size > 12 && size <= (12 + (blockSize/4)))
	{
	IndirectType = 1;
	Type = "Single Indirect";
	}

	
	if(size > (12 + (blockSize/4)) && size <= (12 + (blockSize/4) + pow(blockSize,2)))
	{
	IndirectType = 2;
	Type = "Double Indirect";
	}

	
	if(size > (12 + (blockSize/4) + pow(blockSize,2)))
	{
	IndirectType = 3;
	Type = "Triple Indirect";
	}

	cout << "\nBlock Type: " << Type << "\n\n";
}

//fucntion that gets single indirect block
void ext2::fetchSingleIndirectBlock()
{
	if(IndirectType == 1)
	{
		for(int i=0; i<12; i++)
		{
			filestream.seekg((blockSize*currentInode.i_block[count]), ios_base::beg);
			filestream.read(buffer, blockSize);

			for(int j=0; j<blockSize; j++)
			{
					dirArray[(blockSize*i) + j] = buffer[j];
			}
		}

		filestream.seekg((blockSize*currentInode.i_block[12]), ios_base::beg);
		filestream.read(buffer, blockSize);
		memcpy(&singleIndirect, buffer, blockSize);

		for(int i=0; i<(size - 12); i++)
		{
			filestream.seekg((blockSize*singleIndirect[i]), ios_base::beg);
			filestream.read(buffer, blockSize);

			for(int j=0; j<blockSize; j++)
			{
					dirArray[(blockSize*(i + 12)) + j] = buffer[j];
			}
		}
	}

}

//function that gets double indirect block
void ext2::fetchDoubleIndirectBlock()
{
		for(int i=0; i<12; i++)
		{
			filestream.seekg((blockSize*currentInode.i_block[count]), ios_base::beg);
			filestream.read(buffer, blockSize);

			for(int j=0; j<blockSize; j++)
			{
					dirArray[(blockSize*i) + j] = buffer[j];
			}
		}

		filestream.seekg((blockSize*currentInode.i_block[12]), ios_base::beg);
		filestream.read(buffer, blockSize);
		memcpy(&singleIndirect, buffer, blockSize);

		for(int i=0; i<(blockSize/4); i++)
		{
			filestream.seekg((blockSize*singleIndirect[i]), ios_base::beg);
			filestream.read(buffer, blockSize);

			for(int j=0; j<blockSize; j++)
			{
					dirArray[(blockSize*(i + 12)) + j] = buffer[j];
			}
		}

		filestream.seekg((blockSize*currentInode.i_block[13]), ios_base::beg);
		filestream.read(buffer, blockSize);
		memcpy(&doubleIndirect, buffer, blockSize);
		int count = 12 + (blockSize/4);
		while(count < size)
		{
			int cnt = 0;

			for(int i=0; i<(blockSize/4); i++)
			{
				filestream.seekg((blockSize*doubleIndirect[i]), ios_base::beg);
				filestream.read(buffer, blockSize);
				memcpy(&singleIndirect, buffer, blockSize);

				for(int j=0; j<(blockSize/4); j++)
				{
					filestream.seekg((blockSize*singleIndirect[j]), ios_base::beg);
					filestream.read(buffer, blockSize);

					for(int k=0; k<blockSize; k++)
					{
						dirArray[(blockSize*count) + k] = buffer[k];
						count++;
					}		
				}
			}
		}
}

//function that gets triple indirect block
void ext2::fetchTripleIndirectBlock()
{
	if(IndirectType == 3)
	{
		for(int i=0; i<12; i++)
		{
			filestream.seekg((blockSize*currentInode.i_block[count]), ios_base::beg);
			filestream.read(buffer, blockSize);

			for(int j=0; j<blockSize; j++)
			{
					dirArray[(blockSize*i) + j] = buffer[j];
			}
		}

		filestream.seekg((blockSize*currentInode.i_block[12]), ios_base::beg);
		filestream.read(buffer, blockSize);
		memcpy(&singleIndirect, buffer, blockSize);

		for(int i=0; i<(blockSize/4); i++)
		{
			filestream.seekg((blockSize*singleIndirect[i]), ios_base::beg);
			filestream.read(buffer, blockSize);
			
			for(int j=0; j<blockSize; j++)
			{
					dirArray[(blockSize*(i + 12)) + j] = buffer[j];
			}
		}

		filestream.seekg((blockSize*currentInode.i_block[13]), ios_base::beg);
		filestream.read(buffer, blockSize);
		memcpy(&doubleIndirect, buffer, blockSize);
		int count =0;


		for(int i=0; i<(blockSize/4); i++)
		{
			filestream.seekg((blockSize*doubleIndirect[i]), ios_base::beg);
			filestream.read(buffer, blockSize);
			memcpy(&singleIndirect, buffer, blockSize);

			for(int j=0; j<(blockSize/4); j++)
			{
				filestream.seekg((blockSize*singleIndirect[j]), ios_base::beg);
				filestream.read(buffer, blockSize);

				for(int k=0; k<blockSize; k++)
				{
					dirArray[(blockSize*count) + k] = buffer[k];
					count++;
				}		
			}
		}

		filestream.seekg((blockSize*currentInode.i_block[14]), ios_base::beg);
		filestream.read(buffer, blockSize);
		memcpy(&tripleIndirect, buffer, blockSize);
		while(count < size)
		{
			for(int i=0; i<(blockSize/4); i++)
			{
				filestream.seekg((blockSize*tripleIndirect[i]), ios_base::beg);
				filestream.read(buffer, blockSize);
				memcpy(&doubleIndirect, buffer, blockSize);

				for(int j=0; j<(blockSize/4); j++)
				{
					filestream.seekg((blockSize*doubleIndirect[j]), ios_base::beg);
					filestream.read(buffer, blockSize);
					memcpy(&singleIndirect, buffer, blockSize);

					for(int k=0; k<(blockSize/4); k++)
					{
						filestream.seekg((blockSize*singleIndirect[k]), ios_base::beg);
						filestream.read(buffer,blockSize);

						for(int l=0; l<blockSize; l++)
						{
							dirArray[(blockSize*count) + l] = buffer[l];
							count++;
						}
					}
				}
			}
		}
	}
}

//function that gets the direct block
void ext2::fetchDirectBlock()
{
	
	if(IndirectType == 0)
	{
		for(int i=0; i<size; i++)
		{
			filestream.seekg((blockSize*currentInode.i_block[i]), ios_base::beg);
			filestream.read(buffer, blockSize);
			for(int j=0; j<blockSize; j++){dirArray[(blockSize*i) + j] = buffer[j];}
		}
	}
}


//function that gets the block
void ext2::fetchBlock()
{
	if(IndirectType == 0)
	{
		fetchDirectBlock();
	}
	if(IndirectType == 1)
	{
		fetchSingleIndirectBlock();
	}
	if(IndirectType == 2)
	{
		fetchDoubleIndirectBlock();
	}
	if(IndirectType == 3)
	{
		fetchTripleIndirectBlock();
	}

}


