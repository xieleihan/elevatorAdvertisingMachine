#include "show.h"

int offset_bytes = 0;

int find_node(int i)
{
    ad_info *p;
    i--;
    // 用于遍历的节点结构体变量
    p = ad_listhead->next; // p第一个数据节点
    while (p != NULL)
    {
        if (i == 0)
        {
            bzero(choose_ad_name, sizeof(choose_ad_name));
            bzero(choose_ad, sizeof(choose_ad));
            printf("%s\n", p->ad_path);
            show_bmp(550, 20, p->ad_thumbnail_path);
            show_font(630, 20, 25, p->ad_name, 100, 50, 0, 20, getColor(0, 234, 217, 153));
            strcpy(choose_ad_name, p->ad_name);
            strcpy(choose_ad, p->ad_path);
            return 0;
        }
        i--;
        p = p->next;
    }
    return -1;
}

void *show_news(void *arg)
{
    FILE *fp;
    fp = fopen("news.txt", "r");
    char new[128] = {0};
    char *p;
    int x = 0;
    while (1)
    {
        p = fgets(new, 128, fp);
        new[strlen(new)] = '\0';
        while (1)
        {
            if (x >= 800)
            {

                x = 0;
                break;
            }

            show_font(13, 443, 20, new, 775, 25, x, 5, getColor(0, 0, 0, 0));
            x++;
        }
        if (p == NULL)
        {
            fseek(fp, 0, SEEK_SET);
        }
    }
    fclose(fp);
}

int show_adlist(char *path)
{
    int row = 1, col = 1;
    int i = 0;
    char now_path[128] = {0};
    ad_info *p;
    char(*pt)[48] = NULL;
    creat_thbmp(60, 52, path);
    p = ad_listhead->next;
    system("killall -9 mplayer");
    show_bmp(0, 0, "work_bmp/ad_UI.bmp");
    if (strcmp(path, "/admachine/morning_adlist") == 0 && strlen(mo_order[0]) == 0)
    {
        pt = mo_order;
    }
    if (strcmp(path, "/admachine/night_adlist") == 0 && strlen(ni_order[0]) == 0)
    {
        pt = ni_order;
    }
    if (strcmp(path, "/admachine/midday_adlist") == 0 && strlen(mi_order[0]) == 0)
    {
        pt = mi_order;
    }

    sprintf(now_path, "当前路径为:%s", path);
    show_font(132, 0, 24, now_path, 396, 94, 0, 0, getColor(0, 234, 217, 153));
    bzero(now_path, sizeof(now_path));
    sprintf(now_path, "当前广告页面显示为:%s", ad_listpath);
    show_font(132, 50, 24, now_path, 396, 40, 0, 0, getColor(0, 234, 217, 153));

    while (p != NULL) // 说明数据节点存在
    {
        if (strcmp(path, "/mnt/udisk") != 0 && pt != NULL)
        {
            strcpy(pt[i], p->ad_name);
        }
        show_font(133 + (99 * (col - 1)), 99 + (97 * (row - 1)), 18, p->ad_name, 96, 92, 20, 72, getColor(0, 234, 217, 153));
        show_bmp(158 + (99 * (col - 1)), 109 + (97 * (row - 1)), p->ad_thumbnail_path);
        if (col == 4)
        {
            row++;
            col = 1;
        }
        else
        {
            col++;
        }
        p = p->next;
        i++;
    }
}

void *show_video()
{
    char video_path[128] = {0};
    sprintf(video_path, "mplayer -slave -quiet -input  file=/pipe -geometry 132:99  -zoom -x 396 -y 200  %s", choose_ad);
    show_font(132, 299, 30, choose_ad_name, 150, 50, 20, 20, getColor(0, 234, 217, 153));
    system(video_path);
}

void show_keyboard()
{
    show_font(0, 0, 50, "1", 100, 120, 25, 40, getColor(0, 0, 0, 0));
    show_font(100, 0, 50, "2", 100, 120, 25, 40, getColor(0, 0, 0, 0));
    show_font(200, 0, 50, "3", 100, 120, 25, 40, getColor(0, 0, 0, 0));
    show_font(300, 0, 50, "", 200, 360, 25, 40, getColor(0, 234, 217, 153));
    show_font(0, 120, 50, "4", 100, 120, 25, 40, getColor(0, 0, 0, 0));
    show_font(100, 120, 50, "5", 100, 120, 25, 40, getColor(0, 0, 0, 0));
    show_font(200, 120, 50, "6", 100, 120, 25, 40, getColor(0, 0, 0, 0));
    show_font(0, 240, 50, "7", 100, 120, 25, 40, getColor(0, 0, 0, 0));
    show_font(100, 240, 50, "8", 100, 120, 25, 40, getColor(0, 0, 0, 0));
    show_font(200, 240, 50, "9", 100, 120, 25, 40, getColor(0, 0, 0, 0));
    show_font(0, 360, 50, "退出", 100, 120, 5, 40, getColor(0, 0, 0, 0));
    show_font(100, 360, 50, "0", 100, 120, 25, 40, getColor(0, 0, 0, 0));
    show_font(200, 360, 50, "确认", 100, 120, 5, 40, getColor(0, 0, 0, 0));
    show_font(300, 360, 50, " 回退", 100, 120, 5, 40, getColor(0, 0, 0, 0));
}

void *traverse_smadlist(void *arg)
{
    ad_info *p;
    // 用于遍历的节点结构体变量
    while (1)
    {
        p = ad_listhead->next; // p第一个数据节点
        while (p != NULL)      // 说明数据节点存在
        {
            if (strcmp(&p->ad_mini_path[strlen(p->ad_mini_path) - 4], ".bmp") == 0)
            {
                show_bmp(smad_x, smad_y, p->ad_mini_path);
                sleep(4);
            }
            p = p->next;
        }
    }
}

void *traverse_adlist(void *arg)
{
    ad_info *p;
    char video_path[128] = {0};
    // 用于遍历的节点结构体变量
    while (1)
    {
        p = ad_listhead->next; // p第一个数据节点
        while (p != NULL)      // 说明数据节点存在
        {

            if (strcmp(&p->ad_name[strlen(p->ad_name) - 4], ".bmp") == 0)
            {
                show_bmp(13, 13, p->ad_path);
                sleep(8);
            }
            else
            {
                sprintf(video_path, "mplayer -slave -quiet -input  file=/pipe -geometry 13:13  -zoom -x 536 -y 424  %s", p->ad_path);
                system(video_path);
                system("clear");
            }
            p = p->next;
        }
    }
}

int show_bmp(int x, int y, char *bmp_name)
{
    int bmp_fd;
    bmp_fd = open(bmp_name, O_RDWR);
    if (bmp_fd == -1)
    {
        perror("open");
        return -1;
    }
    struct tagBITMAPFILEHEADER header;
    struct tagBITMAPINFOHEADER info;

    read(bmp_fd, &header, sizeof(header));
    read(bmp_fd, &info, sizeof(info));

    lseek(bmp_fd, 54, SEEK_SET);

    // if (info.biWidth * info.biBitCount / 8 % 4 != 0)
    // {
    //     offset_bytes = 4 - (info.biWidth * 3) % 4;
    //     printf("offset_bytes:%d\n", offset_bytes);
    // }

    unsigned char bmp_buf[info.biWidth * info.biHeight * 3];
    unsigned int buff[info.biWidth * info.biHeight];

    for (int i = 0; i < info.biHeight; i++)
    {
        read(bmp_fd, &bmp_buf[i * info.biWidth * 3], info.biWidth * 3);
        lseek(bmp_fd, offset_bytes, SEEK_CUR);
    }
    offset_bytes = 0;

    for (int i = 0; i < info.biWidth * info.biHeight; i++)
    {
        buff[i] = bmp_buf[3 * i + 2] << 16 | bmp_buf[3 * i + 1] << 8 | bmp_buf[3 * i];
    }

    for (int i = y; i < info.biHeight + y; i++)
    {
        for (int j = x; j < info.biWidth + x; j++)
        {
            lcd->mp[i * 800 + j] = buff[(info.biHeight - 1 - (i - y)) * info.biWidth + j - x];
        }
    }
    close(bmp_fd);
}

void *show_time(void *arg)
{
    char time[50] = {0};
    char week[50] = {0};
    while (1)
    {
        get_time(time, week);
        show_font(553, 13, 27, time, 235, 64, 7, 20, getColor(0, 0, 0, 0));
        show_font(553, 77, 27, week, 235, 61, 80, 10, getColor(0, 0, 0, 0));
    }
}

void show_font(int x, int y, int size, char *str_font, int map_width, int map_height, int font_x, int font_y, color c)
{

    font *f = fontLoad("/usr/share/fonts/DroidSansFallback.ttf");
    // 字体大小的设置
    fontSetSize(f, size);

    // 创建一个画板（点阵图）
    bitmap *bm = createBitmapWithInit(map_width, map_height, 4, c); // 也可使用createBitmapWithInit函数，改变画板颜色
    // bitmap *bm = createBitmap(288, 100, 4);

    // 将字体写到点阵图上
    fontPrint(f, bm, font_x, font_y, str_font, getColor(0, 255, 255, 255), 0);

    // 把字体框输出到LCD屏幕上
    show_font_to_lcd(lcd->mp, x, y, bm);

    fontUnload(f);
    destroyBitmap(bm);
}