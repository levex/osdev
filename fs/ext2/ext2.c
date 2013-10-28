/** @author Levente Kurusa <levex@linux.com> **/
#include "../../include/ext2.h"
#include "../../include/display.h"
#include "../../include/device.h"
#include "../../include/memory.h"

MODULE("EXT2");

void ext2_read_block(uint8_t *buf, uint32_t block, device_t *dev, ext2_priv_data *priv)
{
	uint32_t sectors_per_block = priv->sectors_per_block;
	if(!sectors_per_block) sectors_per_block = 1;
	dev->read(buf, block*sectors_per_block, sectors_per_block);

}
char *block_buf = 0;
void ext2_read_inode(inode_t *inode_buf, uint32_t inode, device_t *dev, ext2_priv_data *priv)
{
	uint32_t bg = (inode - 1) / priv->sb.inodes_in_blockgroup;
	int i = 0;
	/* Now we have which BG the inode is in, load that desc */
	if(!block_buf) block_buf = (char *)malloc(priv->blocksize);
	ext2_read_block(block_buf, priv->sb.superblock_id + 1, dev, priv);
	block_group_desc_t *bgd = (block_group_desc_t*)block_buf;
	/* Seek to the BG's desc */
	for(i = 0; i < bg; i++)
		bgd++;
	/* Find the index and seek to the inode */
	uint32_t index = (inode - 1) % priv->sb.inodes_in_blockgroup;
	uint32_t block = (index * sizeof(inode_t))/ priv->blocksize;
	ext2_read_block(block_buf, bgd->block_of_inode_table + block, dev, priv);
	inode_t* _inode = (inode_t *)block_buf;
	index = index % priv->inodes_per_block;
	for(i = 0; i < index; i++)
		_inode++;
	/* We have found the inode! */
	memcpy(inode_buf, _inode, sizeof(inode_t));
}

inode_t *inode = 0;
char *root_buf = 0;
uint8_t ext2_read_directory(char *filename, ext2_dir *dir, device_t *dev, ext2_priv_data *priv)
{
	while(dir->inode != 0) {
		char *name = (char *)malloc(dir->namelength + 1);
		memcpy(name, &dir->reserved+1, dir->namelength);
		name[dir->namelength] = 0;
		if(filename && strcmp(filename, name) == 0)
		{
			/* If we are looking for a file, we had found it */
			ext2_read_inode(inode, dir->inode, dev, priv);
			mprint("Found inode %s! %d\n", filename, dir->inode);
			free(name);
			return 1;
		}
		if(!filename && filename != 1) {
			//mprint("Found dir entry: %s to inode %d \n", name, dir->inode);
			kprintf("%s\n", name);
		}
		dir = (ext2_dir *)((uint32_t)dir + dir->size);
		free(name);
	}
	return 0;
}

uint8_t ext2_read_root_directory(char *filename, device_t *dev, ext2_priv_data *priv)
{
	/* The root directory is always inode#2, so find BG and read the block. */
	if(!inode) inode = (inode_t *)malloc(sizeof(inode_t));
	if(!root_buf) root_buf = (char *)malloc(priv->blocksize);
	ext2_read_inode(inode, 2, dev, priv);
	if((inode->type & 0xF000) != INODE_TYPE_DIRECTORY)
	{
		mprint("FATAL: Root directory is not a directory!\n");
		return 0;
	}
	/* We have found the directory!
	 * Now, load the starting block
	 */
	for(int i = 0;i < 12; i++)
	{
		uint32_t b = *(uint32_t*)(&inode->dbp0 + i*4);
		if(b == 0) break;
		ext2_read_block(root_buf, b, dev, priv);
		/* Now loop through the entries of the directory */
		if(ext2_read_directory(filename, (ext2_dir*)root_buf, dev, priv)) return 1;
	}
	if(filename && filename != 1) return 0;
	return 1;
}

uint8_t ext2_find_file_inode(char *ff, inode_t *inode_buf, device_t *dev, ext2_priv_data *priv)
{
	char *filename = malloc(strlen(ff) + 1);
	memcpy(filename, ff, strlen(ff) +1);
	size_t n = strsplit(filename, '/');
	filename ++; // skip the first crap
	if(n > 1)
	{ 
		/* Read inode#2 (Root dir) into inode */
		ext2_read_inode(inode, 2, dev, priv);
		/* Now, loop through the DPB's and see if it contains this filename */
		n--;
		while(n--)
		{
			mprint("Looking for: %s\n", filename);
			for(int i = 0; i < 12; i++)
			{
				uint32_t b = *(uint32_t*)(&inode->dbp0 + i*4);
				if(!b) break;
				ext2_read_block(root_buf, b, dev, priv);
				if(!ext2_read_directory(filename, root_buf, dev, priv))
				{
					if(strcmp(filename, "") == 0)
					{
						free(filename);
						return 1;
					}
					mprint("File (%s (0x%x)) not found!\n", filename, filename);
					free(filename);
					return 0;
				} else {
					/* inode now contains that inode
					 * get out of the for loop and continue traversing
					 */
					 goto fix;
				}
			}
			fix:;
			uint32_t s = strlen(filename);
			filename += s + 1;
		}
		memcpy(inode_buf, inode, sizeof(inode_t));
	} else {
		/* This means the file is in the root directory */
		ext2_read_root_directory(filename, dev, priv);
		memcpy(inode_buf, inode, sizeof(inode_t));
	}
	free(filename);
	return 1;
}

void ext2_list_directory(char *dd, char *buffer, device_t *dev, ext2_priv_data *priv)
{
	char *dir = dd;
	int rc = ext2_find_file_inode(dir, buffer, dev, priv);
	if(!rc) return;
	for(int i = 0;i < 12; i++)
	{
		uint32_t b = *(uint32_t *)(&inode->dbp0 + i*4);
		if(!b) break;
		ext2_read_block(root_buf, b, dev, priv);
		ext2_read_directory(0, root_buf, dev, priv);
	}
}

uint8_t ext2_read_file(char *fn, char *buffer, device_t *dev, ext2_priv_data *priv)
{
	/* Put the file's inode to the buffer */
	char *filename = fn;
	if(!ext2_find_file_inode(filename, buffer, dev, priv))
	{
		return 0;
	}
	if(buffer)
	{
		inode_t *minode = (inode_t *)buffer;
		for(int i = 0; i < 12; i++)
		{
			uint32_t b = *(uint32_t*)(&minode->dbp0 + i*4);
			if(b == 0) break;
			mprint("Reading block: %d\n", b);
			ext2_read_block(root_buf, b, dev, priv);
			memcpy(buffer + i*priv->blocksize, root_buf, priv->blocksize);
		}
	}
	return 1;
}

uint8_t ext2_exist(char *file, device_t *dev, ext2_priv_data *priv)
{
	return ext2_read_file(file, 0, dev, priv);
}

uint8_t ext2_probe(device_t *dev)
{
	mprint("Probing device %d\n", dev->unique_id);
	/* Read in supposed superblock location and check sig */
	if(!dev->read)
	{
		kprintf("Device has no read, skipped.\n");
		return 0;
	}
	char *buf = (char *)malloc(512);
	dev->read(buf, 2, 1);
	superblock_t *sb = (superblock_t *)buf;
	if(sb->ext2_sig != EXT2_SIGNATURE)
	{
		kprintf("Invalid EXT2 signature!\n");
		return 0;
	}
	mprint("Valid EXT2 signature!\n");
	filesystem_t *fs = (filesystem_t *)malloc(sizeof(filesystem_t));
	ext2_priv_data *priv = (ext2_priv_data *)malloc(sizeof(ext2_priv_data));
	memcpy(&priv->sb, sb, sizeof(superblock_t));
	/* Calculate volume length */
	uint32_t blocksize = 1024 << sb->blocksize_hint;
	mprint("Size of a block: %d bytes\n", blocksize);
	priv->blocksize = blocksize;
	priv->inodes_per_block = blocksize / sizeof(inode_t);
	priv->sectors_per_block = blocksize / 512;
	mprint("Size of volume: %d bytes\n", blocksize*(sb->blocks));
	/* Calculate the number of block groups */
	uint32_t number_of_bgs0 = sb->blocks / sb->blocks_in_blockgroup;
	if(!number_of_bgs0) number_of_bgs0 = 1;
	mprint("There are %d block group(s).\n", number_of_bgs0);
	priv->number_of_bgs = number_of_bgs0;
	/* Now, we have the size of a block,
	 * calculate the location of the Block Group Descriptor
	 * The BGDT is located directly after the SB, so obtain the
	 * block of the SB first. This is located in the SB.
	 */
	uint32_t block_bgdt = sb->superblock_id + 1;
	char *buffer = (char *)malloc(blocksize);
	ext2_read_block(buffer, block_bgdt, dev, priv);
	block_group_desc_t *bgd = (block_group_desc_t *)buffer;
	priv->first_bgd = block_bgdt;
	fs->name = "EXT2";
	fs->probe = ext2_probe;
	fs->mount = ext2_mount;
	fs->read = ext2_read_file;
	fs->exist = ext2_exist;
	fs->read_dir = ext2_list_directory;
	fs->priv_data = (void *)priv;
	dev->fs = fs;
	mprint("Device %s (%d) is with EXT2 filesystem. Probe successful.\n", dev->name, dev->unique_id);
	free(buf);
	free(buffer);
	return 1;
}
uint8_t ext2_mount(device_t *dev, void *privd)
{
	mprint("Mounting ext2 on device %s (%d)\n", dev->name, dev->unique_id);
	ext2_priv_data *priv = privd;
	if(ext2_read_root_directory(1, dev, priv))
		return 1;
	return 0;
}