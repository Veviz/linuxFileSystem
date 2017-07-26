#include "Directory.h"
#include "file_system.h"


extern FILE *file;            //�ļ�ָ�룬���ڸ��ֶ��ļ��Ĳ���
extern char file_system_name[20];           //�����Ĵ����ļ���
extern super_block_t  file_system_super;    //�����ṹ�����
extern char inode_bitmap[INODE_NUM/8];         //�����ڵ�λʾͼ����λ��=8(char)*16=128bit,����һ�����Ա�ʾ128�������ڵ�
extern char data_bitmap[DATA_BLOCK_NUM/8];    //���ݿ�λʾͼ����λ��=8(char)*64=512bit,����һ�����Ա�ʶ521�����ݿ�
extern directory dir_buf[BLOCK_SIZE/sizeof(directory)];   //Ŀ¼���飬��ʾһ�����̿���Դ��32��Ŀ¼��
extern struct inode_t iNode[BLOCK_SIZE/sizeof(inode_t)];                              //�����ڵ�


extern char cmdHead[50];                     //��¼��ǰ���Ǹ��ļ�����
extern int cur_inode_no;                     //���ص�ǰ���ڵ�Ŀ¼�������ڵ�
extern int spare_datablk_num[512];           //��¼�������ݿ�Ŀ�ţ����ڷ������ݿ�
extern int index[128];                             //��ַʱ������Ѱַ���м���̿�

extern char Buffer[1048576];                //1M�Ļ����ڴ�ռ�
extern char Block_Data[513];                //���ڴ��һ�����̿��е����ݵ���ʱ�ռ�

bool Show_Dir()          //��ǰ�������ڵ��Ϊcur_inode_no,ͨ�����������ڵ���̿飬�ҵ��ýڵ��Ӧ�����ݿ�
	                    //ͨ������Ӧ�����ݿ�����ڴ棬���õ���Ŀ¼�����е��ļ�
{
	int size;                                   //��ʾ���ļ��Ĵ�С
	int type;                                   //��ʾ���ļ�������
	inode_t inode;                              //�����������ļ������ڵ㣬��ȡ�ļ������ڵ������
	char str1[50] = "Common file";
	char str2[50] = "Directory file";
	char str[50];
	inode = Find_Inode(cur_inode_no);
	int diskNo;
	int i;
	diskNo = inode.disk_address[0];      //Ĭ��Ŀ¼�ļ�ֻռ��һ�����̲��Ҹô��̺Ŵ���ڵ�ַ��ĵ�һ����ַ����
	
	file = fopen(file_system_name,"rb+");
	if(NULL == file)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return false;
	}
	Clear_Dir_Buf();
	fseek(file,BLOCK_SIZE*diskNo,SEEK_SET);
	fread(dir_buf,sizeof(dir_buf),1,file);//�ҵ���ǰĿ¼��Ӧ�Ĵ��̿�󣬽����������ӡ�ô��̿���ȫ������
	fclose(file);

	
	int j;
	int name_len;
	int size_len;
	int len;
	i = 0;
	printf("inode_no   file_name    file_size     file_type\n");
	while(dir_buf[i].name[0])
	{
		size_len = 0;
		inode = Find_Inode(dir_buf[i].ino);
		size = inode.size;
		type = inode.type;
		if(type == 0)
			strcpy(str,str2);
		else
			strcpy(str,str1);
		name_len = strlen(dir_buf[i].name);
		printf("%d          %s",dir_buf[i].ino,dir_buf[i].name);
		for(j = 0;j <= 12-name_len;j++)
			printf(" ");
		printf("%d",size);
		len = size;
		if(len == 0)
			size_len ++;
		while(len > 0)
		{	
			len = len/10;
			size_len ++;
		}
		for(j=0;j<= 13 - size_len;j++)
			printf(" ");
		printf("%s\n",str);
		i++;
	}
	return true;
}

//��ʾָ��Ŀ¼�µ������ļ�
bool Show_Dir(char dir_path[])                  //��ʾָ��Ŀ¼�µ������ļ�
{
	int size;                                   //��ʾ���ļ��Ĵ�С
	int type;                                   //��ʾ���ļ�������
	inode_t inode;                              //�����������ļ������ڵ㣬��ȡ�ļ������ڵ������
	char str1[50] = "Common file";
	char str2[50] = "Directory file";
	char str[50];
	int dir_no;
	dir_no = Lookup_File(dir_path);            //ͨ��·���ҵ���ӦĿ¼�������ڵ��
	inode = Find_Inode(dir_no);
	int diskNo;
	int i;
	diskNo = inode.disk_address[0];      //Ĭ��Ŀ¼�ļ�ֻռ��һ�����̲��Ҹô��̺Ŵ���ڵ�ַ��ĵ�һ����ַ����
	
	file = fopen(file_system_name,"rb+");
	if(NULL == file)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return false;
	}
	Clear_Dir_Buf();
	fseek(file,BLOCK_SIZE*diskNo,SEEK_SET);
	fread(dir_buf,sizeof(dir_buf),1,file);//�ҵ���ǰĿ¼��Ӧ�Ĵ��̿�󣬽����������ӡ�ô��̿���ȫ������
	fclose(file);

	int j;
	int name_len;      //�����������ļ����ĳ���
	int size_len;     //�����������ļ���С��λ��
	int len;
	i = 0;
	printf("inode_no   file_name    file_size     file_type\n");
	while(dir_buf[i].name[0])
	{
		size_len = 0;
		inode = Find_Inode(dir_buf[i].ino);    //�����ļ��е������ڵ�ţ��õ��ļ������ڵ�
		size = inode.size;
		type = inode.type;
		if(type == 0)                          //��ȡ�ļ�����
			strcpy(str,str2);
		else
			strcpy(str,str1);
		name_len = strlen(dir_buf[i].name);
		printf("%d          %s",dir_buf[i].ino,dir_buf[i].name);   //����ļ������ڵ�ź��ļ���
		for(j = 0;j <= 12-name_len;j++)     //�����ļ�����ĸ�ʽ
			printf(" ");
		printf("%d",size);                 //����ļ���С
		len = size;
		if(len == 0)
			size_len ++;
		while(len > 0)
		{	
			len = len/10;
			size_len ++;
		}
		for(j=0;j<= 13 - size_len;j++)
			printf(" ");
		printf("%s\n",str);              //����ļ�����
		i++;
	}
	return true;
}

//�ӵ�ǰĿ¼�½���ָ��·����Ŀ¼,�ú��������cmdHead��cur_inode_no
bool Change_Dir(char dir_path[])
{
	int dir_no;
	int i;
	//1.���ݸ�����path���Եõ�ָ����Ŀ¼�������ڵ�ţ��������¼��cur_inode_no��
	if( isPath(dir_path) )
	{
		dir_no = Lookup_File(dir_path);
		cur_inode_no = dir_no;
	}
	else
	{
		Lookup_Dir(cur_inode_no);
		for(i=0;dir_buf[i].name[0];i++)
		{
			if(strcmp(dir_buf[i].name,dir_path) == 0)
			{	
				cur_inode_no = dir_buf[i].ino;
				break;
			}
		}
	}
	//2.����cur_inode_no���������ϼ��˻أ�ֱ�����ڵ㣬��¼����һϵ�е�Ŀ¼�ļ������ڵ��
	int dir_index_path[20];                         //��¼�´ӵ�ǰ�����ڵ㵽���ڵ��Ŀ¼�ļ������ڵ��
	int dir_index_no;                               //��¼�Ѿ��˻ص���Ŀ¼�ļ������ڵ��
	int count = 0;
	memset(dir_index_path,0,sizeof(dir_index_path));
	dir_index_no = cur_inode_no;
	dir_index_path[count++] = cur_inode_no;
	while(0 != dir_index_no)                        //ֱ����������Ŀ¼
	{
	     Lookup_Dir(dir_index_no);
		 for(i=0;dir_buf[i].name[0];i++)
		 {	 
			 if(strcmp(dir_buf[i].name,"..") == 0)
			{
				dir_index_no = dir_buf[i].ino;
				dir_index_path[count++] = dir_index_no;
			 }
		 }
	}
	count -- ;
	//3.�Ӹ��ڵ������������Щ�����ڵ��һ���������ƣ�ֱ����ǰĿ¼������
	memset(cmdHead,0,sizeof(cmdHead));
	strcpy(cmdHead,"root");
	int j;
	for(i=count;i>0;i--)
	{
		dir_index_no = dir_index_path[i];
		Lookup_Dir(dir_index_no);
		for(j=0;dir_buf[j].name[0];j++)
		{	
			if(dir_buf[j].ino == dir_index_path[i-1])
		    {
				strcat(cmdHead,"/");
			    strcat(cmdHead,dir_buf[j].name);
				break;
			}
		}
	}
	int cmd_head_size = strlen(cmdHead);
	cmdHead[cmd_head_size] = 0;                      //��ֹ��β
	return 0;
}

//��ʾ��ǰ·��������¼��ǰ·���������е������������
bool Show_Path()
{
	printf("%s\n",cmdHead);
	return true;
}

//����һ��Ŀ¼,֧���ڵ�ǰĿ¼�´���һ����Ŀ¼��Ҳ֧�ָ���һ��·������ָ��·���´�����Ŀ¼
bool Create_Dir(char dir_path[])
{
	int dir_index;                //Ŀ¼�ļ�������
	char path[50];                //����·��
	char dir_name[50];            //��������Ŀ¼�ļ���
	int middle;                       //�ֽ�
	int count=0;
	int i=0;
	int j=0;
	if( isPath(dir_path) )
	{
		while(dir_path[i])
		{
			if(dir_path[i] == '/')
				count++;
			i++;
		}
		for(i=0;dir_path[i];i++)
		{	
			if(dir_path[i] == '/')
				j++;
			if(j == count)
				break;
		}
		middle = i;              //�ҵ�����2�ķֽ�
		///////////////////////////////////////////////////
		//������2�ֽ��·�����ļ������ֱ�����path��file_name��
		for(i=0;i<middle;i++)
			path[i] = dir_path[i];
		path[i] = 0;
		for(j=0,i=middle+1;dir_path[i];i++,j++)
			dir_name[j]=dir_path[i];
		dir_name[j] = 0;
		dir_index = Lookup_File(path);
	}
	else
	{
		dir_index = cur_inode_no;
		strcpy(dir_name,dir_path);
	}
	if(!Make_Dir(dir_index,dir_name))
		return false;
	return true;
}

//���ݸ�����Ŀ¼�����ţ��ڸ�Ŀ¼�ļ��´���Ŀ¼��Ϊ���������ֵ�Ŀ¼
//�涨Ŀ¼�ļ����ֻ��ռ��һ�����ݿ�
bool Make_Dir(int dir_index,char dir_name[])
{
	int inode_no;                 //���������������Ŀ¼�ļ��������ڵ��
	int dir_block_no;             //���������������Ŀ¼�ļ��Ĵ��̿��
	int block_num;         //��������ָ��������ڵ��������̿��(2#~17#)
	int inode_num;         //�������ڵ�ָ��������ڵ��ڴ������λ��(0~7)
	int father_dir_blk;         //�½����ļ��еĸ��ļ������ڵĴ��̿��
	int i,j;
	///////////////////////////////////////////////////////////////////////////////
	//step1.��ȡ���������ڵ�
	///////////////////////////////////////////////////////////////////////////////
	inode_no = Scan_Inode_Bitmap();
	if(-1 == inode_no)
	{
		printf("The index code has run out!\nCreate failed!\n");
		return false;
	}
	//���������ڵ�Ż�ø������ڵ�ľ���λ��
	block_num = inode_no/8 + 2;
	inode_num = inode_no % 8;
	///////////////////////////////////////////////////////////////////////////
	//step2.��дinode�ṹ
	//////////////////////////////////////////////////////////////////////////
	file = fopen(file_system_name,"rb+");
	if(NULL == file)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return false;
	}
	fseek(file,BLOCK_SIZE*block_num,SEEK_SET);
	fread(iNode,sizeof(iNode),1,file);
	fclose(file);
	iNode[inode_num].type = 0;                           //�������ļ���Ŀ¼�ļ�
	iNode[inode_num].size = 0;                          //Ŀ¼�ļ���size�ֶβ�������

	Scan_Data_Bitmap(1);
	dir_block_no = spare_datablk_num[0];
	iNode[inode_num].disk_address[0] = dir_block_no;
	file = fopen(file_system_name,"rb+");
	fseek(file,BLOCK_SIZE*block_num,SEEK_SET);
	fwrite(iNode,sizeof(iNode),1,file);
	fclose(file);

	//step3.�ڷ�����½���Ŀ¼�ļ��Ĵ��̿��ϳ�ʼ��һЩ����
	strcpy(dir_buf[0].name,".");
	dir_buf[0].ino = inode_no;
	strcpy(dir_buf[1].name,"..");
	dir_buf[1].ino = dir_index;
	for(i=2;i<32;i++)
		memset(dir_buf[i].name,0,sizeof(dir_buf[i].name));
	//д���ļ�
	file = fopen(file_system_name,"rb+");
	fseek(file,BLOCK_SIZE*dir_block_no,SEEK_SET);
	fwrite(dir_buf,sizeof(dir_buf),1,file);
	fclose(file);
	//step4.��ԭ����Ŀ¼�ļ�������һ�����Ŀ¼��
	father_dir_blk = Lookup_Dir(dir_index);
	i=0;
	while(dir_buf[i].name[0])
		i++;
	strcpy(dir_buf[i].name,dir_name);
	dir_buf[i].ino = inode_no;
	//д���ļ�
	file = fopen(file_system_name,"rb+");
	fseek(file,BLOCK_SIZE*father_dir_blk,SEEK_SET);
	fwrite(dir_buf,sizeof(dir_buf),1,file);
	fclose(file);
	return true;
}

//ֻ֧��ɾ����ǰ�ļ����µ��ļ��У�������ֻ����һ���ļ��е�����,ɾ�����ļ���������������ļ�
//����뽫���ļ����µ�ȫ���ļ�ȫ��ɾ��
bool Delete_Dir(char dir_name[])
{
	int i,j;
	int father_block;                               //��ɾ�����ļ������ڵ��ļ��е�Ŀ¼�ļ����ڵĴ��̺�
	int dir_index;                                 //���ڴ�Ŵ�ɾ�����ļ��е�Ŀ¼�ļ�������
	int mark;                                      //�����ļ�����תʱ��¼�ļ����������
	father_block = Lookup_Dir(cur_inode_no);       //����ǰĿ¼ȫ����ȡ���ڴ���
	//1.��ȡ��ɾ���ļ��е������ڵ��,ͬʱ�ڵ�ǰ�ļ�����������д�ɾ���ļ��еı���
	for(i=2;dir_buf[i].name[0];i++)
	{
		if(strcmp(dir_buf[i].name,dir_name) == 0)
		{	
			dir_index = dir_buf[i].ino;
			//�ҵ���Ŀ¼��󣬼ǵý����ڱ�Ŀ¼�����,��Ҫд�ش����ļ���
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
	//2.�����ɾ�����ļ����У�����ɾ��Ŀ¼�ļ������ڴ棬�鿴���Ƿ��������ļ�
	mark = cur_inode_no;
	cur_inode_no = dir_index;
	Lookup_Dir(cur_inode_no);
	for(i=2;dir_buf[i].name[0];i++)          //��ո�Ŀ¼�µ��ļ�������ǿ��ļ��У���һ��������
	{
		if( Get_Type(dir_buf[i].ino) == 1)   //�ļ���������ͨ�ļ�
		{	
			Delete_File(dir_buf[i].name);
			Lookup_Dir(cur_inode_no);        //���½���ǰĿ¼�����ڴ�
			i = 1;                           //����֮ǰ�Ѿ�����ɾ�����ļ���Ŀ¼��Ĩȥ����
			                                 //������Ҫ��ͷ��ʼ
		}
		else if(Get_Type(dir_buf[i].ino) == 0)
		{	
			Delete_Dir(dir_buf[i].name);
			Lookup_Dir(cur_inode_no);
			i = 1;
		}
		else
			return false;
	}
	cur_inode_no = mark;        //��ո��ļ��к�������ԭ�����ļ�����
	//3.�޸�inode_bitmap��data_bitmap�е�����
	Delete_From_Inode_Bitmap(dir_index);
	//ͨ�����ļ���ȡ��ɾ�����ļ��ж�Ӧ�������ڵ�,������ļ��ж�Ӧ�Ĵ��̿��
	int block_num;         //��������ָ��������ڵ��������̿��(2#~17#)
	int inode_num;         //�������ڵ�ָ��������ڵ��ڴ������λ��(0~7)
	int dir_block;         //��Ŀ¼�ļ���ŵĴ��̿��
	char data[512];
	block_num = dir_index /8 + 2;
	inode_num = dir_index % 8;
	file = fopen(file_system_name,"rb+");
	if(NULL == file)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return false;
	}
	fseek(file,BLOCK_SIZE*block_num,SEEK_SET);
	fread(iNode,sizeof(iNode),1,file);
	fclose(file);
	dir_block = iNode[inode_num].disk_address[0];
	//��ɾ�����ļ��е�Ŀ¼�ļ����ڴ������
	memset(data,0,sizeof(data));
	file = fopen(file_system_name,"rb+");
	if(NULL == file)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return false;
	}
	fseek(file,BLOCK_SIZE*dir_block,SEEK_SET);
	fwrite(data,sizeof(data),1,file);
	fclose(file);

	Delete_From_Data_Bitmap(dir_block);
	return true;
}

//�����ļ������ڵ���ж��ļ����͵ĺ������������ͨ�ļ����򷵻�1�������Ŀ¼�ļ����򷵻�0
int Get_Type(int file_index)
{
	int block_num;         //��������ָ��������ڵ��������̿��(2#~17#)
	int inode_num;         //�������ڵ�ָ��������ڵ��ڴ������λ��(0~7)
	block_num = file_index /8 + 2;
	inode_num = file_index % 8;

	file = fopen(file_system_name,"rb+");
	if(NULL == file)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return false;
	}
	fseek(file,BLOCK_SIZE*block_num,SEEK_SET);
	fread(iNode,sizeof(iNode),1,file);
	fclose(file);
	return iNode[inode_num].type;
}