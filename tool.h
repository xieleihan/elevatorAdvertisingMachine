#ifndef __TOOL_H
#define __TOOL_H

#include <stdio.h>
#include "font.h"
#include "show.h"
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/input.h>
#include <fcntl.h>
#include <signal.h>
#include "struct.h"
#include <stdbool.h>
struct tagBITMAPFILEHEADER
{
    unsigned short bfType; //
    // 保存图片类型，读取时需要注释掉，文本标识符只能单独进行读写
    unsigned int bfSize;        // 文件大小
    unsigned short bfReserved1; // 保留，设置为0
    unsigned short bfReserved2; // 保留，设置为0
    unsigned int bfOffBits;     // 从文件头到实际的图像数据之间的字节的偏移量(没调色板的话是54)
} __attribute__((packed));
// 信息头
struct tagBITMAPINFOHEADER
{
    unsigned int biSize;     // 此结构体的大小
    unsigned int biWidth;    // 图像的宽
    unsigned int biHeight;   // 图像的高
    unsigned short biPlanes; // 颜色平面数 恒为1

    unsigned short biBitCount;   // 一像素所占的位数 Windows系统有8,16,24
    unsigned int biCompression;  // 说明图象数据压缩的类型，0为不压缩
    unsigned int biSizeImage;    // 图像大小, 值等于上面文件头结构中bfSize-bfOffBits
    int biXPelsPerMeter;         // 说明水平分辨率，用像素/米表示 一般为0
    int biYPelsPerMeter;         // 说明垂直分辨率，用像素/米表示 一般为0
    unsigned int biClrUsed;      // 说明位图实际使用的彩色表中的颜色索引数（设为0的话，则说明使用所有调色板项）
    unsigned int biClrImportant; // 说明对图象显示有重要影响的颜色索引的数目
                                 // 如果是0表示都重要
} __attribute__((packed));
// 调色板
struct tagRGBQUAND
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char rgbReserved;
} __attribute__((packed));

#define TS_PATH "/dev/input/event0"
#define NUM_Threads 5
#define ADMS 1

bool playing;
char time_list[4][50];
int now_hour;
char mo_order[20][48];
char ni_order[20][48];
char mi_order[20][48];
char choose_ad_name[128];
char choose_ad[248];
char work_path[128];
int ts_fd;
struct LcdDevice *lcd;
char ad_listpath[248];
pthread_t tids[NUM_Threads];
int smad_x;
int smad_y;

char *add_ad();
void touch_value(int x, int y, char *ch);
// 获取目标目录照片
void get_bmp(char *dirpath);
// 获取时间
void get_time(char *out_time, char *now_week);
struct LcdDevice *init_lcd(const char *device);
// 改变图片大小
int zoom_bmp(char *old_bmppath, int new_width, int new_height, char *new_bmppath);
void changbmp_zoom(int new_width, int new_height);
// 生成副屏图片
void creat_minibmp(int new_width, int new_height);
int touch_s(int mode);
int inp_password();
int dele_node(char *name);
// 生成缩略图
void creat_thbmp(int new_width, int new_height, char *path);
// 按顺序排序链表
void order_adlist(char *dirpath);
// 判断目标目录是否有同名文件
int is_namesame(char *p);

#endif