#ifndef __SHOW_H
#define __SHOW_H

#include <stdio.h>
#include "font.h"
#include <time.h>
#include <string.h>
#include <pthread.h>
#include "tool.h"
#include "show.h"

int offset_bytes;
// 显示时间
void *show_time(void *arg);
// 显示文字
void show_font(int x, int y, int size, char *str_font, int map_width, int map_height, int font_x, int font_y, color c);
// 显示图片
int show_bmp(int x, int y, char *bmp_name);
// 主屏图片视频轮播
void *traverse_adlist(void *arg);
// 副屏图片轮播
void *traverse_smadlist();
// 显示键盘
void show_keyboard();
// 显示视频
void *show_video();
// 显示缩略图列表
int show_adlist(char *path);
// 显示底边新闻
void *show_news(void *arg);
int find_node(int i);
#endif