#ifndef CMD_H
#define CMD_H
#include "Directory.h"
#include "file_system.h"
#include "Header.h"


void Enter_File_System();           //��ʽ�������ļ�֮������������Ѿ����ڸղŴ����Ĵ����ļ�
                                   //ͨ������"ufsman �ļ���",��װ����ϵͳ
int Identify_Cmd(char cmd[]);      //�����������������֣�����ֵΪ-1������������

bool Dump_FS();                              //��ʾ�ļ�ϵͳ������

void Print_Byte(char bit);                 //��һ���ֽڵĸ���bitλ�����


#endif