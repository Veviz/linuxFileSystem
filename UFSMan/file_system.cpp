#include "file_system.h"


FILE *file;            //�ļ�ָ�룬���ڸ��ֶ��ļ��Ĳ���
char file_system_name[20];           //�����Ĵ����ļ���
super_block_t  file_system_super;    //�����ṹ�����
char inode_bitmap[INODE_NUM/8];         //�����ڵ�λʾͼ����λ��=8(char)*16=128bit,����һ�����Ա�ʾ128�������ڵ�
char data_bitmap[DATA_BLOCK_NUM/8];    //���ݿ�λʾͼ����λ��=8(char)*64=512bit,����һ�����Ա�ʶ521�����ݿ�
directory dir_buf[BLOCK_SIZE/sizeof(directory)];   //Ŀ¼���飬��ʾһ�����̿���Դ��32��Ŀ¼��
struct inode_t iNode[BLOCK_SIZE/sizeof(inode_t)];                              //�����ڵ�


char cmdHead[50];                     //��¼��ǰ���Ǹ��ļ�����
int cur_inode_no;                     //���ص�ǰ���ڵ�Ŀ¼�������ڵ�
int spare_datablk_num[512];           //��¼�������ݿ�Ŀ�ţ����ڷ������ݿ�
int index[128];                             //��ַʱ������Ѱַ���м���̿�

char Buffer[1048576];                //1M�Ļ����ڴ�ռ�
char Block_Data[513];                //���ڴ��һ�����̿��е����ݵ���ʱ�ռ�

bool Format()         //��ʽ�������ļ�����ʼ�����е�����
{
	int i;
	file = fopen(file_system_name,"wb");//������������ļ�
	if(NULL == file)
	{
		printf("������������ļ�ʧ�ܣ�");
		return false;
	}

	//��ʼ��������
	file_system_super.magic = 0;
	file_system_super.total_block_count = TOTAL_BLOCK_NUMBER;          //�ļ�ϵͳ���ܴ�����
	file_system_super.inode_count = INODE_NUM;                         //�ļ�ϵͳ���������ڵ���
	file_system_super.data_block_count = DATA_BLOCK_NUM;               //�ļ�ϵͳ�������ݿ���Ŀ

	file_system_super.inode_table_bitmap.start_block = INODE_BITMAP_START;                //�����ڵ�λʾͼ��ʼ���̿�
	file_system_super.inode_table_bitmap.block_count = INODE_BITMAP_BKNUM;                //�����ڵ�λʾͼռ�ݴ��̿���
	file_system_super.inode_table_bitmap.byte_count = INODE_NUM/8;                       //�����ڵ�λʾͼռ�õ��ֽڸ��� 

	file_system_super.inode_table.start_block = INODE_BLOCK_START;                       //�����ڵ㿪ʼ���̿�
	file_system_super.inode_table.block_count = INODE_BLOCK_NUM;                         //�����ڵ�ռ�ݴ��̿���
	file_system_super.inode_table.byte_count = INODE_SIZE * INODE_NUM_PER_BLOCK * INODE_BLOCK_NUM;//�����ڵ�ռ�õ��ֽڸ��� 

	file_system_super.data_area_bitmap.start_block = DATA_BITMAP_START;                //���ݿ�λʾͼ��ʼ���̿�
	file_system_super.data_area_bitmap.block_count = DATA_BITMAP_BLNUM;                //���ݿ�λʾͼռ�ݴ��̿���
	file_system_super.data_area_bitmap.byte_count = DATA_BLOCK_NUM/8;                  //���ݿ�λʾͼռ�õ��ֽڸ���

	file_system_super.data_area.start_block = DATA_BLOCK_START;                //���ݿ鿪ʼ���̿�
	file_system_super.data_area.block_count = DATA_BLOCK_NUM;                 //���ݿ�ռ�ݴ��̿���
	file_system_super.data_area.byte_count = DATA_BLOCK_NUM * BLOCK_SIZE;     //���ݿ�ռ�õ��ֽڸ���

	fseek(file,0L,SEEK_SET);
	fwrite(&file_system_super,BLOCK_SIZE,1,file);

	//��ʼ�������ڵ�λʾͼ
	inode_bitmap[0] = 0x80;            //�����ڵ�λʾͼ�ĵ�һλ��ʶ��Ŀ¼,0x80��ʾ100000000
	for(i = 1;i < INODE_NUM/8;i ++)
	{
		inode_bitmap[i] = 0;
	}
	fseek(file,BLOCK_SIZE,SEEK_SET);
	fwrite(inode_bitmap,sizeof(inode_bitmap),1,file);

	//��ʼ�������ڵ㣬��ʱֻ��һ�������ڵ㣬����Ŀ¼�������ڵ�
	//��Ӧ�ĸ�Ŀ¼���������ݿ�����ĵ�һ�����ݿ���
	iNode[0].type = 0;                 //��ʾ���ļ���Ŀ¼�ļ�
	iNode[0].size = 0;                 //Ŀ¼�ļ���size����0
	iNode[0].disk_address[0] = 0+DATA_BLOCK_START;      //��Ŀ¼��ռ�ݵ����ݿ��<���ݿ�Ŵ�0#��511#>����Ӧ�ڴ��̺Ż�Ҫ����19
	fseek(file,BLOCK_SIZE*2,SEEK_SET);
	fwrite(&iNode,sizeof(iNode),1,file);

	//��ʼ�����ݿ�λʾͼ
	data_bitmap[0] = 0x80;          //���ݿ�λʾͼ�ĵ�һλҲ��ռ�ã����ڴ�Ÿ�Ŀ¼��
	for(i = 1; i < DATA_BLOCK_NUM/8; i ++)
	{
		data_bitmap[i] = 0;
	}
	fseek(file,BLOCK_SIZE*18,SEEK_SET);
	fwrite(data_bitmap,BLOCK_SIZE,1,file);

	//��ʼ�����ݿ飬��ʱֻ�е�һ�����ݿ��д���Ÿ�Ŀ¼��Ŀ¼��
	//��Ŀ¼����ֻ������ϼ�Ŀ¼�͵�ǰĿ¼�������ڵ�Ŷ���0
	Clear_Dir_Buf();
	strcpy(dir_buf[0].name,".");
	dir_buf[0].ino = 0;
	strcpy(dir_buf[1].name,"..");
	dir_buf[1].ino = 0;
	fseek(file,BLOCK_SIZE*19,SEEK_SET);
	fwrite(dir_buf,BLOCK_SIZE,1,file);

	fclose(file);
	return true;
}

bool Install()                        //װ�ش����ļ�ϵͳ
{
	int i;
	printf("Installing...\n");
	file = fopen(file_system_name,"rb+");       //��ֻ����ʽ�򿪴����ļ�
	if(NULL == file)
	{
		printf("%s�����ļ���ʧ�ܣ�\n",file_system_name);
		return false;
	}
	//�Ƚ�����������ڴ�
	fseek(file,0L,SEEK_SET);
	fread(&file_system_super,sizeof(super_block_t),1,file);
	//�ٰ������ڵ�λʾͼ�����ڴ�
	fseek(file,BLOCK_SIZE,SEEK_SET);
	fread(inode_bitmap,sizeof(inode_bitmap),1,file);
	//�������ڵ�����ڴ���
	fseek(file,BLOCK_SIZE*2,SEEK_SET);
	fread(iNode,sizeof(iNode),1,file);

	//�ٰ����ݿ�λʾͼ�����ڴ�
	fseek(file,BLOCK_SIZE*18,SEEK_SET);
	fread(data_bitmap,sizeof(data_bitmap),1,file);

	//����Ŀ¼��Ŀ¼�ļ����뵽�ڴ���
	fseek(file,BLOCK_SIZE*19,SEEK_SET);
	fread(dir_buf,BLOCK_SIZE,1,file);

	fclose(file);
	return true;
}


bool Showhelp()
{
	printf("help      Display all the command you can use.\n");
	printf("dumpfs    Display the information of the file system.\n");
	printf("ls        Display all the files the directory has.\n");
	printf("cat       Display the content of the file.\n");
	printf("cp        Copy the files.\n");
	printf("cpin      Copy the files from the Physical disk to the current disk.\n");
	printf("cpout     Copy the files from the current disk to the Physical disk.\n");
	printf("mkfir     Make a new directory.\n");
	printf("new       Make a new file.\n");
	printf("cd        Change the current directory.\n");
	printf("pwd       Print the current directory.\n");
	printf("rm        Remove a single file.\n");
	printf("rmdir     Remove all files in the directory.\n");
	printf("exit      Exit form the current system.\n");
	return true;
}


//�ú������ڸ���һ�������ڵ�ţ��ú�������һ��inode_t���͵Ľ�����������ڵ���
//���ýڵ��Ӧ���������ṩ����Ҫ�ĺ���
inode_t Find_Inode(int ino)
{
	inode_t inode;
	int i,j,k;
	i = ino/8;                //i��ʾ����������ڵ㿪ʼ�飨INODE_BLOCK_START#���̿飩��λ�ã���2+i#���̿飩
	j = ino - 8*i;            //j��ʶ�ڸô��̿��е�һ�������ڵ��¼

	file = fopen(file_system_name,"rb+");
	if(NULL == file)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return inode;
	}
	fseek(file,BLOCK_SIZE*(i+INODE_BLOCK_START),SEEK_SET);
	fread(iNode,sizeof(iNode),1,file);
	fclose(file);

	inode.size = iNode[j].size;
	inode.type = iNode[j].type;
	for(k=0;k<MAX_DISK_ADDRESS;k++)
		inode.disk_address[k] = iNode[j].disk_address[k];
	return inode;
}
//��ǰ�����ڵ�ţ�cur_inode_no
bool Create_File(char file_name[],int index_node)   //����Buffer���飬�ǵý��������ݺ�һ���ֽڸ�0����Ȼ�����ֽ���ʱ�����
{
	int inode_no;          //��ż�����������ļ��������ڵ�
	int buf_block_count;   //������������ռ���ݿ�ĸ���
	int buf_byte_count;    //���������ݵ��ֽ�����
	int block_num;         //��������ָ��������ڵ��������̿��(2#~17#)
	int inode_num;         //�������ڵ�ָ��������ڵ��ڴ������λ��(0~7)
	int i,j;
	int indirect_indexblooc_no; //���Ѱַʱ��ר������Ѱַ�����ݿ�
	///////////////////////////////////////////////////////////////////////////////
	//step1.��ȡ���������ڵ�
	///////////////////////////////////////////////////////////////////////////////
	inode_no = Scan_Inode_Bitmap();     
	if(-1 == inode_no)
	{
		printf("The index code has run out!\nCreate failed!\n");
		return false;
	}
	///////////////////////////////////////////////////////////////////////////
	//step2.��дinode�ṹ
	//////////////////////////////////////////////////////////////////////////
	buf_byte_count = strlen(Buffer);
	buf_block_count = buf_byte_count/BLOCK_SIZE;       
	if(buf_byte_count % BLOCK_SIZE != 0)                   //���Buffer���ֽ���������BLOCK_SIZE��������������Ҫ����һ�����ݿ����Ƕ����������
		buf_block_count++;   
	//���������ڵ�Ż�ø������ڵ�ľ���λ��
	block_num = inode_no/8 + 2;
	inode_num = inode_no % 8;
	
	file = fopen(file_system_name,"rb+");
	if(NULL == file)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return false;
	}
	fseek(file,BLOCK_SIZE*block_num,SEEK_SET);
	fread(iNode,sizeof(iNode),1,file);
	fclose(file);

	iNode[inode_num].type = 1;                           //�������ļ�����ͨ�ļ�
	iNode[inode_num].size = buf_byte_count;              //�������ļ��Ĵ�СΪ�ղż���������ֽ���

	char data[513];                                     //������ʱ����ļ������ݣ�����д����̿������
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//�����ļ��Ĵ�С��ѡ��ͬ��Ѱַ��ʽ
	if(buf_block_count <= 10)                            //���ļ���С<512B*10ʱ,ֻ��Ҫ�õ�ֱ�ӵ�ַ���10��
	{
		if(Scan_Data_Bitmap(buf_block_count))
		{
			for(i=0;i<buf_block_count;i++)
			{
				iNode[inode_num].disk_address[i] = spare_datablk_num[i];
				//====================================================================
			    //��д��Ѱַ��ʽ�Ϳ���д������
				memset(data,0,sizeof(data));                          //�ȶԽ�Ҫд�Ĵ��̿����0����
				file = fopen(file_system_name,"rb+");
				fseek(file,BLOCK_SIZE*spare_datablk_num[i],SEEK_SET);
				fwrite(data,BLOCK_SIZE,1,file);
				fclose(file);

				Spill(Buffer,data,i+1,512);                //��Buffer���ݿ�ָ��size=512��С���ݿ飬���ֱ�д����̿���
				file = fopen(file_system_name,"rb+");
				fseek(file,BLOCK_SIZE*spare_datablk_num[i],SEEK_SET);
				fwrite(data,BLOCK_SIZE,1,file);
				fclose(file);
			}
			
		}
		else
		{
			printf("�Ѿ�û�и�������ݿ���Է��䣡����ʧ�ܣ�\n");
		}
	}
	else if(buf_block_count <=128)                                   //���ļ���С512B*10<size<=512B*128ʱ��ֻ�õ�һ����ַ��ַ���1��
	{
		Scan_Data_Bitmap(1);                                          //�Ȼ�ȡһ�����ݿ飬Ȼ���ڸ����ݿ�����д��һ��Ѱַ�Ĵ��̺�
		indirect_indexblooc_no = spare_datablk_num[0];                //���ղ�ɨ�����һ�����̿�Ÿ�������indirect_indexblooc_no
		iNode[inode_num].disk_address[10] = indirect_indexblooc_no;   //�����ڵ�ֻ��Ҫ�����������̿飬���ܹ����ҵ��ļ�����Ҫ��ȫ�����̿��
		if(Fill_Index_In_Datablk(indirect_indexblooc_no,buf_block_count))//�����ļ���Ҫ��buf_block_count�����̺������������̿�
		{
			for(i=0;i<buf_block_count;i++)
			{
				memset(data,0,sizeof(data));                          //�ȶԽ�Ҫд�Ĵ��̿����0����
				file = fopen(file_system_name,"rb+");
				fseek(file,BLOCK_SIZE*index[i],SEEK_SET);
				fwrite(data,BLOCK_SIZE,1,file);
				fclose(file);

				Spill(Buffer,data,i+1,512);                //��Buffer���ݿ�ָ��size=512��С���ݿ飬���ֱ�д����̿���
				file = fopen(file_system_name,"rb+");
				fseek(file,BLOCK_SIZE*index[i],SEEK_SET);  //index[i]��ʾ��i+1�����ݴ�ŵĴ��̿��
				fwrite(data,BLOCK_SIZE,1,file);
				fclose(file);
			}
		}
		else
		{
		    printf("�Ѿ�û�и�������ݿ���Է��䣡����ʧ�ܣ�\n");
		}
	}
	else if(buf_block_count <=128*2)                     //���ļ���С512B*128<size<=512B*256ʱ��ֻ�õ�һ����ַ��ַ�������
	{
		Scan_Data_Bitmap(1);                                                // 
		indirect_indexblooc_no = spare_datablk_num[0];                      // 
		iNode[inode_num].disk_address[10] = indirect_indexblooc_no;
		if(Fill_Index_In_Datablk(indirect_indexblooc_no,128))                  //��һ��һ��������Ҫ�ܹ��ҵ�128�����̿�
		{
			for(i=0;i<128;i++)
			{
				memset(data,0,sizeof(data));                          //�ȶԽ�Ҫд�Ĵ��̿����0����
				file = fopen(file_system_name,"rb+");
				fseek(file,BLOCK_SIZE*index[i],SEEK_SET);
				fwrite(data,BLOCK_SIZE,1,file);
				fclose(file);

				Spill(Buffer,data,i+1,512);
				file = fopen(file_system_name,"rb+");
				fseek(file,BLOCK_SIZE*index[i],SEEK_SET);
				fwrite(data,BLOCK_SIZE,1,file);
				fclose(file);
			}
		}
		else
		{
			printf("�Ѿ�û�и�������ݿ���Է��䣡����ʧ�ܣ�\n");
		}

		Scan_Data_Bitmap(1);                                                // 
		indirect_indexblooc_no = spare_datablk_num[0];                      // 
		iNode[inode_num].disk_address[11] = indirect_indexblooc_no;
		if(Fill_Index_In_Datablk(indirect_indexblooc_no,buf_block_count - 128))//ʣ�µ�buf_block_count - 128�����̿齻���ڶ���һ������
		{
			for(i=128,j=0;i<buf_block_count;i++,j++)
			{
				memset(data,0,sizeof(data));                          //�ȶԽ�Ҫд�Ĵ��̿����0����
				file = fopen(file_system_name,"rb+");
				fseek(file,BLOCK_SIZE*index[j],SEEK_SET);
				fwrite(data,BLOCK_SIZE,1,file);
				fclose(file);

				Spill(Buffer,data,i+1,512);
				file = fopen(file_system_name,"rb+");
				fseek(file,BLOCK_SIZE*index[j],SEEK_SET);
				fwrite(data,BLOCK_SIZE,1,file);
				fclose(file);
			}
		}
		else
		{
			printf("�Ѿ�û�и�������ݿ���Է��䣡����ʧ�ܣ�\n");
		}
	}
	else                                                //����ֻ��Ҫ�õ�������ַ��ַ���һ������ʵ��ʱ�����ڵ����ݿ����ֻ��
		                                                //512-128-1=383��
	{
		Scan_Data_Bitmap(1);                            //��Ϊ��һ�����������ݿ�Ĵ��̺�
		indirect_indexblooc_no = spare_datablk_num[0];
		iNode[inode_num].disk_address[12] = indirect_indexblooc_no;
		int index_count;                               //��ʾ�ڶ���������Ҫ�������ݿ�ĸ���
		index_count = buf_block_count / 128;
		if(buf_block_count % 128 != 0)
			index_count ++;
		Fill_Index_In_Datablk(indirect_indexblooc_no,index_count);    //���һ���������ݿ�����д���ݣ���Щ������ָ��ڶ����������ݿ�Ĵ��̿��
		int temp[128];
		for(i=0;i<index_count;i++)//��temp���µ�һ���������ݿ���ָ������������ݿ�Ĵ��̿�ţ�������һ��������Щ���̿�ż����������ݿ�����ݿ�
			temp[i] = index[i];
		int k;
		int buffer_part_count = 1;              //��¼���ݻ���buffer�Ŀ���
		for(k=0;k<index_count-1;k++)//temp[k]��ʾ��k�������������̿�
		{	
			if(Fill_Index_In_Datablk(temp[k],128))//������������̿�����д�ָ����ݵ����ݴ��̿�ţ���Щ�������̿��ж���128��
			{
				//Ȼ�������Щ���̿�Ž���д�������һһд��ȥ
				for(i=0;i<128;i++)
				{
					memset(data,0,sizeof(data));                          //�ȶԽ�Ҫд�Ĵ��̿����0����
					file = fopen(file_system_name,"rb+");
					fseek(file,BLOCK_SIZE*index[i],SEEK_SET);
					fwrite(data,BLOCK_SIZE,1,file);
					fclose(file);

					Spill(Buffer,data,buffer_part_count,512);
					buffer_part_count ++;
					file = fopen(file_system_name,"rb+");
					fseek(file,BLOCK_SIZE*index[i],SEEK_SET);
					fwrite(data,BLOCK_SIZE,1,file);
					fclose(file);
				}
			}
			else
			{
				printf("�Ѿ�û�и�������ݿ���Է��䣡����ʧ�ܣ�\n");
				return false;
			}
		}
		//���һ��������index_count���������������ݿ��п��ܲ���128��,
		if(Fill_Index_In_Datablk(temp[k],buf_block_count-128*(index_count-1)))
		{
			//�����������������Ҫ��ʣ�µ��������Ӧ�Ĵ��̿���
			for(i=0;i<buf_block_count-128*(index_count-1);i++)
			{
				memset(data,0,sizeof(data));                          //�ȶԽ�Ҫд�Ĵ��̿����0����
				file = fopen(file_system_name,"rb+");
				fseek(file,BLOCK_SIZE*index[i],SEEK_SET);
				fwrite(data,BLOCK_SIZE,1,file);
				fclose(file);

				Spill(Buffer,data,buffer_part_count,512);
				buffer_part_count ++;
				file = fopen(file_system_name,"rb+");
				fseek(file,BLOCK_SIZE*index[i],SEEK_SET);
				fwrite(data,BLOCK_SIZE,1,file);
				fclose(file);
			}
		}
		else
		{
			printf("�Ѿ�û�и�������ݿ���Է��䣡����ʧ�ܣ�\n");
				return false;
		}
		/*---------------  ������ַ��ʱ�ò���  ------------------*/
	}
	/*----------------------�ٽ���д�õ�iNode�ṹд���ļ���----------------------------*/
	file = fopen(file_system_name,"rb+");
	fseek(file,BLOCK_SIZE*block_num,SEEK_SET);
	fwrite(iNode,sizeof(iNode),1,file);   //��ʵֻ������iNode[inode_num]����һ��
	fclose(file);

	//step4.��д�ļ�Ŀ¼�����������ļ����ӵ���ǰĿ¼��
	int dir_blkno;                      //��ǰ�ļ������ڵĴ��̿�
	block_num = index_node/8 + 2;       //���ݸ����������ڵ㣬���½����ļ�����ڸýڵ�ָ����λ��
	inode_num = index_node % 8;
	file = fopen(file_system_name,"rb+");
	fseek(file,BLOCK_SIZE*block_num,SEEK_SET);
	fread(iNode,sizeof(iNode),1,file);
	fclose(file);
	dir_blkno = iNode[inode_num].disk_address[0];   //���ݵ�ǰ�����ڵ�Ų��������ڵ㣬Ȼ���ҵ���Ӧ��Ŀ¼�ļ��ڵĴ��̿��
	Clear_Dir_Buf();
	file = fopen(file_system_name,"rb+");
	fseek(file,BLOCK_SIZE*dir_blkno,SEEK_SET);
	fread(dir_buf,sizeof(dir_buf),1,file);
	fclose(file);
	i=0;
	while(dir_buf[i].name[0])
		i++;
	strcpy(dir_buf[i].name,file_name);
	dir_buf[i].ino = inode_no;

	file = fopen(file_system_name,"rb+");
	fseek(file,BLOCK_SIZE*dir_blkno,SEEK_SET);
	fwrite(dir_buf,sizeof(dir_buf),1,file);
	fclose(file);
}

//ɨ�������ڵ�λʾͼ������һ�����õ������ڵ�ţ�ͬʱ����Ӧ�������ڵ�λʾͼ��Ӧλ��1
int Scan_Inode_Bitmap()
{
	file = fopen(file_system_name,"rb+");
	if(NULL == file)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return false;
	}
	fseek(file,BLOCK_SIZE,SEEK_SET);
	fread(inode_bitmap,sizeof(inode_bitmap),1,file);
	fclose(file);
	int column,row;                  //column��¼λʾͼ���У�һ��16��;row��¼λʾͼ���У�һ��8��
	char temper;
	int inode_no;                     //ɨ�������ڵ�λʾͼ�Ľ��������ɨ��ĵ�һ�����������ڵ�ĺţ���0��127��
	for(column = 0;column < 16;column ++)
	{
		for(row = 7;row >= 0;row --)
		{
		    temper = inode_bitmap[column];
			if( ((temper>>row) & 0x01) == 0)           //��ʾ��column�е�λʾͼ�ĵ�(8-row) bitֵΪ0������Ӧ�������ڵ�Ϊ����
			{
				inode_no = column * 8 + (7 - row);
				inode_bitmap[column] = inode_bitmap[column] | (0x01<<row);   //�����Ѿ������ȥ�������ڵ�λʾͼ����1��
				file = fopen(file_system_name,"rb+");
				fseek(file,BLOCK_SIZE,SEEK_SET);
				fwrite(inode_bitmap,sizeof(inode_bitmap),1,file);
				fclose(file);
				return inode_no;
			}
		}
	}
	return -1;    //��ʶ�Ѿ�û�������ڵ�ɹ������ˣ������ļ�ʧ��
}

 //ɨ�����ݿ�λʾͼ������Ĳ�������Ҫ�����ݿ�ĸ�������˳��ɨ��󣬽��������ݿ�Ŀ�Ŵ���ȫ�ֱ���spare_datablk_num������
bool Scan_Data_Bitmap(int data_block_count)
{
	int count = 0;                   //��¼�Ѿ��ҵ��Ŀ��п����Ŀ
	int column,row;                  //column��¼λʾͼ���У�һ��16��;row��¼λʾͼ���У�һ��8��
	char temper;
	int trans;
	memset(spare_datablk_num,0,sizeof(spare_datablk_num));

	file = fopen(file_system_name,"rb+");
	if(NULL == file)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return false;
	}
	fseek(file,BLOCK_SIZE*18,SEEK_SET);
	fread(data_bitmap,sizeof(data_bitmap),1,file);

	for(column = 0;column < 64;column ++)   //���ݿ�λʾͼ�������8 bit * 64 = 512 bit
	{
		for(row = 7;row >= 0;row --)       //ɨ��ÿһ�е�8��bitλ
		{
		    temper = data_bitmap[column];
			if( ((temper>>row) & 0x01) == 0)           //��ʾ��column�е�λʾͼ�ĵ�(8-row) bitֵΪ0������Ӧ�������ڵ�Ϊ����
			{
				trans = column * 8 + (7 - row);             //�õ���ֻ�Ƕ�Ӧ�����ݿ�ţ�����Ҫ��֮ת��Ϊ���̺�
				spare_datablk_num[count++] = trans + 19;    //���ݿ�ż�19֮����Ǵ��̺�
				data_bitmap[column] = data_bitmap[column] | (0x01<<row);   //�����Ѿ������ȥ�������ڵ�λʾͼ����1��
				if(count == data_block_count)
				{
					file = fopen(file_system_name,"rb+");
					fseek(file,BLOCK_SIZE*18,SEEK_SET);
					fwrite(data_bitmap,sizeof(data_bitmap),1,file);
					fclose(file);
					return true;
				}
			}
		}
	}
	if(count < data_block_count)
	{
	     return false;    //��ʶ�Ѿ�û�������ڵ�ɹ������ˣ������ļ�ʧ��
	}
}



bool Fill_Index_In_Datablk(int indexblock_no,int buf_block_count)
{
	int i;
	memset(index,0,sizeof(index));
	
	if(Scan_Data_Bitmap(buf_block_count))          //��ȡbuf_block_count���������ݿ�
	{
		for(i = 0;i < buf_block_count; i ++)      //����ȡ��buf_block_count���������ݿ��д��indexblock_noָ������ݿ���
		{
			index[i] = spare_datablk_num[i];
		}
		file = fopen(file_system_name,"rb+");
	    if(NULL == file)
	    {
		     printf("���ļ�ʧ�ܣ�\n");
		    return false;
	     }
		fseek(file,BLOCK_SIZE*indexblock_no,SEEK_SET);//indexblock_noֱ�Ӷ�Ӧ�ľ��Ǵ��̺�
		fwrite(index,sizeof(index),1,file);
		fclose(file);
	}
	else
	{
		return false;
	}
	return true;
}
//��һ��������ݿ�ָ�ɶ��С���ݿ�,data_count��ʶ��big[]�ĵڼ�������small[]��,size��ʶÿһ��Ĵ�С
void Spill(char big[],char small[],int data_count,int size) 
{
	int i,j;
	i = (data_count-1)*size;
	j = 0;
	for(;big[i] && j<size;i++,j++)
	{
		small[j] = big[i];
	}
	small[j] = 0;
}

//�ļ��������������룺�ļ�·��    ������ļ��������ڵ��
int Lookup_File(char *path)
{
	int i,j;
	char filename[20];        //��ʱ����ļ���
	int index_node_num ;      //�ļ���Ӧ�������ڵ��
	bool found ;              //·���Ƿ��д�
	int file_count = 0;       //��·�����ж��ٸ��ļ���
	int file_start;           //��·���ָ�ʱ��¼�ϵ��
	inode_t inode;            //���������ڵ���ҵ���Ӧ�������ڵ��
	int dir_blk_num;         //Ŀ¼�ļ���Ӧ�Ĵ��̿��
	for(i=0;path[i];i++)
		if(path[i] == '/')
			file_count++;


	if(path[0] != '/')
	{
		return -1;
	}

	file_start = 1;
	index_node_num = 0;//��Ŀ¼�������ڵ�Ϊ0
	for(j=0;j<file_count;j++)
	{
		found = false;
		for(i=file_start;path[i]!='/' && path[i];i++)
			filename[i-file_start] = path[i];
		filename[i-file_start] = 0;
		file_start = i+1;
		inode = Find_Inode(index_node_num);
		dir_blk_num = inode.disk_address[0];

		Clear_Dir_Buf();
		file = fopen(file_system_name,"rb+");
	    if(NULL == file)
	    {
		   printf("���ļ�ʧ�ܣ�\n");
		   return -1;
	    }
		fseek(file,BLOCK_SIZE*dir_blk_num,SEEK_SET);
		fread(dir_buf,sizeof(dir_buf),1,file);
		fclose(file);
		for(i=0;dir_buf[i].name[0];i++)
		{
			if(strcmp(dir_buf[i].name,filename) == 0)
			{	
				index_node_num = dir_buf[i].ino;
				found = true;
				break;
			}
		}
		if(false == found)
		{
			printf("·������!\n");
			return -1;
		}
	}
	return index_node_num;
}

//�����ļ���֧�ֵ�ǰ�ļ����µ�ǰ�ļ���Ҳ֧��·���ļ�����
bool Copy_File(char filename1[],char filename2[])
{
	int block_num;         //��������ָ��������ڵ��������̿��(2#~17#)
	int inode_num;         //�������ڵ�ָ��������ڵ��ڴ������λ��(0~7)
	int file_index;        //�ļ���Ӧ��������
	int i,j;
	//��ȡ��һ����������Ӧ���ļ��������ڵ��
	if( isPath(filename1) )
	{
		file_index = Lookup_File(filename1);
	}
	else
	{
		Lookup_Dir(cur_inode_no);
		for(i=0;dir_buf[i].name[0];i++)
		{
			if(strcmp(dir_buf[i].name,filename1) == 0)
				file_index = dir_buf[i].ino;
		}
	}
	if(!Read_File(file_index))
	{
		printf("Can't read the directory!\n");
		return false;
	}
	//��ȡ�ڶ�����������Ӧ���ļ��������ڵ��
	int dir_file_index;                //Ŀ¼�ļ��������ڵ��
	char file_name[20];                //�ļ���
	char path[20];                     //�ļ���ŵ�·��
	int middle;                       //�ֽ�
	int count=0;
	i=0;
	j=0;
	if( isPath(filename2) )
	{
		while(filename2[i])
		{
			if(filename2[i] == '/')
				count++;
			i++;
		}
		for(i=0;filename2[i];i++)
		{	
			if(filename2[i] == '/')
				j++;
			if(j == count)
				break;
		}
		middle = i;              //�ҵ�����2�ķֽ�
		///////////////////////////////////////////////////
		//������2�ֽ��·�����ļ������ֱ�����path��file_name��
		for(i=0;i<middle;i++)
			path[i] = filename2[i];
		path[i] = 0;
		for(j=0,i=middle+1;filename2[i];i++,j++)
			file_name[j]=filename2[i];
		file_name[j] = 0;
		dir_file_index = Lookup_File(path);   //����·�����õ������������ļ�Ӧ��ŵ��ļ�Ŀ¼��������
	}
	else
	{
		dir_file_index = cur_inode_no;
		strcpy(file_name,filename2);
	}
	if(isLegal(dir_file_index,file_name))
	{
	     Create_File(file_name,dir_file_index);    //�������ļ��ļ������ļ�Ӧ�ڵ�Ŀ¼��Ŀ¼�ļ���������
	}
	else
	{
		printf("\tError!\nThe file has existed!\n");
		return false;
	}
	return true;
}

//�жϸò������ļ�������·��
bool isPath(char *name)
{
	int i = 0;
	while(name[i])
	{
		if(name[i] == '/')     //�ļ����в����ڡ�/��
			return true;
		i++;
	}
	return false;
}

//����Ŀ¼������Ŀ¼�ļ��ڵ㣬����ǽ�Ŀ¼�ڵ��Ӧ��Ŀ¼���뵽ȫ�ֱ���dir_buf[]��,ͬʱ����Ŀ¼�ļ����ڵĴ��̺ŷ���
int Lookup_Dir(int index_node)
{
	int block_num;         //��������ָ��������ڵ��������̿��(2#~17#)
	int inode_num;         //�������ڵ�ָ��������ڵ��ڴ������λ��(0~7)
	int dir_file_block;    //Ŀ¼�ļ���Ӧ�Ĵ��̿��
	block_num = index_node / 8 + 2;
	inode_num = index_node % 8;
	file = fopen(file_system_name,"rb+");
	if(NULL == file)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return false;
	}

	fseek(file,BLOCK_SIZE * block_num,SEEK_SET);
	fread(iNode,sizeof(iNode),1,file);
	fclose(file);
	dir_file_block = iNode[inode_num].disk_address[0];     //�õ�Ŀ¼�ļ���Ӧ�Ĵ��̺�
	Clear_Dir_Buf();
	file = fopen(file_system_name,"rb+");
	fseek(file,BLOCK_SIZE*dir_file_block,SEEK_SET);
	fread(dir_buf,sizeof(dir_buf),1,file);                //��Ŀ¼�ļ��е����ݶ��뻺��dir_buf��
	fclose(file);
	return dir_file_block;
}

//��ȡ�ļ��������ļ��������ڵ�ţ�����Ӧ���ļ������ݶ�ȡ���ڴ滺��Buffer��
bool Read_File(int file_index)
{
	//1.��ȡ�����ڵ�Ŷ�Ӧ�������ڵ�
	int block_num;         //��������ָ��������ڵ��������̿��(2#~17#)
	int inode_num;         //�������ڵ�ָ��������ڵ��ڴ������λ��(0~7)
	int file_size;         //�����ڵ�ָʾ���ļ��Ĵ�С����λ���ֽ���
	int file_block;        //���ļ���С��������ļ���ӵ�еĴ������ĸ���
	block_num = file_index / 8 + 2;
	inode_num = file_index % 8;
	file = fopen(file_system_name,"rb+");
	if(NULL == file)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return false;
	}

	fseek(file,BLOCK_SIZE * block_num,SEEK_SET);
	fread(iNode,sizeof(iNode),1,file);
	fclose(file);

	if(iNode[inode_num].type == 0)          //����������ļ���һ��Ŀ¼�ļ�ʱ����ȡʧ�ܣ�
		return false;
	file_size = iNode[inode_num].size;
	file_block = file_size / 512;
	if(file_size % 512 != 0)
	{
		file_block ++;
	}
	//2.�����ļ���ӵ�еĴ��̿�������ô���Ѱַ��ʽ
	//���̿���Ŀ������10ʱ���ļ�����ֱ��Ѱַ
	int i,j;
	int block_index;
	if( file_block <= 10 )
	{
		for(i=0;i<file_block;i++)
		{	
			block_index = iNode[inode_num].disk_address[i];
			Get_Data(block_index);
			Merge(i);
		}
	}
	//���̿���Ŀ������128ʱ,����һ����ַ
	else if( file_block <= 128 )
	{
		block_index = iNode[inode_num].disk_address[10];
		file = fopen(file_system_name,"rb+");
		fseek(file,BLOCK_SIZE * block_index,SEEK_SET);
		fread(index,sizeof(index),1,file);                    //�Ƚ��������������ݶ�ȡ����
		fclose(file);
		for(i = 0;i < file_block;i ++)
		{
			block_index = index[i];                          //�������������е���õ��ļ��ĸ������̿��
			Get_Data(block_index);                           //��ȡ�������̿飬��ȡ���е�����
			Merge(i);                                        //�������̿�����ݺϲ�����Buffer��
		}
	}
	//���̿���Ŀ������256ʱ,��������һ����ַ
	else if(file_block <= 128 * 2)
	{
		block_index = iNode[inode_num].disk_address[10];
		file = fopen(file_system_name,"rb+");
		fseek(file,BLOCK_SIZE * block_index,SEEK_SET);
		fread(index,sizeof(index),1,file);                    //�Ƚ��������������ݶ�ȡ����
		fclose(file);
		for(i = 0;i < 128;i ++)
		{
			block_index = index[i];                          //�������������е���õ��ļ��ĸ������̿��
			Get_Data(block_index);                           //��ȡ�������̿飬��ȡ���е�����
			Merge(i);                                        //�������̿�����ݺϲ�����Buffer��
		}

		block_index = iNode[inode_num].disk_address[11];
		file = fopen(file_system_name,"rb+");
		fseek(file,BLOCK_SIZE * block_index,SEEK_SET);
		fread(index,sizeof(index),1,file);                    //�Ƚ��������������ݶ�ȡ����
		fclose(file);
		for(i = 0;i < file_block - 128;i ++)
		{
			block_index = index[i];                          //�������������е���õ��ļ��ĸ������̿��
			Get_Data(block_index);                           //��ȡ�������̿飬��ȡ���е�����
			Merge(i + 128);                                        //�������̿�����ݺϲ�����Buffer��
		}
	}
	//���򣬲���һ��������ַ
	else
	{
		//��ʱ��д
		int temp_len;
		int temp[128];
		int buffer_part = 0;
		block_index = iNode[inode_num].disk_address[12];
		file = fopen(file_system_name,"rb+");
		fseek(file,BLOCK_SIZE * block_index,SEEK_SET);
		fread(index,sizeof(index),1,file);                    //�Ƚ��������������ݶ�ȡ����
		fclose(file);
		for(i=0;index[i]!=0;i++)
		{
			temp[i] = index[i];
		}
		temp_len = i;
		for(i=0;i<temp_len-1;i++)
		{
			block_index = temp[i];
			file = fopen(file_system_name,"rb+");
		    fseek(file,BLOCK_SIZE * block_index,SEEK_SET);
		    fread(index,sizeof(index),1,file);                    //�Ƚ��������������ݶ�ȡ����
		    fclose(file);
			for(j = 0;j < 128;j ++)
		    {
			   block_index = index[j];                          //�������������е���õ��ļ��ĸ������̿��
			   Get_Data(block_index);                           //��ȡ�������̿飬��ȡ���е�����
			   Merge(buffer_part);                              //�������̿�����ݺϲ�����Buffer��
			   buffer_part ++;
		    }
		}
		block_index = temp[i];
		file = fopen(file_system_name,"rb+");
		fseek(file,BLOCK_SIZE * block_index,SEEK_SET);
		fread(index,sizeof(index),1,file);                    //�Ƚ��������������ݶ�ȡ����
		fclose(file);
		for(j = 0;j < file_block - 128 * (temp_len-1);j ++)
		{
			block_index = index[j];                          //�������������е���õ��ļ��ĸ������̿��
			Get_Data(block_index);                           //��ȡ�������̿飬��ȡ���е�����
			Merge(buffer_part);                              //�������̿�����ݺϲ�����Buffer��
			buffer_part ++;
		}
	}

	return true;
}

//ͨ�����̿�ţ��������ϵ����ݶ��뻺����Block_Data��
bool Get_Data(int block_index)
{
	memset(Block_Data,0,sizeof(Block_Data));
	file = fopen(file_system_name,"rb+");
	if(NULL == file)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return false;
	}
	fseek(file,BLOCK_SIZE*block_index,SEEK_SET);
	fread(Block_Data,sizeof(Block_Data),1,file);

	fclose(file);
}


//�ϲ����������ڽ�ÿ�ζ�����һ�����̿��ϵ�����Block_Data�鲢��Buffer��countΪ�ڼ������̿��ϵ�����
bool Merge(int count)
{
	int i,j;
	j = count * BLOCK_SIZE;
	for(i=0;Block_Data[i]!=0;i++,j++)
		Buffer[j] = Block_Data[i];
	Buffer[j] = 0;                             //������ܳ��ֵ���β��
	return true;
}

bool Copy_File_out(char filename1[],char filename2[])
{
	int file_index;        //�ļ���Ӧ��������
	bool find;
	int i,j;
	//��ȡ��һ����������Ӧ���ļ��������ڵ��
	if( isPath(filename1) )
	{
		file_index = Lookup_File(filename1);
		if(file_index == -1)
		{
			printf("�ļ�·��������!\n");
			return false;
		}
	}
	else
	{
		Lookup_Dir(cur_inode_no);
		find = false;
		for(i=0;dir_buf[i].name[0];i++)
		{
			if(strcmp(dir_buf[i].name,filename1) == 0)
			{	
				file_index = dir_buf[i].ino;
				find = true;
			}
		}
		if(find == false)
		{
			printf("Ҫ���Ƶ��ļ������ڣ�\n");
			return false;
		}
	}
	if(!Read_File(file_index))                      //������1ָ�����ļ��е����ݶ�ȡ��Buffer��
	{	
	 	printf("Can't read the directory!\n");
		return false;
	}
	file = fopen(filename2,"w");
	if(NULL == file)
	{
		printf("�ļ�����ʧ�ܣ�\n");
		return false;
	}
	fseek(file,0,SEEK_SET);
	fwrite(Buffer,strlen(Buffer),1,file);
	fclose(file);
	return true;
}

bool Copy_File_In(char filename1[],char filename2[])
{
	file = fopen(filename1,"rb+");
	if(NULL == file)
	{
		printf("�ļ�����ʧ�ܣ�\n");
		return false;
	}
	fseek(file,0,SEEK_SET);
	fread(Buffer,sizeof(Buffer),1,file);        //����������ļ��е����ݶ���Buffer��
	fclose(file);

	//��ȡ�ڶ�����������Ӧ���ļ��������ڵ��
	int dir_file_index;                //Ŀ¼�ļ��������ڵ��
	char file_name[20];                //�ļ���
	char path[20];                     //�ļ���ŵ�·��
	int middle;                       //�ֽ�
	int count=0;
	int i=0;
	int j=0;
	if( isPath(filename2) )
	{
		while(filename2[i])
		{
			if(filename2[i] == '/')
				count++;
			i++;
		}
		for(i=0;filename2[i];i++)
		{	
			if(filename2[i] == '/')
				j++;
			if(j == count)
				break;
		}
		middle = i;              //�ҵ�����2�ķֽ�
		///////////////////////////////////////////////////
		//������2�ֽ��·�����ļ������ֱ�����path��file_name��
		for(i=0;i<middle;i++)
			path[i] = filename2[i];
		path[i] = 0;
		for(j=0,i=middle+1;filename2[i];i++,j++)
			file_name[j]=filename2[i];
		file_name[j] = 0;
		dir_file_index = Lookup_File(path);   //����·�����õ������������ļ�Ӧ��ŵ��ļ�Ŀ¼��������
	}
	else
	{
		dir_file_index = cur_inode_no;
		strcpy(file_name,filename2);
	}
	if(isLegal(dir_file_index,file_name))
	{
	    Create_File(file_name,dir_file_index);    //�������ļ��ļ������ļ�Ӧ�ڵ�Ŀ¼��Ŀ¼�ļ���������
	}
	else
	{
		printf("\tError!\nThe file has existed!\n");
		return false;
	}
	return true;
}

bool Delete_File(char filename[])
{
	int i,j;
	bool find;
	int file_index;                         //��ɾ�����ļ���������
	int father_block;                       //��ǰ�ļ������ڵĴ��̿�
	father_block = Lookup_Dir(cur_inode_no);

	find = false;
	for(i=0;dir_buf[i].name[0];i++)
	{
		if(strcmp(dir_buf[i].name,filename) == 0)   //�ڵ�ǰĿ¼���ҵ����ļ����������ڸ�Ŀ¼�ļ���ɾ����ͬʱ����Ӧ���ļ������ڵ����
		{
			file_index = dir_buf[i].ino;
			find = true;
			for(j=i;dir_buf[j].name[0];j++)
			{
				strcpy(dir_buf[j].name,dir_buf[j+1].name);
				dir_buf[j].ino = dir_buf[j+1].ino;
			}
			file = fopen(file_system_name,"rb+");
	        if(NULL == file)
	        {
		        printf("���ļ�ʧ�ܣ�\n");
		        return false;
	        }
			fseek(file,BLOCK_SIZE*father_block,SEEK_SET);
			fwrite(dir_buf,sizeof(dir_buf),1,file);
			fclose(file);
			break;
		}
	}
	if(find == false)
	{
		printf("Can't find the file to be deleted!\n");
		return false;
	}
	//2.ͨ���ļ��������ڵ���������ڵ�λʾͼ������ӦbitΪ0
	Delete_From_Inode_Bitmap(file_index);

	//3.�����ļ��������ڵ���ҵ���Ӧ�������ڵ㣬��������ļ�ӵ�е����ݿ�
	//ͬʱ�����ݿ�λʾͼ������ӦbitΪ0
	int block_num;         //��������ָ��������ڵ��������̿��(2#~17#)
	int inode_num;         //�������ڵ�ָ��������ڵ��ڴ������λ��(0~7)

	int file_size;        //�����ļ��Ĵ�С����λΪ�ֽ�
	int file_block;       //��¼�ļ���ռ�����ݿ���Ŀ
	block_num = file_index / 8 + 2;
	inode_num = file_index % 8;

	file = fopen(file_system_name,"rb+");
	fseek(file,BLOCK_SIZE*block_num,SEEK_SET);
	fread(iNode,sizeof(iNode),1,file);
	fclose(file);

	file_size = iNode[inode_num].size;
	file_block = file_size / 512;
	if(file_size % 512 != 0)
		file_block ++;
	int block_index;
	if(file_block <= 10)
	{
		for(i=0;i<file_block;i++)
		{
			block_index = iNode[inode_num].disk_address[i];     //ÿ�λ��һ�����ݿ�������ڵ�
			Delete_From_Data_Bitmap(block_index);               //����������ݿ�������ڵ�ţ������ݿ�λʾͼ����Ӧ��bitλ��0
		}
	}
	else if(file_block <= 128)
	{
		block_index = iNode[inode_num].disk_address[10];
		file = fopen(file_system_name,"rb+");
		fseek(file,BLOCK_SIZE*block_index,SEEK_SET);
		fread(index,sizeof(index),1,file);
		fclose(file);
		Delete_From_Data_Bitmap(block_index);               //���������ݿ������֮�󣬱�����ͷ��������ݿ����������
		for(i=0;i<file_block;i++)
		{
			block_index = index[i];
			Delete_From_Data_Bitmap(block_index);               //����������ݿ�������ڵ�ţ������ݿ�λʾͼ����Ӧ��bitλ��0
		}
	}
	else if(file_block <= 128*2)
	{
		//�ȶԵ�һ���������ݿ�����ͷ������ڵ�
		block_index = iNode[inode_num].disk_address[10];
		file = fopen(file_system_name,"rb+");
		fseek(file,BLOCK_SIZE*block_index,SEEK_SET);
		fread(index,sizeof(index),1,file);
		fclose(file);
		Delete_From_Data_Bitmap(block_index);               //���������ݿ������֮�󣬱�����ͷ��������ݿ����������
		for(i=0;i<128;i++)
		{
			block_index = index[i];
			Delete_From_Data_Bitmap(block_index);               //����������ݿ�������ڵ�ţ������ݿ�λʾͼ����Ӧ��bitλ��0
		}
		//�ٶԵڶ����������ݿ�����ͷ������ڵ�
		block_index = iNode[inode_num].disk_address[11];
		file = fopen(file_system_name,"rb+");
		fseek(file,BLOCK_SIZE*block_index,SEEK_SET);
		fread(index,sizeof(index),1,file);
		fclose(file);
		Delete_From_Data_Bitmap(block_index);               //���������ݿ������֮�󣬱�����ͷ��������ݿ����������
		for(i=0;i<file_block-128;i++)
		{
			block_index = index[i];
			Delete_From_Data_Bitmap(block_index);               //����������ݿ�������ڵ�ţ������ݿ�λʾͼ����Ӧ��bitλ��0
		}
	}
	else
	{
		//������ַ���֣���ʱδ����
		int temp[128];
		int temp_len;
		block_index = iNode[inode_num].disk_address[12];
		file = fopen(file_system_name,"rb+");
		fseek(file,BLOCK_SIZE*block_index,SEEK_SET);
		fread(index,sizeof(index),1,file);
		fclose(file);
		for(i=0;index[i];i++)
			temp[i] = index[i];
		temp_len = i;
		Delete_From_Data_Bitmap(block_index);               //���������ݿ������֮�󣬱�����ͷ��������ݿ����������
		for(i=0;i<temp_len-1;i++)
		{
			block_index = temp[i];                         //���ҵ�i������������
			file = fopen(file_system_name,"rb+");
			fseek(file,BLOCK_SIZE*block_index,SEEK_SET);
			fread(index,sizeof(index),1,file);
			fclose(file);
			Delete_From_Data_Bitmap(block_index);           //���������ݿ������֮�󣬱�����ͷ��������ݿ����������
			for(j=0;j<128;j++)
		    {
			    block_index = index[j];
			    Delete_From_Data_Bitmap(block_index);      //�ͷŶ����������Ӧ�����ݿ�
		    }
		}
		//���һ��������������Ҫ�ر���
		block_index = temp[i];                         //�������һ������������
		file = fopen(file_system_name,"rb+");
		fseek(file,BLOCK_SIZE*block_index,SEEK_SET);
		fread(index,sizeof(index),1,file);
		fclose(file);
		Delete_From_Data_Bitmap(block_index);           //���������ݿ������֮�󣬱�����ͷ��������ݿ����������
		for(j=0;j<file_block - 128*(temp_len-1);j++)
		{
			block_index = index[j];
			Delete_From_Data_Bitmap(block_index);      //�ͷŶ����������Ӧ�����ݿ�
		}
	}
	fclose(file);
	return true;
}

 //���ݸ������ļ��������ڵ㣬�������ڵ�λʾͼ�н���Ӧ��bitλ��0
bool Delete_From_Inode_Bitmap(int file_index)
{
	int column,row;
	row = file_index / 8;
	column = file_index % 8;
	file = fopen(file_system_name,"rb+");
	if(NULL == file)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return false;
	}
	fseek(file,BLOCK_SIZE,SEEK_SET);
	fread(inode_bitmap,sizeof(inode_bitmap),1,file);
	fclose(file);
	switch(column)
	{
	case 0:
		inode_bitmap[row] = inode_bitmap[row] & 0x7f;
		break;
	case 1:
		inode_bitmap[row] = inode_bitmap[row] & 0xbf;
		break;
	case 2:
		inode_bitmap[row] = inode_bitmap[row] & 0xdf;
		break;
	case 3:
		inode_bitmap[row] = inode_bitmap[row] & 0xef;
		break;
	case 4:
		inode_bitmap[row] = inode_bitmap[row] & 0xf7;
		break;
	case 5:
		inode_bitmap[row] = inode_bitmap[row] & 0xfb;
		break;
	case 6:
		inode_bitmap[row] = inode_bitmap[row] & 0xfd;
		break;
	case 7:
		inode_bitmap[row] = inode_bitmap[row] & 0xfe;
		break;
	}
	file = fopen(file_system_name,"rb+");
	fseek(file,BLOCK_SIZE,SEEK_SET);
	fwrite(inode_bitmap,sizeof(inode_bitmap),1,file);
	fclose(file);
	return true;
}

 //���ݸ����Ĵ��̵������ڵ㣬�����ݿ�λʾͼ�н���Ӧ��bitλ��0
bool Delete_From_Data_Bitmap(int block_index_no)
{
	int column,row;
	int block_index = block_index_no - 19;
	row = block_index / 8;
	column = block_index % 8;
	file = fopen(file_system_name,"rb+");
	if(NULL == file)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return false;
	}
	fseek(file,BLOCK_SIZE*18,SEEK_SET);
	fread(data_bitmap,sizeof(data_bitmap),1,file);
	fclose(file);
	switch(column)
	{
	case 0:
		data_bitmap[row] = data_bitmap[row] & 0x7f;
		break;
	case 1:
		data_bitmap[row] = data_bitmap[row] & 0xbf;
		break;
	case 2:
		data_bitmap[row] = data_bitmap[row] & 0xdf;
		break;
	case 3:
		data_bitmap[row] = data_bitmap[row] & 0xef;
		break;
	case 4:
		data_bitmap[row] = data_bitmap[row] & 0xf7;
		break;
	case 5:
		data_bitmap[row] = data_bitmap[row] & 0xfb;
		break;
	case 6:
		data_bitmap[row] = data_bitmap[row] & 0xfd;
		break;
	case 7:
		data_bitmap[row] = data_bitmap[row] & 0xfe;
		break;
	}
	file = fopen(file_system_name,"rb+");
	fseek(file,BLOCK_SIZE*18,SEEK_SET);
	fwrite(data_bitmap,sizeof(data_bitmap),1,file);
	fclose(file);
	return true;
}

bool Clear_Dir_Buf()
{
	int i;
	for(i=0;i<32;i++)
		memset(dir_buf[i].name,0,sizeof(dir_buf[i].name));
	return true;
}

//��ʾ�ļ����ݣ����������������·���������ܹ�����·���ҵ��ļ�����ʾ������
bool Show_Content(char FileName[])
{
	int file_index;                     //����ʾ���ļ�����
	int dir_file_index;                //Ŀ¼�ļ��������ڵ��
	char file_name[20];                //�ļ���
	char path[20];                     //�ļ���ŵ�·��
	int middle;                       //�ֽ�
	int count=0;
	int i=0;
	int j=0;
	if( isPath(FileName) )
	{
		while(FileName[i])
		{
			if(FileName[i] == '/')
				count++;
			i++;
		}
		for(i=0;FileName[i];i++)
		{	
			if(FileName[i] == '/')
				j++;
			if(j == count)
				break;
		}
		middle = i;              //�ҵ�����2�ķֽ�
		///////////////////////////////////////////////////
		//������2�ֽ��·�����ļ������ֱ�����path��file_name��
		for(i=0;i<middle;i++)
			path[i] = FileName[i];
		path[i] = 0;
		for(j=0,i=middle+1;FileName[i];i++,j++)
			file_name[j]=FileName[i];
		file_name[j] = 0;
		dir_file_index = Lookup_File(path);   //����·����������ȡ���ļ����ڵ�Ŀ¼�ļ���������dir_buf��
		Lookup_Dir(dir_file_index);
		for(i=2;dir_buf[i].name[0];i++)
		{
			if(strcmp(dir_buf[i].name,file_name) == 0)
				file_index = dir_buf[i].ino;
		}

	}
	else
	{
		Lookup_Dir(cur_inode_no);
		for(i=2;dir_buf[i].name[0];i++)
		{
			if(strcmp(dir_buf[i].name,FileName) == 0)
				file_index = dir_buf[i].ino;
		}
	}
	if(!Read_File(file_index))
	{
		printf("Can't read the directory!\n");
		return false;
	}
	for(i=0;i<strlen(Buffer);i++)
	{	
		printf("%c",Buffer[i]);
	}
	printf("\n");
	return true;
}

//����һ�����ļ����ļ��������û��Լ�����
bool New_file(char filename[])
{
	if( !isLegal(cur_inode_no,filename) )
	{
		printf("\tError!\nThe file has existed!\n");
		return false;
	}
	char selection;
	int count = 0;
	int i;
	char temp[100];
	printf("Please edit now:(End with ##)\n");
	gets(temp);
	while( !(temp[0] == '#' && temp[1] == '#') )
	{
		for(i=0;temp[i];i++)
			Buffer[count++] = temp[i];
		Buffer[count++] = 10;
		gets(temp);
	}
	count -- ;
	Buffer[count] = 0;
	Create_File(filename,cur_inode_no);
	return true;
}

//��ָ����Ŀ¼�£����ļ����Ƿ�Ϸ�
bool isLegal(int dir_index,char filename[])
{
	Lookup_Dir(dir_index);
	int i;
	for(i=2;dir_buf[i].name[0];i++)
	{
		if(strcmp(dir_buf[i].name,filename) == 0)
			return false;
	}
	return true;
}