#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

struct super_save{
  int magic_number;
  unsigned int inode_count;
  unsigned int block_count;
  unsigned long long block_size;
  unsigned long long fragment_size;
  unsigned int blocks_per_group;
  unsigned int inodes_per_group;
  unsigned int fragments_per_group;
  unsigned int first_data_block;
  unsigned int group_count;
};

struct group_save{
  unsigned int contained_blocks;
  unsigned int num_of_free_blocks;
  unsigned int num_of_free_inodes;
  unsigned int num_of_dir;
  unsigned int first_inode_bitmap_block;
  unsigned int first_block_bitmap_block;
  unsigned int first_inode_table_block;
};

struct super_save *s_save;
struct group_save *g_save;
unsigned int *used_inodes_num=NULL;
unsigned int used_inodes_num_len=0;
unsigned int used_inodes_num_index=0;
unsigned int *dir_first_data_block=NULL;
unsigned int dir_first_data_block_len=0;
unsigned int dir_first_data_block_index=0;

struct indirect_block{
  unsigned int first_level;
  unsigned int second_level;
  unsigned int third_level;
};

struct indirect_block *indirect_block_pt=NULL;
unsigned int indirect_block_pt_len=0;
unsigned int indirect_block_pt_index=0;



unsigned int *dir_inode=NULL;
unsigned int dir_inode_len=0;
unsigned int dir_inode_index=0;



void superblock(int fd){
  off_t st_off=1024;
  int buffer=0;

  s_save = (struct super_save *) malloc(sizeof(struct super_save));
  
  //getting magic number
  off_t magic_off= st_off+56;
  int magic_num=0;
  pread(fd,(void *) &magic_num,2,magic_off);
  s_save->magic_number = magic_num;
  
  //getting total number of inodes
  off_t in_co_off = st_off;
  unsigned int in_co=0;
  pread(fd,(void *) &in_co,4,in_co_off);
  s_save->inode_count = in_co;
  
  //getting total number of blocks
  off_t bl_co_off = st_off+4;
  unsigned int bl_co=0;
  pread(fd,(void *) &bl_co,4,bl_co_off);
  s_save->block_count=bl_co;
  
  //block size
  off_t bl_sz_off = st_off+24;
  pread(fd,(void *) &buffer,4,bl_sz_off);
  unsigned long long bl_sz = 1024 << buffer;
  s_save->block_size=bl_sz;
  
  //fragment size
  off_t fr_sz_off = st_off+28;
  pread(fd,(void *) &buffer,4,fr_sz_off);
  unsigned long long fr_sz = 0;
  if(buffer >= 0){
    fr_sz = 1024 << buffer;
  }
  else if (buffer < 0){
    fr_sz = 1024 >> (-buffer);
  }
  s_save->fragment_size=fr_sz;
  
  //blocks per group
  off_t bl_per_gr_off = st_off+32;
  unsigned int bl_per_gr=0;
  pread(fd,(void *) &bl_per_gr,4,bl_per_gr_off);
  s_save->blocks_per_group=bl_per_gr;
  
  //inodes per group
  off_t in_per_gr_off=st_off+40;
  unsigned int in_per_gr=0;
  pread(fd,(void *) &in_per_gr,4,in_per_gr_off);
  s_save->inodes_per_group=in_per_gr;
  
  //fragments per group
  off_t fr_per_gr_off=st_off+36;
  unsigned int fr_per_gr=0;
  pread(fd,(void *) &fr_per_gr,4,fr_per_gr_off);
  s_save->fragments_per_group=fr_per_gr;
  
  //first data block
  off_t first_data_off=st_off+20;
  unsigned int first_data=0;
  pread(fd,(void *) &first_data,4,first_data_off);
  s_save->first_data_block=first_data;
  
  char str_buffer[1024];
  memset(str_buffer,0,1024);
  sprintf(str_buffer,"%x,%u,%u,%llu,%llu,%u,%u,%u,%u\n",magic_num,in_co,bl_co,bl_sz,fr_sz,bl_per_gr,in_per_gr,fr_per_gr,first_data);

  /*
  int size_buffer = 0;
  int i =0;
  while((int) str_buffer[i++]!=0)size_buffer++;
  */ 
  
  int new_fd = creat("super.csv",0766);
  write(new_fd,str_buffer,strlen(str_buffer));
  close(new_fd);
}

void groupdescriptor(int fd){
  off_t st_off_gr_base = (1024/s_save->block_size + 1)*s_save->block_size;
  off_t size_group_desc=32;
  //off_t st_off = 1024;
  int new_fd = creat("group.csv",0766);

  //blocks per group
  /*
  off_t bl_per_gr_off = st_off+32;
  unsigned int bl_per_gr=0;
  pread(fd,(void *) &bl_per_gr,4,bl_per_gr_off);*/
  unsigned int bl_per_gr=s_save->blocks_per_group;

  //calculate how many groups by getting total block count first
  /*
  off_t bl_co_off = st_off+4;
  unsigned int bl_co=0;
  pread(fd,(void *) &bl_co,4,bl_co_off);*/
  unsigned int bl_co=s_save->block_count;

  unsigned int gp_co = 1+(bl_co-1)/bl_per_gr;
  s_save->group_count = gp_co;
  g_save = (struct group_save *) malloc(gp_co*sizeof(struct group_save));
  
  for(unsigned int i=0;i<gp_co;i++){
    off_t gr_off = st_off_gr_base + i*size_group_desc;

    //calculate blocks in this group
    unsigned int bl_this_gr = bl_per_gr;
    if(i+1==gp_co)bl_this_gr=bl_co-i*bl_per_gr;//revised @ Friday, get rid of the first data block offset
    g_save[i].contained_blocks=bl_this_gr;
    
    //get free blocks count
    unsigned int free_bl=0;
    pread(fd,(void *) &free_bl,2,gr_off+12);
    g_save[i].num_of_free_blocks=free_bl;
    
    //get free inodes count
    unsigned int free_in=0;
    pread(fd,(void *) &free_in,2,gr_off+14);
    g_save[i].num_of_free_inodes=free_in;
    
    //get dir counts
    unsigned int dir_co=0;
    pread(fd,(void *) &dir_co,2,gr_off+16);
    g_save[i].num_of_dir=dir_co;
    
    //The following two might be changed
    //get first inode bitmap block and first block bitmap block
    unsigned int first_in_bl=0;
    pread(fd,(void *) &first_in_bl,4,gr_off+4);
    g_save[i].first_inode_bitmap_block=first_in_bl;
    unsigned int first_bl_bl=0;
    pread(fd,(void *) &first_bl_bl,4,gr_off);
    g_save[i].first_block_bitmap_block=first_bl_bl;
    //get start of inode table block
    unsigned int first_in_ta_bl=0;
    pread(fd,(void *) &first_in_ta_bl,4,gr_off+8);
    g_save[i].first_inode_table_block=first_in_ta_bl;
    
    char str_buffer[1024];
    memset(str_buffer,0,1024);
    sprintf(str_buffer,"%u,%u,%u,%u,%x,%x,%x\n",bl_this_gr,free_bl,free_in,dir_co,first_in_bl,first_bl_bl,first_in_ta_bl);
    /*
    int size_buffer = 0;
    int i =0;
    while((int) str_buffer[i++]!=0)size_buffer++;
    */
    write(new_fd,str_buffer,strlen(str_buffer));
    //close(new_fd);
    
  }
  close(new_fd);
}

unsigned int *grow_unsigned_array(unsigned int *list,unsigned int size){
  unsigned int *new_list = (unsigned int *) realloc(list,size*sizeof(unsigned int));
  return new_list;
}

void free_bitmap(int fd){
  int new_fd = creat("bitmap.csv",0766);
  used_inodes_num=grow_unsigned_array(used_inodes_num,256);
  used_inodes_num_len=256;
  unsigned int bl_sz = s_save->block_size;
  unsigned int gp_co = s_save->group_count;
  unsigned int mask = 1;
  /*
  unsigned int masks[8];
  masks[0]=1;
  for(int i =1;i<8;i++){
    masks[i]=masks[i-1]*2;
  }
  */
  //iterate through each group
  for(unsigned int i=0;i<gp_co;i++){
    unsigned int bl_this_gr = g_save[i].contained_blocks;
    unsigned int in_this_gr = s_save->inodes_per_group;
    if(i+1==gp_co){
      unsigned int remainder = s_save->inode_count%s_save->inodes_per_group;
      in_this_gr = (remainder  == 0 ? s_save->inodes_per_group : remainder); 
    }
    unsigned int block_bitmap_block = g_save[i].first_block_bitmap_block;
    unsigned int inode_bitmap_block = g_save[i].first_inode_bitmap_block;
    //fprintf(stdout,"Doing inode:%u we need to iterate %u \n",inode_bitmap_block,in_this_gr);
    //calculate out the starting offset for bitmap blocks
    off_t block_start = bl_sz * block_bitmap_block;
    off_t inode_start = bl_sz * inode_bitmap_block;

    unsigned int buffer=0;
    
    for(unsigned int j = 0; j<bl_this_gr;j+=8){
      //if(j>=bl_sz*8)block_bitmap_block++;
      //read in one byte at time/8bits
      pread(fd,(void *) &buffer,1,block_start++);
      //for(unsigned int j=i;j<i+8;j++){
      //need to consider the case j+8 > bl_this_gr but still need to pull some bits
      int k = 0;
      while(k<8){
	//once we reach a blocksize, increase the block number
	if(j+k!=0&&(j+k)%(8*bl_sz)==0)block_bitmap_block++;
	if(j+k>=bl_this_gr)break;//if the bits exceed the total bits we need to check
	//apply mask to check if the relevent block is free
	if((buffer&mask)==0){
	  unsigned int block_num = i*s_save->blocks_per_group+j+k+s_save->first_data_block;//calulate block number

	  char str_buffer[128];
	  memset(str_buffer,0,128);
	  sprintf(str_buffer,"%x,%u\n",block_bitmap_block,block_num);
	  //int size_buffer = 0;
	  //int i =0;
	  //while((int) str_buffer[i++]!=0)size_buffer++;   
	  write(new_fd,str_buffer,strlen(str_buffer));
	}
	
	buffer=buffer>>1;
	k++;
      }
      buffer=0;
    }

    buffer=0;
    for(unsigned int j=0;j<in_this_gr;j+=8){
      pread(fd,(void *) &buffer,1,inode_start++);
      //if(i==0)fprintf(stdout,"inode block 4 buffer value:%u \n",buffer);
      int k =0;
      while(k<8){
	if(j+k!=0&&(j+k)%(8*bl_sz)==0)inode_bitmap_block++;
	if(j+k>=in_this_gr)break;
	if((buffer&mask)==0){
	  unsigned int inode_num = i*s_save->inodes_per_group+j+k+1;
	  
	  char str_buffer[128];
	  memset(str_buffer,0,128);
	  sprintf(str_buffer,"%x,%u\n",inode_bitmap_block,inode_num);
	  //int size_buffer = 0;
	  //int i =0;
	  //while((int) str_buffer[i++]!=0)size_buffer++;   
	  write(new_fd,str_buffer,strlen(str_buffer));
	}
	else{
	  if(used_inodes_num_index>=used_inodes_num_len){
	    used_inodes_num_len*=2;
	    used_inodes_num = grow_unsigned_array(used_inodes_num,used_inodes_num_len);
	  }
	  used_inodes_num[used_inodes_num_index++]=i*s_save->inodes_per_group+j+k+1;
	  
	}
	buffer=buffer>>1;
	k++;
      }
      buffer=0;
    }

        
  }

  close(new_fd);
}


void inode(int fd){
  char *str_buffer;
  int new_fd = creat("inode.csv",0766);

  dir_first_data_block_len=256;
  dir_first_data_block = (unsigned int *) malloc(dir_first_data_block_len*sizeof(unsigned int));
  indirect_block_pt_len=128;
  indirect_block_pt = (struct indirect_block *) malloc(indirect_block_pt_len*sizeof(struct indirect_block));
  dir_inode_len = 256;
  dir_inode = (unsigned int *) malloc(dir_inode_len*sizeof(unsigned int));

  
  for(unsigned int i=0;i<used_inodes_num_index;i++){
    unsigned int used_inodes = used_inodes_num[i];
    unsigned int gr_inodes_index = (used_inodes-1)/s_save->inodes_per_group;
    unsigned int start_inode_block = g_save[gr_inodes_index].first_inode_table_block;
    off_t start_off = s_save->block_size * start_inode_block + ((used_inodes-1)%s_save->inodes_per_group)*128;

    unsigned int buffer=0;
    //get i_mode and calculate file_type and protection mode
    pread(fd,(void *) &buffer,2,start_off);
    unsigned int mask_format = 0xf000;
    unsigned int result = buffer & mask_format;
    char fl_type;
    if(result == 0xA000)fl_type='s';
    else if(result == 0x8000)fl_type='f';
    else if(result == 0x4000)fl_type='d';
    else fl_type='?';
    //unsigned int mask_mode = 0x01ff;
    unsigned int mode = buffer;
    //unsigned int mode = buffer & mask_mode;//change to full mode entry @ Friday

    //get owner id
    unsigned int ow_id = 0;
    pread(fd,(void *) &ow_id,2,start_off+2);
    unsigned int hi_ow_id = 0;
    pread(fd,(void *) &hi_ow_id,2,start_off+116+4);
    ow_id = (hi_ow_id<<16)+ow_id;
    
    //get group id
    unsigned int gr_id = 0;
    pread(fd,(void *) &gr_id,2,start_off+24);
    unsigned int hi_gr_id = 0;
    pread(fd,(void *) &hi_gr_id,2,start_off+116+6);
    gr_id = (hi_gr_id<<16)+gr_id;
    
    //get link count
    unsigned int lk_co = 0;
    pread(fd,(void *) &lk_co,2,start_off+26);

    //get creation time
    unsigned int ctime = 0;
    pread(fd,(void *) &ctime,4,start_off+12);
    
    //get modification time
    unsigned int mtime = 0;
    pread(fd,(void *) &mtime,4,start_off+16);

    //get access time
    unsigned int atime = 0;
    pread(fd,(void *) &atime,4,start_off+8);
    
    //get file size
    unsigned int fl_size = 0;
    pread(fd,(void *) &fl_size,4,start_off+4);
    
    //get number of blocks--with per block size in the superblock
    unsigned int block_512 = 0;
    pread(fd,(void *) &block_512,4,start_off+28);
    unsigned int block_num = block_512/(s_save->block_size/512);
    
    //get 15 block pointers
    unsigned int blocks_pt[15];
    for(unsigned int z = 0; z<15;z++){
      blocks_pt[z]=0;
      pread(fd,(void *) &blocks_pt[z],4,start_off+40+z*4);
      if(fl_type=='d'&&blocks_pt[z]!=0){
	if(z<12){
	  if(dir_first_data_block_index>=dir_first_data_block_len){
	    dir_first_data_block_len*=2;
	    dir_first_data_block = grow_unsigned_array(dir_first_data_block,dir_first_data_block_len);
	  }
	  dir_first_data_block[dir_first_data_block_index++]=blocks_pt[z];
	  if(dir_inode_index>=dir_inode_len){
	    dir_inode_len*=2;
	    dir_inode = grow_unsigned_array(dir_inode,dir_inode_len);
	  }
	  dir_inode[dir_inode_index++]=used_inodes;
	}
	else if(z==12){
	  off_t indir_bl  = s_save->block_size * blocks_pt[z];
	  for(unsigned int it=0;it<s_save->block_size/4;it++){
	    unsigned int bl = 0;
	    pread(fd,(void *) &bl,4,indir_bl+it*4);
	    if(bl!=0){
	      if(dir_first_data_block_index>=dir_first_data_block_len){
		dir_first_data_block_len*=2;
		dir_first_data_block = grow_unsigned_array(dir_first_data_block,dir_first_data_block_len);
	      }
	      dir_first_data_block[dir_first_data_block_index++]=blocks_pt[z];
	      if(dir_inode_index>=dir_inode_len){
		dir_inode_len*=2;
		dir_inode = grow_unsigned_array(dir_inode,dir_inode_len);
	      }
	      dir_inode[dir_inode_index++]=used_inodes;
	    }
	  }
	}
	else if(z==13){
	  off_t indir_bl_db = s_save->block_size * blocks_pt[z];
	  for(unsigned int it = 0;it<s_save->block_size/4;it++){
	    unsigned int bl_db = 0;
	    pread(fd,(void *) &bl_db,4,indir_bl_db+it*4);
	    if(bl_db!=0){
	      off_t indir_bl = s_save->block_size * bl_db;
	      for(unsigned int it_2 = 0;it_2<s_save->block_size/4;it_2++){
		unsigned int bl = 0;
		pread(fd,(void *) &bl,4,indir_bl+4*it_2);
		if(bl!=0){
		  if(dir_first_data_block_index>=dir_first_data_block_len){
		    dir_first_data_block_len*=2;
		    dir_first_data_block = grow_unsigned_array(dir_first_data_block,dir_first_data_block_len);
		  }
		  dir_first_data_block[dir_first_data_block_index++]=blocks_pt[z];
		  if(dir_inode_index>=dir_inode_len){
		    dir_inode_len*=2;
		    dir_inode = grow_unsigned_array(dir_inode,dir_inode_len);
		  }
		  dir_inode[dir_inode_index++]=used_inodes;
		}
	      }
	    }
	  }
	}
	else if(z==14){
	  off_t indir_bl_db = s_save->block_size * blocks_pt[z];
	  for(unsigned int it = 0;it<s_save->block_size/4;it++){
	    unsigned int bl_db = 0;
	    pread(fd,(void *) &bl_db,4,indir_bl_db+it*4);
	    if(bl_db!=0){
	      off_t indir_bl_tr = s_save->block_size * bl_db;
	      for(unsigned int it_2 = 0;it_2<s_save->block_size/4;it_2++){
		unsigned int bl_tr = 0;
		pread(fd,(void *) &bl_tr,4,indir_bl_tr+4*it_2);
		if(bl_tr!=0){
		  off_t indir_bl = s_save->block_size * bl_tr;
		  for(unsigned int it_3=0;it_3<s_save->block_size/4;it_3++){
		    unsigned int bl = 0;
		    pread(fd,(void *) &bl,4,indir_bl+4*it_3);
		    if(bl!=0){
		      if(dir_first_data_block_index>=dir_first_data_block_len){
			dir_first_data_block_len*=2;
			dir_first_data_block = grow_unsigned_array(dir_first_data_block,dir_first_data_block_len);
		      }
		      dir_first_data_block[dir_first_data_block_index++]=blocks_pt[z];
		      if(dir_inode_index>=dir_inode_len){
			dir_inode_len*=2;
			dir_inode = grow_unsigned_array(dir_inode,dir_inode_len);
		      }
		      dir_inode[dir_inode_index++]=used_inodes;
		    }
		  }
		}
	      }
	    }
	  }
	}

	
      }
      if(z==14){
	if(indirect_block_pt_index>=indirect_block_pt_len){
	  indirect_block_pt_len*=2;
	  indirect_block_pt = (struct indirect_block *) realloc(indirect_block_pt,indirect_block_pt_len*sizeof(struct indirect_block));
	}
	indirect_block_pt[indirect_block_pt_index].first_level=blocks_pt[z-2];
	indirect_block_pt[indirect_block_pt_index].second_level=blocks_pt[z-1];
	indirect_block_pt[indirect_block_pt_index++].third_level=blocks_pt[z];
      }
    }

    //write out to file
    str_buffer = (char *) malloc(2048);
    memset(str_buffer,0,2048);

    sprintf(str_buffer,"%u,%c,%o,%u,%u,%u,%x,%x,%x,%u,%u,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\n",used_inodes,fl_type,mode,ow_id,gr_id,lk_co,ctime,mtime,atime,fl_size,block_num,blocks_pt[0],blocks_pt[1],blocks_pt[2],blocks_pt[3],blocks_pt[4],blocks_pt[5],blocks_pt[6],blocks_pt[7],blocks_pt[8],blocks_pt[9],blocks_pt[10],blocks_pt[11],blocks_pt[12],blocks_pt[13],blocks_pt[14]);
    write(new_fd,str_buffer,strlen(str_buffer));
    
  }

  free(str_buffer);
  close(new_fd);

  //for(unsigned int i  = 0 ; i < indirect_block_pt_index; i++){
  //fprintf(stdout,"first:%u second:%u third:%u \n",indirect_block_pt[i].first_level,indirect_block_pt[i].second_level,indirect_block_pt[i].third_level);
  //}
  
}

//Question can directory have indirect block??
void directory_entry(int fd){

  int new_fd = creat("directory.csv",0766);
  //fprintf(stdout,"new_fd: %d\n",new_fd);
  //return;
  char str_buffer[1024];
  char name[257];
  unsigned int entry_num = 0;
  for(unsigned int i=0;i<dir_first_data_block_index;i++){
    
    unsigned int parent_inode = dir_inode[i];
    unsigned int first_block = dir_first_data_block[i];
    off_t data_off = s_save->block_size * first_block;
    off_t data_dest = s_save->block_size + data_off;
    
    if(i>=1&&dir_inode[i]!=dir_inode[i-1])entry_num=0;
    unsigned int inode_num = 0;
    
    do{
      //get entry length;
      unsigned int entry_len = 0;
      pread(fd,(void *) &entry_len,2,data_off+4);

      //get name length
      unsigned int name_len = 0;
      pread(fd,(void *) &name_len,1,data_off+6);
      //if(name_len==0)break;
      
      //get inode number
      pread(fd,(void *) &inode_num,4,data_off);
      //if(inode_num==0)continue;
      
      //get name
      memset(name,0,257);
      pread(fd,(void *) name,name_len,data_off+8);

      //write to file
      if(inode_num!=0){
	memset(str_buffer,0,1024);
	sprintf(str_buffer,"%u,%u,%u,%u,%u,\"%s\"\n",parent_inode,entry_num,entry_len,name_len,inode_num,name);
	//printf("new_fd:%d\n",new_fd);
	write(new_fd,str_buffer,strlen(str_buffer));
      }
      //increment offset and entry number
      data_off += entry_len;
      entry_num++;

      
    }while(data_off<data_dest);
    


  }

  close(new_fd);
  
}

void indirect(int fd){

  int new_fd = creat("indirect.csv",0766);

  char str_buffer[128];
  for(unsigned int z=0;z<indirect_block_pt_index;z++){
    unsigned int first_level = indirect_block_pt[z].first_level;
    unsigned int second_level = indirect_block_pt[z].second_level;
    unsigned int third_level = indirect_block_pt[z].third_level;

    
    unsigned int block;
    unsigned int block_entry = 0;

    //fprintf(stdout,"z: %u \n",z);
    
    if(first_level!=0){
      off_t first_off = s_save->block_size * first_level;
      while(block_entry*4<s_save->block_size){
	pread(fd,(void *) &block,4,first_off+block_entry*4);
	if(block==0){
	  block_entry++;
	  continue;
	}
	memset(str_buffer,0,128);
	sprintf(str_buffer,"%x,%u,%x\n",first_level,block_entry,block);
	write(new_fd,str_buffer,strlen(str_buffer));
	block_entry++;
	//printf("asdasd\n");
      }
    }

    unsigned int *second_in=NULL;
    if(second_level!=0){
      //first_level
      off_t second_off = s_save->block_size * second_level;
      second_in = (unsigned int *) calloc(s_save->block_size/4,sizeof(unsigned int));
      block_entry = 0;
      while(block_entry*4<s_save->block_size){
	pread(fd,(void *) &block,4,second_off+block_entry*4);
	second_in[block_entry]=block;
	if(block==0){
	  block_entry++;
	  continue;
	}
	memset(str_buffer,0,128);
	sprintf(str_buffer,"%x,%u,%x\n",second_level,block_entry,block);
	write(new_fd,str_buffer,strlen(str_buffer));
	block_entry++;
      }
      //second_level
      for(unsigned int i = 0; i<s_save->block_size/4;i++){
	if(second_in[i]==0)continue;
	block_entry=0;
	off_t second_in_off = s_save->block_size * second_in[i];
	while(block_entry*4<s_save->block_size){
	  pread(fd,(void *) &block,4,second_in_off+block_entry*4);
	  if(block==0){
	    block_entry++;
	    continue;
	  }
	  memset(str_buffer,0,128);
	  sprintf(str_buffer,"%x,%u,%x\n",second_in[i],block_entry,block);
	  write(new_fd,str_buffer,strlen(str_buffer));
	  block_entry++;
	}
      }

      
    }
    //printf("hello\n");
    unsigned int **third_in=NULL;
    if(second_in != NULL){
      for(unsigned int i=0;i<s_save->block_size/4;i++){
	second_in[i]=0;
      }
    }
    if(third_level!=0){
      //first_level unfold
      off_t third_off = s_save->block_size * third_level;
      block_entry=0;
      while(block_entry*4<s_save->block_size){
	pread(fd,(void *) &block,4,third_off+block_entry*4);
	second_in[block_entry]=block;
	if(block==0){
	  block_entry++;
	  continue;
	}
	memset(str_buffer,0,128);
	sprintf(str_buffer,"%x,%u,%x\n",third_level,block_entry,block);
	write(new_fd,str_buffer,strlen(str_buffer));
	block_entry++;
      }

      //unfold second level
      third_in = (unsigned int **) malloc(s_save->block_size/4*sizeof(unsigned int *));
      for(unsigned int i=0;i<s_save->block_size/4;i++){
	if(second_in[i]==0)continue;
	block_entry=0;
	off_t second_in_off = s_save->block_size * second_in[i];
	third_in[i] = calloc(s_save->block_size/4,sizeof(unsigned int));
	
	while(block_entry*4<s_save->block_size){
	  pread(fd,(void *) &block,4,second_in_off+block_entry*4);
	  third_in[i][block_entry]=block;
	  if(block==0){
	    block_entry++;
	    continue;
	  }
	  memset(str_buffer,0,128);
	  sprintf(str_buffer,"%x,%u,%x\n",second_in[i],block_entry,block);
	  write(new_fd,str_buffer,strlen(str_buffer));
	  block_entry++;
	}
      }

      //unfold the third level
      for(unsigned int i=0;i<s_save->block_size/4;i++){
	for(unsigned int j=0;j<s_save->block_size/4;j++){
	  if(third_in[i][j]==0)continue;
	  block_entry=0;
	  off_t third_in_off = s_save->block_size * third_in[i][j];
	  while(block_entry*4<s_save->block_size){
	    pread(fd,(void *) &block,4,third_in_off+block_entry*4);
	    if(block==0){
	      block_entry++;
	      continue;
	    }
      
	    memset(str_buffer,0,128);
	    sprintf(str_buffer,"%x,%u,%x\n",third_in[i][j],block_entry,block);
	    write(new_fd,str_buffer,strlen(str_buffer));
	    block_entry++;
	  }

	}	
      }
    }
    
  }

  close(new_fd);
}



int main(int argc, char *argv[]){

  int fd;
  if(argc!=2){
    fprintf(stderr,"The number of arguments should be 2.\n");
  }
  else{
    //fprintf(stdout,"Opening file system %s \n",argv[1]);
    fd = open(argv[1],O_RDONLY);
    if(fd<0){
      fprintf(stderr,"Open failed.\n");
      exit(1);
    }
  }

  superblock(fd);
  groupdescriptor(fd);
  free_bitmap(fd);
  inode(fd);
  directory_entry(fd);
  indirect(fd);

  //fprintf(stdout,"indirect: %d\n",indirect_block_pt_index);
  //for(unsigned int i=0;i<indirect_block_pt_index;i++){
  //fprintf(stdout,"%x,%x,%x\n",indirect_block_pt[i].first_level,indirect_block_pt[i].second_level,indirect_block_pt[i].third_level);
  //}
  
  //for(unsigned int i=0;i<dir_first_data_block_index;i++){
  //fprintf(stdout,"directory inode: %u directory data block: %u \n",dir_inode[i],dir_first_data_block[i]);
  //}

  
}
