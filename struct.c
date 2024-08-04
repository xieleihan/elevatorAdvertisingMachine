#include "struct.h"

void insert_adinfo(ad_info *head, ad_info *new)
{

    // 右边表示值，左边表示变量
    new->next = head->next; // head->next表示头节点后缀节点的首地址
    head->next = new;
}

ad_info *init_adinfo(char *adname, char *adpath, int order)
{
    ad_info *new;

    new = (ad_info *)malloc(sizeof(ad_info));

    if (new == NULL)

    {
        printf("malloc fail\n");
        return NULL;
    }
    strcpy(new->ad_name, adname);
    strcpy(new->ad_path, adpath);
    new->order = order;
    bzero(new->ad_mini_name, sizeof(new->ad_mini_name));
    bzero(new->ad_mini_path, sizeof(new->ad_mini_path));
    bzero(new->ad_thumbnail_name, sizeof(new->ad_thumbnail_name));
    bzero(new->ad_thumbnail_path, sizeof(new->ad_thumbnail_path));
    new->next = NULL;

    return new;
}

void ad_info_display()
{
    ad_info *p;
    // 用于遍历的节点结构体变量
    p = ad_listhead->next; // p第一个数据节点
    while (p != NULL)      // 说明数据节点存在
    {
        printf("%s\t", p->ad_name);
        printf("%d\n", p->order);
        p = p->next;
    }
}

ad_info *find_delnode(char *name)
{
    ad_info *p;

    // p头节点的下一个节点
    p = ad_listhead->next;
    while (p != NULL)

    {
        if (strcmp(name, p->ad_name) == 0)
            return p;
        // 返回查找到的数据节点的地址
        p = p->next;
    }

    return NULL;
}

void destroy()
{
    ad_info *p = ad_listhead->next;
    ad_info *temp; // 指针变量 存地址
    while (p != NULL)
    {
        // 保存当前节点地址
        temp = p;
        // 移动下一个节点
        p = p->next;
        free(temp);
    }
    // 头节点指向NULL
    ad_listhead->next = NULL;
}