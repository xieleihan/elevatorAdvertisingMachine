#ifndef __STRUCT_H
#define __STRUCT_H

#include <stdio.h>
#include "font.h"
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
#include "show.h"
#include "tool.h"

typedef struct ad_info
{
    char ad_name[50];
    char ad_path[128];
    char ad_mini_name[50];
    char ad_mini_path[128];
    char ad_thumbnail_name[50];
    char ad_thumbnail_path[128];
    int order;
    struct ad_info *next;
} ad_info;

ad_info *ad_listhead;

// 销毁链表
void destroy();
// 按名字找节点
ad_info *find_delnode(char *name);
// 遍历链表
void ad_info_display();
// 初始化节点
ad_info *init_adinfo(char *adname, char *adpath, int order);
// 插入链表
void insert_adinfo(ad_info *head, ad_info *new);
#endif