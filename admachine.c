#include "font.h"
#include "show.h"
#include "tool.h"
#include "struct.h"
#define INP_PWD 0

ad_info *order_listhead;

int main(void)
{
    // 初始化各项
    int out;
    char time[128] = {0};
    char week[30] = {0};
    ad_listhead = init_adinfo("", "", 0);
    order_listhead = init_adinfo("", "", 0);

    lcd = init_lcd("/dev/fb0");

    getcwd(work_path, sizeof(work_path));

    // 时间切换时自动切换播放列表while包装成函数并弄个线程，切换线程杀死并重新运行（未实现）

    while (1)
    {

        get_time(time, week);
        // sprintf(ad_listpath, "%s/morning_adlist", work_path);
        if (now_hour >= 7 && now_hour <= 12)
        {
            sprintf(ad_listpath, "%s/%s", work_path, time_list[0]);
        }
        else if (now_hour >= 13 && now_hour <= 18)
        {
            sprintf(ad_listpath, "%s/%s", work_path, time_list[1]);
        }
        else if (now_hour >= 19 || (now_hour >= 0 && now_hour <= 6))
        {
            sprintf(ad_listpath, "%s/%s", work_path, time_list[2]);
        }
        printf("%s\n", ad_listpath);

        for (int i = 0; i < 800 * 480; i++)
        {
            lcd->mp[i] = 0x002E1BCC;
        }
        // show_bmp(0, 0, "/admachine/work_bmp/home_ui.bmp");
        //  时间显示
        pthread_create(&tids[0], NULL, show_time, NULL);
        // 播放广告
        pthread_create(&tids[3], NULL, show_news, NULL);
        // 要显示的广告列表
        get_bmp(ad_listpath);

        ad_info_display();
        // 修改文件夹中所有照片大小
        changbmp_zoom(536, 424);
        // 生成副屏幕小广告
        creat_minibmp(236, 292);
        // 循环播放小广告
        pthread_create(&tids[1], NULL, traverse_smadlist, NULL);
        // 循环播放大屏幕广告和视频
        pthread_create(&tids[2], NULL, traverse_adlist, NULL);

        out = touch_s(INP_PWD);
        if (out == 0)
        {
            // 登录成功跳转界面界面
            pthread_detach(tids[0]);
            pthread_cancel(tids[0]);
            show_bmp(0, 0, "work_bmp/ad_UI.bmp");
            show_adlist(ad_listpath);
            touch_s(ADMS);
        }

        ad_listhead->next = NULL;

        // 循环最后应该是管理员退出管理界面的行为

        system("rm /admachine/work_bmp/th*");
    }

    pthread_exit(NULL);

    return 0;
}