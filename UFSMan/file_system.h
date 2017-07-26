#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H
//===================================================================================================
//���ļ�ϵͳ����Ҫ�ĸ��ֲ�����������
//
//===================================================================================================
#include "Header.h"


extern char file_system_name[20];           //�����Ĵ����ļ���
extern directory dir_buf[BLOCK_SIZE/sizeof(directory)];   //Ŀ¼���飬��ʾһ�����̿���Դ��32��Ŀ¼��

bool Format();                       //��ʽ�����������ļ������û�У��򴴽��ô����ļ�
                                     //��ʽ���ɹ��󣬷���true�����г��ִ����򷵻�false
bool Install();                      //װ�ش���ϵͳ����ʱ��ԭ��DOS��������½����ļ�ϵͳ

bool Showhelp();                    //ͨ���ú������û���ʾ�����û����ԵĲ���ָ���Լ�������ʽ


inode_t Find_Inode(int ino);                     //����һ�������ڵ�ţ��ú�������һ��inode_t���͵Ľ�����������ڵ���
                                                 //���ýڵ��Ӧ���������ṩ����Ҫ�ĺ���
bool Create_File(char file_name[],int index_node);  //ͨ��������ļ������ڱ��ļ�ϵͳ�д���һ���ļ�
                                                 //ͬʱ���ļ����ݴ����buffer�У�buffer������ռ��1M�ڴ�
                                                //�������ļ������ָ����Ŀ¼(����index_node��ָ����Ŀ¼�ļ�)��
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//��λʾͼ�ĸ���ֻ��������������������Ե�����ϵͳ֮�󣬽�λʾͼ�����ڴ�󣬲��õ�����ֵ�������������
//
int Scan_Inode_Bitmap();                         //ɨ�������ڵ�λʾͼ������һ�����õ������ڵ�ţ�ͬʱ����Ӧ�������ڵ�λʾͼ
                                                 //��Ӧλ��1
bool Scan_Data_Bitmap(int data_block_count);     //ɨ�����ݿ�λʾͼ������Ĳ�������Ҫ�����ݿ�ĸ�������˳��ɨ��󣬽��������ݿ�
                                                 //�Ŀ�Ŵ���ȫ�ֱ���spare_datablk_num������
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Fill_Index_In_Datablk(int indexblock_no,int buf_block_count);//���Ѱַʱ�õ��������ݿ��з���һ���̿�����ר��ָ����ļ�����Ҫ�Ĵ��̿�

void Spill(char big[],char small[],int data_count,int size);  //��һ��������ݿ�ָ�ɶ��С���ݿ�,data_count��ʶ��big[]�ĵڼ�������small[]��

int Lookup_File(char *path);                                  //�����ļ��ĺ��ĺ���Ϊlookup_file

bool Copy_File(char filename1[],char filename2[]);                      //�����ļ���֧�ֵ�ǰ�ļ����µ�ǰ�ļ���Ҳ֧��·���ļ�����

bool isPath(char *name);                                         //�жϸò������ļ�������·��

int Lookup_Dir(int index_node);                              //����Ŀ¼������Ŀ¼�ļ��ڵ㣬����ǽ�Ŀ¼�ڵ��Ӧ��Ŀ¼���뵽ȫ�ֱ���dir_buf[]��
                                                             //ͬʱ���ظ�Ŀ¼�ļ����ڵĴ��̿��

bool Read_File(int file_index);                             //��ȡ�ļ��������ļ��������ڵ�ţ�����Ӧ���ļ������ݶ�ȡ���ڴ滺��buffer��                             

bool Get_Data(int block_index);                               //ͨ�����̿�ţ��������ϵ����ݶ��뻺����Block_Data��

bool Merge(int count);                                       //�ϲ����������ڽ�ÿ�ζ�����һ�����̿��ϵ�����Block_Data�鲢��Buffer��
                                                             //countΪ�ڼ������̿��ϵ�����
bool Copy_File_out(char filename1[],char filename2[]);      //����ϵͳ�е��ļ���������������ϣ�����2����ֻ��һ���ļ���
                                                           //����1����ֱ�����ļ�����Ҳ�����Ǵ�·�����ļ���
bool Copy_File_In(char filename1[],char filename2[]);     //����������ϵ��ļ����������ļ�ϵͳ�У�����1ֻ��һ���ļ���
                                                          //����2����ֱ�����ļ�����Ҳ�����Ǵ�·�����ļ���
bool Delete_File(char filename[]);                         //ɾ��һ���ļ���ֻ֧��ɾ���ڵ�ǰ�ļ����µ��ļ�

bool Delete_From_Inode_Bitmap(int file_index);           //���ݸ������ļ��������ڵ㣬�������ڵ�λʾͼ�н���Ӧ��bitλ��0

bool Delete_From_Data_Bitmap(int block_index);           //���ݸ����Ĵ��̵������ڵ㣬�����ݿ�λʾͼ�н���Ӧ��bitλ��0

bool Clear_Dir_Buf();                                       //������Dir_Buf���

bool Show_Content(char file_name[]);                      //��ʾ�ļ����ݣ����������������·���������ܹ�����·���ҵ��ļ�����ʾ������

bool New_file(char filename[]);                                         //����һ�����ļ����ļ��������û��Լ�����

bool isLegal(int dir_index,char filename[]);            //��ָ����Ŀ¼�£����ļ����Ƿ�Ϸ�

#endif