#include "tool.h"

int now_hour = 0;
char time_list[4][50] = {"morning_adlist", "midday_adlist", "night_adlist", "vertical_list"};
char mo_order[20][48] = {0};
char ni_order[20][48] = {0};
char mi_order[20][48] = {0};
char choose_ad_name[128] = {0};
char choose_ad[248] = {0};
char work_path[128] = {0};
int ts_fd;
bool playing = false;
struct LcdDevice *lcd;
char ad_listpath[248] = {0};
pthread_t tids[NUM_Threads];
int smad_x = 553;
int smad_y = 145;

char *add_ad()
{
    int X, Y, out;
    struct input_event ts;
    int ts_fd_ch = open(TS_PATH, O_RDWR);
    if (ts_fd_ch == -1)
    {
        printf("open ts failure\n");
        return NULL;
    }
    while (1)
    {
        // 松开触摸后，再打印
        while (1)
        {
            read(ts_fd_ch, &ts, sizeof(struct input_event));

            // 判断类型
            if (ts.type == EV_ABS && ts.code == ABS_X)
            {
                X = ts.value;
            }

            if (ts.type == EV_ABS && ts.code == ABS_Y)
            {
                Y = ts.value;
            }

            // 判断按下
            if (ts.type == EV_KEY && ts.code == BTN_TOUCH && ts.value == 1)
            {
            }
            // 判断是否松开
            if (ts.type == EV_KEY && ts.code == BTN_TOUCH && ts.value == 0)
            {

                X = X * (800.0 / 1024.0);
                Y = Y * (480.0 / 600.0);
                break;
            }
        }
        if (X >= 0 && X <= 130 && Y >= 97 && Y <= 189)
        {
            close(ts_fd_ch);
            return time_list[0];
        }
        if (X >= 0 && X <= 130 && Y >= 195 && Y <= 284)
        {
            close(ts_fd_ch);
            return time_list[1];
        }
        if (X >= 0 && X <= 130 && Y >= 290 && Y <= 381)
        {
            close(ts_fd_ch);
            return time_list[2];
        }
    }
}

int is_namesame(char *p)
{
    DIR *pdir;
    struct dirent *pdirent;
    ad_info *new;
    char ad_path[128] = {0};
    pdir = opendir(p);
    if (pdir == NULL)
    {
        printf("opendir fail\n");
        return -1;
    }
    while ((pdirent = readdir(pdir)) != NULL)
    {
        if (strcmp(pdirent->d_name, choose_ad_name) == 0)
        {
            return -1;
        }
    }

    closedir(pdir);
    return 0;
}

void order_adlist(char *dirpath)
{
    int i = 0;
    char(*pt)[48] = NULL;
    ad_info *p, *move, *pos;
    p = ad_listhead;
    pos = ad_listhead;

    if (strcmp(dirpath, "/admachine/morning_adlist") == 0 && strlen(mo_order[0]) != 0)
    {
        pt = mo_order;
    }
    if (strcmp(dirpath, "/admachine/night_adlist") == 0 && strlen(ni_order[0]) != 0)
    {
        pt = ni_order;
    }
    if (strcmp(dirpath, "/admachine/midday_adlist") == 0 && strlen(mi_order[0]) != 0)
    {
        pt = mi_order;
    }
    if (pt == NULL)
    {
        return;
    }
    while (strcmp(pt[i], ""))
    {
        printf("pt[%d]:%s\n", i, pt[i]);
        i++;
    }
    i = 0;
    while (strcmp(pt[i], ""))
    {

        while (p->next != NULL)
        {
            if (strcmp(p->next->ad_name, pt[i]) == 0)
            {
                move = p->next;
                break;
            }

            p = p->next;
        }
        // 解开节点
        p->next = move->next;
        move->next = NULL;
        // 将节点放到指定位置
        // 移动到要插入位置的前一个节点
        int j = 0;
        while (j != i)
        {
            pos = pos->next;
            j++;
        }
        // 插入节点
        move->next = pos->next;
        pos->next = move;

        i++;
        p = ad_listhead->next;
        pos = ad_listhead;
        move = NULL;
    }
    printf("order:\n");
    ad_info_display();
    // 找出要移动的节点的前一位
}

void creat_thbmp(int new_width, int new_height, char *path)
{
    ad_info *p;
    destroy();
    get_bmp(path);
    usleep(200);
    p = ad_listhead->next;
    char path_a[128] = {0};
    char name[128] = {0};
    while (p != NULL)
    {
        if (strcmp(&p->ad_name[strlen(p->ad_name) - 4], ".bmp") == 0 && strncmp(p->ad_name, "sm", 2) != 0)
        {
            if (strcmp(path, "/mnt/udisk") == 0)
            {
                sprintf(path_a, "/admachine/work_bmp/th%s", p->ad_name);
            }
            else
            {
                sprintf(path_a, "%s/th%s", path, p->ad_name);
            }
            sprintf(name, "th%s", p->ad_name);
            strcpy(p->ad_thumbnail_name, name);
            strcpy(p->ad_thumbnail_path, path_a);
            zoom_bmp(p->ad_path, new_width, new_height, path_a);
        }
        if (strcmp(&p->ad_name[strlen(p->ad_name) - 4], ".avi") == 0)
        {
            sprintf(name, "%s", p->ad_name);
            strcpy(p->ad_thumbnail_name, name);
            strcpy(p->ad_thumbnail_path, "/admachine/work_bmp/tavi.bmp");
        }
        p = p->next;
    }
}

int dele_node(char *name)
{
    ad_info *del = NULL;
    ad_info *p = NULL;
    // 先找到要删除的节点
    del = find_delnode(name);
    if (del == NULL)

    {
        printf("无此数据的节点，无法删除\n");
        return -1;
    }
    // 查找到del的前缀节点
    p = ad_listhead;
    // 地址的判断
    while (p->next != del)

    {
        p = p->next;
    }
    // 节点解除
    p->next = del->next;
    del->next = NULL;
    // 节点释放
    free(del);
    return 0;
}

int inp_password()
{
    struct input_event ts;
    char password[128] = {0};
    char ch[2] = {0};
    int X, Y;
    int low = 0;
    int col = 0;
    while (1)
    {
        // 松开触摸后，再打印
        while (1)
        {
            read(ts_fd, &ts, sizeof(struct input_event));

            // 判断类型
            if (ts.type == EV_ABS && ts.code == ABS_X)
            {
                X = ts.value;
            }

            if (ts.type == EV_ABS && ts.code == ABS_Y)
            {
                Y = ts.value;
            }

            // 判断按下
            if (ts.type == EV_KEY && ts.code == BTN_TOUCH && ts.value == 1)
            {
            }
            // 判断是否松开
            if (ts.type == EV_KEY && ts.code == BTN_TOUCH && ts.value == 0)
            {

                X = X * (800.0 / 1024.0);
                Y = Y * (480.0 / 600.0);
                break;
            }
        }
        if ((X >= 0 && X <= 300 && Y >= 0 && Y <= 480) || (X >= 300 && X <= 400 && Y >= 360 && Y <= 480))
        {
            touch_value(X, Y, ch);
            if (strlen(password) >= 18)
            {
                show_font(300, 120, 30, "输入密码过长", 200, 120, 20, 5, getColor(0, 0, 0, 255));
                printf("密码错误\n");
                usleep(800);
                bzero(ch, sizeof(ch));
                return -1;
            }

            if (strcmp(ch, "b") == 0 && strlen(password) != 0)
            {
                char p[128] = {0};
                strncpy(p, password, strlen(password) - 1);
                strcpy(password, p);
                show_font(300 + ((low - 1) * 50), 0 + (col * 40), 30, "", 50, 40, 20, 5, getColor(0, 234, 217, 153));
                low--;
                if (low == 0 && col > 0)
                {
                    low = 4;
                    col--;
                }
                bzero(ch, sizeof(ch));
                bzero(p, sizeof(p));
            }
            else if (strcmp(ch, "o") != 0 && strcmp(ch, "q") != 0 && strcmp(ch, "b") != 0)
            {
                strcat(password, ch);
                if (low > 3)
                {
                    low = 0;
                }
                col = (strlen(password) - 1) / 4;
                show_font(300 + (low * 50), 0 + (col * 40), 30, ch, 50, 40, 20, 5, getColor(0, 0, 0, 0));
                low++;
                bzero(ch, sizeof(ch));
            }
            if (strcmp(ch, "o") == 0)
            {
                bzero(ch, sizeof(ch));
                return -1;
            }
            if (strcmp(ch, "q") == 0)
            {
                char pwd[128] = {0};
                int fd = open("password.txt", O_RDWR);
                read(fd, pwd, sizeof(pwd));
                if (strcmp(password, pwd) == 0)
                {
                    printf("密码正确\n");
                    show_font(300, 120, 30, "密码正确", 200, 120, 20, 5, getColor(0, 0, 0, 255));
                    sleep(2);
                    close(fd);
                    bzero(ch, sizeof(ch));
                    return 0;
                }
                else
                {
                    show_font(300, 120, 30, "密码错误", 200, 120, 20, 5, getColor(0, 0, 0, 255));
                    printf("密码错误\n");
                    sleep(2);
                    close(fd);
                    bzero(ch, sizeof(ch));
                    return -1;
                }
            }
        }
    }
}

int touch_s(int mode)
{
    int X, Y, out;
    struct input_event ts;
    char showlist_path[128] = {0};
    char addad[248] = {0};

    strcpy(showlist_path, ad_listpath);

    ts_fd = open(TS_PATH, O_RDWR);
    if (ts_fd == -1)
    {
        printf("open ts failure\n");
        return -1;
    }

    while (1)
    {
        // 松开触摸后，再打印
        while (1)
        {
            read(ts_fd, &ts, sizeof(struct input_event));

            // 判断类型
            if (ts.type == EV_ABS && ts.code == ABS_X)
            {
                X = ts.value;
            }

            if (ts.type == EV_ABS && ts.code == ABS_Y)
            {
                Y = ts.value;
            }

            // 判断按下
            if (ts.type == EV_KEY && ts.code == BTN_TOUCH && ts.value == 1)
            {
            }
            // 判断是否松开
            if (ts.type == EV_KEY && ts.code == BTN_TOUCH && ts.value == 0)
            {
                X = X * (800.0 / 1024.0);
                Y = Y * (480.0 / 600.0);
                break;
            }
        }

        if (X >= 700 && X <= 800 && Y >= 0 && Y <= 100 && mode == 0)
        {
            printf("输入密码\n");
            // 暂停所有图片输出
            pthread_cancel(tids[0]);
            pthread_cancel(tids[1]);
            pthread_cancel(tids[2]);
            pthread_cancel(tids[3]);
            system("killall -9 mplayer");
            show_keyboard();
            out = inp_password();
            if (out == -1)
            {
                return -1;
            }
            if (out == 0)
            {
                return 0;
            }
        }
        // 返回
        if (X >= 0 && X <= 130 && Y >= 0 && Y <= 96 && mode == ADMS)
        {
            playing = false;
            system("killall -9 mplayer");
            bzero(choose_ad_name, sizeof(choose_ad_name));
            bzero(choose_ad, sizeof(choose_ad));
            close(ts_fd);
            return 0;
        }
        if (X >= 0 && X <= 130 && Y >= 97 && Y <= 189 && mode == ADMS)
        {
            playing = false;
            system("killall -9 mplayer");
            bzero(choose_ad_name, sizeof(choose_ad_name));
            bzero(choose_ad, sizeof(choose_ad));
            bzero(showlist_path, sizeof(show_adlist));
            show_bmp(0, 0, "work_bmp/ad_UI.bmp");
            sprintf(showlist_path, "%s/%s", work_path, time_list[0]);
            show_adlist(showlist_path);
        }
        if (X >= 0 && X <= 130 && Y >= 195 && Y <= 284 && mode == ADMS)
        {
            playing = false;
            system("killall -9 mplayer");
            bzero(choose_ad_name, sizeof(choose_ad_name));
            bzero(choose_ad, sizeof(choose_ad));
            bzero(showlist_path, sizeof(show_adlist));
            show_bmp(0, 0, "work_bmp/ad_UI.bmp");
            sprintf(showlist_path, "%s/%s", work_path, time_list[1]);
            show_adlist(showlist_path);
        }
        if (X >= 0 && X <= 130 && Y >= 290 && Y <= 381 && mode == ADMS)
        {
            playing = false;
            system("killall -9 mplayer");
            bzero(choose_ad_name, sizeof(choose_ad_name));
            bzero(choose_ad, sizeof(choose_ad));
            bzero(showlist_path, sizeof(show_adlist));
            show_bmp(0, 0, "work_bmp/ad_UI.bmp");
            sprintf(showlist_path, "%s/%s", work_path, time_list[2]);
            show_adlist(showlist_path);
        }
        if (X >= 0 && X <= 130 && Y >= 381 && mode == ADMS)
        {
            playing = false;
            system("killall -9 mplayer");
            bzero(showlist_path, sizeof(show_adlist));
            show_bmp(0, 0, "work_bmp/ad_UI.bmp");
            strcpy(showlist_path, "/mnt/udisk");
            show_adlist(showlist_path);
        }
        if (X >= 132 && X <= 527 && Y >= 99 && Y <= 480 && mode == ADMS && playing == false)
        {
            playing = false;
            system("killall -9 mplayer");
            bzero(choose_ad_name, sizeof(choose_ad_name));
            bzero(choose_ad, sizeof(choose_ad));
            int a = (X - 132) / 96 + 1;
            int b = (Y - 99) / 92 + 1;
            int i = a + (b - 1) * 4;
            find_node(i);
        }
        if (X >= 533 && X <= 800 && Y >= 387 && Y <= 480 && mode == ADMS)
        {
            playing = false;
            if (strcmp(choose_ad, "") == 0)
            {
                show_font(600, 20, 20, "未选择图片或视频", 150, 50, 20, 20, getColor(0, 0, 0, 0));
                sleep(1);
                show_font(600, 20, 20, "", 150, 50, 20, 20, getColor(0, 234, 217, 153));
            }
            else
            {
                show_font(540, 20, 20, "选择一个目录", 180, 60, 20, 20, getColor(0, 0, 0, 0));
                char *p = NULL;
                p = add_ad();
                sprintf(showlist_path, "%s/%s", work_path, p);
                // 判断目标文件夹是否有同名文件
                int out = is_namesame(showlist_path);
                if (out == -1)
                {
                    show_font(540, 20, 20, "目标目录有同名文件", 180, 60, 20, 20, getColor(0, 0, 0, 0));
                    sleep(1);
                    bzero(choose_ad_name, sizeof(choose_ad_name));
                    bzero(choose_ad, sizeof(choose_ad));
                }
                else if (out == 0)
                {
                    char(*pt)[48] = NULL;
                    char p[48] = {0};
                    char p_c[48] = {0};
                    int i = 0;
                    if (strcmp(showlist_path, "/admachine/morning_adlist") == 0)
                    {
                        pt = mo_order;
                    }
                    if (strcmp(showlist_path, "/admachine/night_adlist") == 0)
                    {
                        pt = ni_order;
                    }
                    if (strcmp(showlist_path, "/admachine/midday_adlist") == 0)
                    {
                        pt = mi_order;
                    }
                    while (strcmp(pt[i], ""))
                    {
                        i++;
                    }
                    strcpy(pt[i], choose_ad_name);
                    sprintf(addad, "cp %s %s", choose_ad, showlist_path);
                    system(addad);
                    show_font(540, 20, 20, "添加成功", 180, 60, 20, 20, getColor(0, 0, 0, 0));
                    sleep(1);
                    bzero(choose_ad_name, sizeof(choose_ad_name));
                    bzero(choose_ad, sizeof(choose_ad));
                }
            }
        }
        if (X >= 533 && X <= 800 && Y >= 291 && Y <= 381 && mode == ADMS && (strcmp(showlist_path, "/mnt/udisk") != 0))
        {
            playing = false;
            if (strcmp(choose_ad, "") == 0)
            {
                show_font(600, 20, 20, "未选择图片或视频", 150, 50, 20, 20, getColor(0, 0, 0, 0));
                sleep(1);
                show_font(600, 20, 20, "", 150, 50, 20, 20, getColor(0, 234, 217, 153));
            }
            else
            {
                char(*pt)[48] = NULL;
                char p[48] = {0};
                char p_c[48] = {0};
                int i = 0;
                if (strcmp(showlist_path, "/admachine/morning_adlist") == 0)
                {
                    pt = mo_order;
                }
                if (strcmp(showlist_path, "/admachine/night_adlist") == 0)
                {
                    pt = ni_order;
                }
                if (strcmp(showlist_path, "/admachine/midday_adlist") == 0)
                {
                    pt = mi_order;
                }
                while (strcmp(pt[i], ""))
                {
                    if (strcmp(pt[i], choose_ad_name) == 0)
                    {
                        break;
                    }
                    i++;
                }
                int j = i;
                while (strcmp(pt[j], ""))
                {
                    strcpy(pt[i], pt[i + 1]);
                    i++;
                    j++;
                }
                bzero(pt[j], sizeof(pt[j]));
                sprintf(addad, "rm %s/*%s", showlist_path, choose_ad_name);
                system(addad);
                show_font(540, 20, 20, "移除成功", 170, 60, 20, 20, getColor(0, 0, 0, 0));
                sleep(1);
                show_bmp(0, 0, "work_bmp/ad_UI.bmp");
                dele_node(choose_ad_name);
                bzero(choose_ad_name, sizeof(choose_ad_name));
                bzero(choose_ad, sizeof(choose_ad));
                show_adlist(showlist_path);
            }
        }
        if (X >= 533 && X <= 800 && Y >= 195 && Y <= 285 && mode == ADMS)
        {
            playing = false;
            if (strcmp(choose_ad, "") == 0)
            {
                show_font(600, 20, 20, "未选择图片或视频", 150, 50, 20, 20, getColor(0, 0, 0, 0));
                sleep(1);
                show_font(600, 20, 20, "", 150, 50, 20, 20, getColor(0, 234, 217, 153));
            }
            else
            {
                show_bmp(0, 0, "work_bmp/ad_UI.bmp");
                if (strcmp(&choose_ad_name[strlen(choose_ad_name) - 4], ".bmp") == 0)
                {
                    playing = true;
                    zoom_bmp(choose_ad, 396, 200, "detail.bmp");
                    show_font(132, 299, 30, choose_ad_name, 150, 50, 20, 20, getColor(0, 234, 217, 153));
                    show_bmp(132, 99, "detail.bmp");
                    system("rm detail.bmp");
                }
                else if (strcmp(&choose_ad_name[strlen(choose_ad_name) - 4], ".avi") == 0)
                {
                    system("killall -9 mplayer");
                    pthread_create(&tids[4], NULL, show_video, NULL);
                    playing = true;
                }
            }
        }
        if (X >= 533 && X <= 800 && Y >= 99 && Y <= 188 && mode == ADMS && (strcmp(showlist_path, "/mnt/udisk") != 0))
        {
            playing = false;
            if (strcmp(choose_ad, "") == 0)
            {
                show_font(600, 20, 20, "未选择图片或视频", 150, 50, 20, 20, getColor(0, 0, 0, 0));
                sleep(1);
                show_font(600, 20, 20, "", 150, 50, 20, 20, getColor(0, 234, 217, 153));
            }
            else
            {
                char(*pt)[48] = NULL;
                char p[48] = {0};
                char p_c[48] = {0};
                int i = 0;
                if (strcmp(showlist_path, "/admachine/morning_adlist") == 0)
                {
                    pt = mo_order;
                }
                if (strcmp(showlist_path, "/admachine/night_adlist") == 0)
                {
                    pt = ni_order;
                }
                if (strcmp(showlist_path, "/admachine/midday_adlist") == 0)
                {
                    pt = mi_order;
                }
                strcpy(p, pt[0]);
                while (strcmp(pt[i], ""))
                {
                    if (strcmp(pt[i], choose_ad_name) == 0)
                    {
                        break;
                    }

                    i++;
                }
                strcpy(p_c, pt[i]);
                strcpy(pt[i], p);
                strcpy(pt[0], p_c);

                show_font(540, 20, 20, "移动成功", 170, 60, 20, 20, getColor(0, 0, 0, 0));
                sleep(1);
                show_bmp(0, 0, "work_bmp/ad_UI.bmp");
                bzero(choose_ad_name, sizeof(choose_ad_name));
                bzero(choose_ad, sizeof(choose_ad));
                show_adlist(showlist_path);
            }
        }
    }
}

void creat_minibmp(int new_width, int new_height)
{
    ad_info *p;
    // 用于遍历的节点结构体变量
    p = ad_listhead->next; // p第一个数据节点
    char path[128] = {0};
    char name[128] = {0};
    while (p != NULL) // 说明数据节点存在
    {
        if (strcmp(&p->ad_name[strlen(p->ad_name) - 4], ".bmp") == 0)
        {
            sprintf(path, "%s/sm%s", ad_listpath, p->ad_name);
            sprintf(name, "sm%s", p->ad_name);
            strcpy(p->ad_mini_path, path);
            strcpy(p->ad_mini_name, name);
            zoom_bmp(p->ad_path, new_width, new_height, path);
        }

        p = p->next;
    }
    printf("\n");
}

void changbmp_zoom(int new_width, int new_height)
{
    ad_info *p;
    // 用于遍历的节点结构体变量
    p = ad_listhead->next; // p第一个数据节点
    while (p != NULL)      // 说明数据节点存在
    {
        if (strcmp(&p->ad_name[strlen(p->ad_name) - 4], ".bmp") == 0 && strncmp(p->ad_name, "sm", 2) != 0)
        {
            zoom_bmp(p->ad_path, new_width, new_height, p->ad_path);
        }
        p = p->next;
    }
    printf("\n");
}

int zoom_bmp(char *old_bmppath, int new_width, int new_height, char *new_bmppath)
{

    char *oldPhoto = old_bmppath, *newPhoto = new_bmppath;
    FILE *fp1 = fopen(oldPhoto, "r+");

    if (fp1 == NULL)
    {
        printf("Opening photos failed!\n");
        if (fp1 == NULL)
            fclose(fp1);
        return -1;
    }

    // 单独读取bmp图片文本标识符0x4d42
    unsigned short fileType;
    fread(&fileType, sizeof(unsigned short), 1, fp1);

    if (fileType != 0x4d42)
    { // 如果不是的话证明不是bmp图片
        printf("The photo is not of bmp type!\n");
        return -1;
    }

    fseek(fp1, 0, SEEK_SET);

    // 读取原图信息
    struct tagBITMAPFILEHEADER fileHeader; // 原图文件头
    struct tagBITMAPINFOHEADER infoHeader; // 原图消息头
    fread(&fileHeader, sizeof(struct tagBITMAPFILEHEADER), 1, fp1);
    fread(&infoHeader, sizeof(struct tagBITMAPINFOHEADER), 1, fp1);
    int byte = infoHeader.biBitCount / 8;
    int sum = 1;
    for (int i = 0; i < infoHeader.biBitCount; i++)
    {
        sum = sum * 2;
    }
    // 每个像素的字节数
    struct tagRGBQUAND *palette = (struct tagRGBQUAND *)malloc(sum * 4); // 分配调色板空间
    if (infoHeader.biBitCount != 24)                                     // 如果是24位图的没有调色板
        fread(palette, sizeof(struct tagRGBQUAND), sum,
              fp1);

    // 得到原图宽高和修改后的宽高
    unsigned int oldWidth, oldHeight, newWidth, newHeight;
    oldWidth = infoHeader.biWidth;
    oldHeight = infoHeader.biHeight;
    double width_pzoom = (double)new_width / (double)oldWidth;
    double height_pzoom = (double)new_height / (double)oldHeight;

    // printf("width_pzoom:%f  height_pzoom:%f\n", width_pzoom, height_pzoom);
    // printf("Oldphoto's height:%d\n", oldHeight);
    // printf("Oldphoto's width:%d\n", oldWidth);
    // 图像显示不出来原因在于图像长或宽不是4的倍数
    // 下面这一步可以保证得到的宽高是4的倍数
    newHeight = ((int)(oldHeight * height_pzoom) + 3) / 4 * 4;
    newWidth = ((int)(oldWidth * width_pzoom) + 3) / 4 * 4;

    // newHeight = (int)(oldHeight * pzoom);
    // newWidth = (int)(oldWidth * pzoom);
    // printf("Newphoto's height:%d\n", newHeight);
    // printf("Newphoto's width:%d\n", newWidth);
    unsigned int oldsize = oldWidth * oldHeight * byte, // byte = 3
        newsize = newWidth * newHeight * byte;
    if (oldHeight == newHeight && oldWidth == newWidth && strcmp(old_bmppath, new_bmppath) == 0)
    {
        // printf("无需修改\n");
        return 0;
    }
    // 获取原图位图数据
    unsigned char *sourceData = (unsigned char *)malloc(oldsize);
    if (infoHeader.biBitCount == 24)
    {                             // 无调色板时
        fseek(fp1, 54, SEEK_SET); // 文件指针指向文件的第54个字节
        fread(sourceData, oldsize, 1, fp1);
    }
    else if (infoHeader.biBitCount == 8)
    {                               // 有调色板是要加上分配调色板所需要的空间
        fseek(fp1, 1078, SEEK_SET); // 文件指针指向文件的第54+2^8*4=1078个字节
        fread(sourceData, oldsize, 1, fp1);
    }

    // 修改两个header的数据并把修改后的header(及调色板信息)写入新图片中
    infoHeader.biWidth = newWidth;
    infoHeader.biHeight = newHeight;
    if (infoHeader.biBitCount == 24)
    {
        fileHeader.bfSize = 54 + newsize;
        infoHeader.biSizeImage = newsize;
        printf("fileHeader.bfSize:%#x\n", fileHeader.bfSize);
    }
    else if (infoHeader.biBitCount == 8)
    {
        fileHeader.bfSize = 1078 + newsize;
        infoHeader.biSizeImage = newsize;
    }

    FILE *fp2 = fopen(newPhoto, "w+");
    fseek(fp2, 0, SEEK_SET);

    // fwrite(&fileType, sizeof(unsigned short), 1, fp2);

    fwrite(&fileHeader, sizeof(struct tagBITMAPFILEHEADER), 1, fp2);

    fwrite(&infoHeader, sizeof(struct tagBITMAPINFOHEADER), 1, fp2);
    sum = 1;
    for (int i = 0; i < infoHeader.biBitCount; i++)
    {
        sum = sum * 2;
    }
    if (infoHeader.biBitCount != 24)
    {
        fwrite(palette, sizeof(struct tagRGBQUAND), sum, fp2);
        printf("error\n");
    }

    // 使用双线性差值法进行图片缩放
    double p, q;
    unsigned int x1, y1, x2, y2; // 原图所在像素点的宽高
    unsigned int X, Y;
    unsigned char *pDestination; // 修改像素的位置（即字节偏移量）
    unsigned char a, b, c;
    unsigned char *pSource1 = &a, *pSource2 = &b; // 获取像素的位置（即字节偏移量）
    unsigned char *destinationData =
        (unsigned char *)malloc(newsize); // 开好新图片的位图数据所需空间
    for (Y = 0; Y < newHeight; Y++)
    {
        y1 = Y / height_pzoom;
        y2 = Y / height_pzoom + 1;
        q = Y / height_pzoom - y1;
        pDestination = destinationData + Y * newWidth * byte;
        pSource1 = sourceData + y1 * oldWidth * byte;
        pSource2 = sourceData + y2 * oldWidth * byte;
        for (X = 0; X < newWidth; X++)
        {
            x1 = X / width_pzoom;
            x2 = X / width_pzoom + 1;
            p = X / width_pzoom - x1;
            if (byte == 3)
            {
                *(pDestination + X * byte) =
                    *(pSource1 + x1 * byte) * (1 - p) * (1 - q) +
                    *(pSource1 + x2 * byte) * p * (1 - q) +
                    *(pSource2 + x1 * byte) * (1 - p) * q +
                    *(pSource2 + x2 * byte) * p * q;

                *(pDestination + X * byte + 1) =
                    *(pSource1 + x1 * byte + 1) * (1 - p) * (1 - q) +
                    *(pSource1 + x2 * byte + 1) * p * (1 - q) +
                    *(pSource2 + x1 * byte + 1) * (1 - p) * q +
                    *(pSource2 + x2 * byte + 1) * p * q;

                *(pDestination + X * byte + 2) =
                    *(pSource1 + x1 * byte + 2) * (1 - p) * (1 - q) +
                    *(pSource1 + x2 * byte + 2) * p * (1 - q) +
                    *(pSource2 + x1 * byte + 2) * (1 - p) * q +
                    *(pSource2 + x2 * byte + 2) * p * q;
            }
            else if (byte == 1)
            {
                *(pDestination + X * byte) =
                    *(pSource1 + x1 * byte) * (1 - p) * (1 - q) +
                    *(pSource1 + x2 * byte) * p * (1 - q) +
                    *(pSource2 + x1 * byte) * (1 - p) * q +
                    *(pSource2 + x2 * byte) * p * q;
            }
        }
    }

    // 将位图数据写入新的图片并进行后续处理
    fwrite(destinationData, newsize, 1, fp2);
    printf("success!\n");
    free(destinationData);
    free(sourceData);
    free(palette);
    fclose(fp1);
    fclose(fp2);
    return 0;
}

struct LcdDevice *init_lcd(const char *device)
{
    // 申请空间
    struct LcdDevice *lcd = malloc(sizeof(struct LcdDevice));
    if (lcd == NULL)
    {
        return NULL;
    }

    // 1打开设备
    lcd->fd = open(device, O_RDWR);
    if (lcd->fd < 0)
    {
        perror("open lcd fail");
        free(lcd);
        return NULL;
    }

    // 映射
    lcd->mp = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd->fd, 0);

    for (int i = 0; i < 800 * 480; i++)
    {
        lcd->mp[i] = 0x002E1BCC;
    }

    return lcd;
}

void get_time(char *out_time, char *now_week)
{
    time_t timep;

    struct tm *p;
    char week[7][50] = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};
    char time1[50] = {0};

    time(&timep);

    p = gmtime(&timep);

    // 将它们合成一个字符串
    now_hour = p->tm_hour;

    sprintf(time1, "%d年%d月%d日 %d:%d:%d", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

    strcpy(out_time, time1);
    strcpy(now_week, week[p->tm_wday]);
}

void get_bmp(char *dirpath)
{
    DIR *pdir;
    struct dirent *pdirent;
    ad_info *new;
    char ad_path[128] = {0};
    pdir = opendir(dirpath);
    int i = 1;
    if (pdir == NULL)
    {
        printf("opendir fail\n");
        return;
    }

    while ((pdirent = readdir(pdir)) != NULL)
    {
        if (strcmp(&pdirent->d_name[strlen(pdirent->d_name) - 4], ".bmp") != 0 && strcmp(&pdirent->d_name[strlen(pdirent->d_name) - 4], ".avi") != 0)
        {
            continue;
        }
        if (strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0 || strncmp(pdirent->d_name, "sm", 2) == 0 || strncmp(pdirent->d_name, "th", 2) == 0)
        {
            continue;
        }

        sprintf(ad_path, "%s/%s", dirpath, pdirent->d_name);

        new = init_adinfo(pdirent->d_name, ad_path, i);
        insert_adinfo(ad_listhead, new);
        bzero(ad_path, sizeof(ad_path));
        i++;
    }
    order_adlist(dirpath);
    closedir(pdir);
}

void touch_value(int x, int y, char *ch)
{
    if (x >= 0 && x <= 100 && y >= 0 && y <= 120)
    {
        strcpy(ch, "1");
    }
    if (x >= 100 && x <= 200 && y >= 0 && y <= 120)
    {
        strcpy(ch, "2");
    }
    if (x >= 200 && x <= 300 && y >= 0 && y <= 120)
    {
        strcpy(ch, "3");
    }
    if (x >= 0 && x <= 100 && y >= 120 && y <= 240)
    {
        strcpy(ch, "4");
    }
    if (x >= 100 && x <= 200 && y >= 120 && y <= 240)
    {
        strcpy(ch, "5");
    }
    if (x >= 200 && x <= 300 && y >= 120 && y <= 240)
    {
        strcpy(ch, "6");
    }
    if (x >= 0 && x <= 100 && y >= 240 && y <= 360)
    {
        strcpy(ch, "7");
    }
    if (x >= 100 && x <= 200 && y >= 240 && y <= 360)
    {
        strcpy(ch, "8");
    }
    if (x >= 200 && x <= 300 && y >= 240 && y <= 360)
    {
        strcpy(ch, "9");
    }
    if (x >= 0 && x <= 100 && y >= 360 && y <= 480)
    {
        strcpy(ch, "o");
    }
    if (x >= 200 && x < 300 && y >= 360 && y <= 480)
    {
        strcpy(ch, "q");
    }
    if (x >= 100 && x <= 200 && y >= 360 && y <= 480)
    {
        strcpy(ch, "0");
    }
    if (x > 300 && x <= 400 && y >= 360 && y <= 480)
    {
        strcpy(ch, "b");
    }
}