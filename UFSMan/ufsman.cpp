//#include "Header.h"
#include "file_system.h"
#include "Directory.h"
#include "cmd.h"
#include <windows.h>
// disk layout:
//
// super block
// inode_table_bitmap
// inode_table
// data_area_bitmap
// data_area
//-----------------------------------------------------------
//��Unix���ļ�ϵͳ��5��region��ɣ������飬�����ڵ��λͼ�������ڵ�����ݿ�λͼ�����ݿ�����
//�ļ�ϵͳ��Ҫ�ڴ����д���һ����������ļ�disk.img�����жԴ��̵Ĳ�����Ϊ���ļ��Ĳ�����
//����ÿ�����̿�Ĵ�СΪ512B�����̿��0��n��0���̿��ų����飬
//1#���̿��� inode_table_bitmap���������ڵ�λʾͼ��ʹ��int�����ݣ�512B/4B = 128 �����ļ�ϵͳһ������ͬʱӵ��128�������ڵ�
//2# ~ 16#���̿��� inode_table ,�������ڵ�,ÿ�������ڵ���ռ56B��ÿ�����̿���Դ��9�������ڵ㣬����һ����Ҫ15�����̿�
//17#���̿��� data_table_bitmap ,�����ݿ�λʾͼ��ʹ��char�����ݣ�512B/1B = 512,���ļ�ϵͳһ��ӵ��512�����ݿ飬����ǰ���17����һ����529�����̿�
//18# ~ 529#���̿����ڴ������
//-----------------------------------------------------------

/*============       ȫ�ֱ���(�ⲿ����)       ====================*/
extern char cmdHead[20];                     //��¼��ǰ���Ǹ��ļ�����
extern directory dir_buf[BLOCK_SIZE/sizeof(directory)];   //Ŀ¼���飬��ʾһ�����̿���Դ��32��Ŀ¼��
extern super_block_t  file_system_super;    //�����ṹ�����
extern char inode_bitmap[INODE_NUM/8];         //�����ڵ�λʾͼ����λ��=8(char)*16=128bit,����һ�����Ա�ʾ128�������ڵ�
extern char data_bitmap[DATA_BLOCK_NUM/8];    //���ݿ�λʾͼ����λ��=8(char)*64=512bit,����һ�����Ա�ʶ521�����ݿ�



int main(int argc,char * argv[])
{
    char cmd[40];
	char cmd1[20],cmd2[20],cmd3[20];
	int i,j;
    int space_count;
	while( 1 )
	{
		printf("%s>",argv[0]);
		gets(cmd);
		if(cmd[0])
		{
			i = 0;
			space_count = 0;
			while(cmd[i])
				if(cmd[i++] == ' ')
					space_count++;
			if(0 == space_count)
			{	
				if(strcmp(cmd,"ls") == 0)                       //ԭϵͳ�е���ʾ��ǰĿ¼���ļ�
					system("dir");
				else if(strcmp(cmd,"exit") == 0)               //exit
					break;
				else
					printf("command error!\n");
			}
			else if(1 == space_count)
			{
				for(i=0;cmd[i]!=' ';i++)
					cmd1[i] = cmd[i];
				cmd1[i] = 0;
				i++;
				for(j=0;cmd[i];i++,j++)
					cmd2[j] = cmd[i];
				cmd2[j] = 0;
				if(strcmp(cmd1,"ufsman") == 0 && cmd2[0])  //ufsman disk.img
				{
					strcpy(file_system_name,cmd2);        //�����ڶ�������ļ�ϵͳʱ������ͨ������Ĳ�������ѡ���Լ���������ļ�ϵͳ
					if(Install())
						Enter_File_System();
				}
				else
					printf("command error!\n");
			}
			else if(2 == space_count)
			{
				for(i=0;cmd[i] != ' ';i++)
					cmd1[i] = cmd[i];
				cmd1[i++]=0;
				for(j=0;cmd[i] != ' ';i++,j++)
					cmd2[j] = cmd[i];
				cmd2[j++]=0;
				i++;
				for(j=0;cmd[i];i++,j++)
					cmd3[j] = cmd[i];
				cmd3[j]=0;
				if((strcmp(cmd1,"ufsman") == 0) && (strcmp(cmd2,"--mkfs")==0) && (cmd3[0] != 0))  //������������ļ���ͬʱ���и�ʽ��  /*ufsman --mkfs disk.img*/
				{	
					strcpy(file_system_name,cmd3);
					Format();
				}
				else
					printf("command error!\n");
			}
			else
				printf("command error!\n");
		}
	}
	return 0;
}