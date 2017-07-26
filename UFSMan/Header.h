#ifndef HEADER_H
#define HEADER_H
#include <stdio.h>
#include <string.h>

#define BLOCK_SIZE 512                    //���̿��С512B

#define TOTAL_BLOCK_NUMBER 531           //���������һ����531�����̿�
#define INODE_NUM 128                     //�����ڵ���Ŀ128

#define INODE_BITMAP_START 1              //�����ڵ�λʾͼ��ʼ���̿�� 1
#define INODE_BITMAP_BKNUM 1              //�����ڵ�λʾͼռ�ݴ��̿��� 1
#define INODE_BLOCK_START 2               //�����ڵ㿪ʼ���̿�� 2
#define INODE_BLOCK_NUM 16                //�����ڵ���ռ�Ĵ��̿���ĿΪ16
#define INODE_NUM_PER_BLOCK 8            //ÿ�����̿���ӵ�е������ڵ���ĿΪ8
#define INODE_SIZE 60                     //ÿ�������ڵ�ռ��60B��С�Ŀռ�

#define DATA_BITMAP_START 18              //���ݿ�λʾͼ��ʼ���̿�� 18
#define DATA_BITMAP_BLNUM 1              //���ݿ�λʾͼռ�ݴ��̿��� 1
#define DATA_BLOCK_START 19              //���ݿ鿪ʼ���̿�� 19
#define DATA_BLOCK_NUM  512               //���ݿ�ռ�ݴ��̿���Ŀ  512


#define MAX_DISK_ADDRESS 13              //�����ڵ��Ѱַ�0~9Ϊֱ��Ѱַ��10,11Ϊһ�μ�ַ��12Ϊ���μ�ַ

typedef struct region_t{
    int start_block;   // ��ʼ�飬ÿ��region�ӿ�߽翪ʼ
    int block_count;   // regionռ�õĿ����
    int byte_count;    // regionռ�õ��ֽڸ��� 
                       // byte_count <= block_count * BLOCK_SIZE
}region_t;
//������Ķ���
typedef struct super_block{
	int magic;
    int total_block_count;        //�涨�ܹ��Ĵ��̿����Ŀ
    int inode_count;              //�涨�ܹ��������ڵ���Ŀ
	int data_block_count;         //�涨�ܹ������ݿ����Ŀ
    region_t inode_table_bitmap;  //�����ڵ�λͼ
    region_t inode_table;        //�����ڵ��
    region_t data_area_bitmap;   //���ݿ�λͼ
    region_t data_area;         //���ݿ�����
}super_block_t;


enum Type{
    INODE_REGULAR, // ��ͨ�ļ����ı��ļ�����������ļ�
    INODE_DIR     // Ŀ¼�ļ�
};

//�����ڵ㣬ÿ�������ڵ��Сռ60B,ÿ�����̿�ֻ�ܴ��8�������ڵ�
typedef struct inode_t{
    int type;      // �ļ�����,���ļ������ļ���
    int size;      // �ļ���С
    int disk_address[MAX_DISK_ADDRESS];  // Ѱ���̿鷽ʽ��0~9Ϊֱ�Ӳ��Ҵ��̿�(�ļ�������Ϊ5120B)
	                                     //10Ϊһ�μ�ַ���ļ�����Ϊ64KB��
	                                     //11Ϊ���μ�ַ���ļ�����Ϊ8MB��
}inode_t;

//�ļ�Ŀ¼�ڵ㣬ÿһ���ļ�Ŀ¼��ռ16B
typedef struct dir_entry 
{
	char name[12];  // Ŀ¼������
    int ino;        // �����ڵ��
}directory;


#endif