//
//  main.c
//  4_内存可变分区分配算法的分配和回收
//
//  Created by Air on 2020/5/19.
//  Copyright © 2020 Lucifer. All rights reserved.
//

#include<stdio.h>
#include<stdlib.h>

typedef struct linkk//建立动态链表---存放内存空闲分区的信息
{
    int size;
    int addr;
    struct linkk *next;
}LINK;

 typedef struct jobtab//建立一个结构体---存放作业的信息
{
    int jno;
    int size;
    int addr;
    int flag;
}TAB;


LINK *ahead,*head,*p,*q;//定义一个临时前驱结点、头结点指针以及两个常用的临时指针

TAB tab[20];//定义存放作业的结构体数组大小为20（大小可调，全局替换）

int memory_size=0;//定义一个全局变量---内存空间的大小，（后面紧凑的时候用得着）

void allocm()
{
    int n=0;
    printf("\n请输入作业号:");
    scanf("%d",&n);
    
    if(tab[n].flag!=0)   //检测作业是否已经分配内存（已经分配内存）
    {
        printf("\n此作业已分配内存!\n");
        return;
    }
    
    int length=0;
    printf("\n请输入分配内存大小:");
    scanf("%d",&length);
    
    p=head;//头指针head指向不能修改，因此将其赋值给临时指针变量”p=head“
    
    if(head->next!=NULL)//判断是否已经回收过空间，没有回收过空间那就是按顺序自底向上分割内存空间，也就是默认为首次适应（此时总共就一块空闲分区）
    {
    flash:
        printf("\n1、首次适应 2、最佳适应 3、最坏适应\n\n请选择：");
        int choice=0;
        scanf("%d",&choice);
        
        if(choice==1)//首次适应
        {
            while(p->size<length)
            {
                p=p->next;//指针后移一位
                if(p==NULL)
                {
                    tab[n].flag=0;
                    printf("\n没有足够的内存空间可以分配!\n");
                    return;
                }
            }
            
//           while(p!=NULL)            //第二种写法，与第一种没有本质区别，在下面也会用上
//           {
//                if(p->size<length)
//                {
//                    p=p->next;
//                    if(p==NULL)
//                    {
//                        tab[n].flag=0;
//                        printf("\n没有足够的内存空间可以分配!\n");
//                        return;
//                    }
//                }
//                if(p->size>length)
//                    break;
//           }
            
        }
        
        if(choice==2)//最佳适应
        {
            while(p->size<length)//首次适应，先按顺序找到一个内存足够大的空闲分区，但不一定是最佳
            {
                p=p->next;
                if(p==NULL)
                {
                    tab[n].flag=0;
                    printf("\n没有足够的内存空间可以分配!\n");
                    return;
                }
            }
            
            //printf("test");
            
            q=p;
            //p=head;                          //                                 如果把这句删掉更好，可以减少资源占用
            p=p->next;
            
            while(p!=NULL)                  //                                  此处还有一种写法，省略不写，懒得写
            {
                if(p->size<length || p->size>=q->size)//如果空闲分区的大小比所要申请的内存小，下一个
                {
                    p=p->next;
                    continue;
                }
                if(p->size<q->size)//此处原理取自选择排序
                {
                    q=p;
                    p=p->next;
                }
            }
            p=q;//将指针p指向的地址赋值给p
            
        }
        
        if(choice==3)//最坏适应
        {
            while(p->size<length)
            {
                p=p->next;
                if(p==NULL)
                {
                    tab[n].flag=0;
                    printf("\n没有足够的内存空间可以分配!\n");
                    return;
                }
            }
            
            //printf("test");
            
            
            q=p;
            //p=head;
            p=p->next;
            while(p!=NULL)
            {
                if(p->size<q->size)
                {
                    p=p->next;
                    continue;
                }
                if(p->size>q->size)
                {
                    q=p;
                    p=p->next;
                }
            }
            p=q;
        }
        
        if(choice!=1 && choice!=2 && choice!=3)//如果选项不在规定内，则重选
            goto flash;//此处用goto语句的用法，仔细体会
        
    }
    
    if(tab[n].flag==0)   //检测作业是否已经分配内存（尚未分配内存），此处的if语句纯粹是想把下面三个语句框起来
    {    //此段代码看似冗余，其实是回收前分配内存和回收后分配内存共用一段代码
        tab[n].flag=1;//打上已经分配了内存的标志
        tab[n].size=length;
        tab[n].addr=p->addr;
    }
    
    if(p->size>=length)
    {
        tab[n].addr=p->addr;
        p->size-=length;
        p->addr+=length;
        return;
    }
    
    if(p->size<length)//判断是否有足够的内存可以分配给作业
    {

        tab[n].flag=0;
        printf("\n没有足够的内存空间可以分配!\n");
        return;
        
    }
}

void freem()
{
    ahead=(LINK *)malloc(sizeof(LINK));
    int n=0;
    printf("\n请输入要回收空间的作业号：");
    scanf("%d",&n);
    
    if(tab[n].flag==0)
    {
        printf("\n该作业尚未分配空间或者空间已经被回收！\n");
        return ;
    }
       
    tab[n].flag=0;
    ahead->addr=tab[n].addr;
    ahead->size=tab[n].size;
    
    if(ahead->addr<head->addr)
    {
        if(ahead->addr+ahead->size==head->addr)   //被释放空间前面没有任何空间，后面有可以合并的空闲空间，此时没必要插入空闲空间链表，只需要扩充第一个节点的空间即可
        {
            head->size+=ahead->size;
            head->addr-=ahead->size;
            //free(ahead);
            return;
        }
        
        if(ahead->addr+ahead->size!=head->addr) //   <=  的条件也是可以的
        {
            ahead->next=head;
            head=ahead;
            return;
        }
    }
    
    if(ahead->addr>head->addr)
    {
        p=head;
        while(p->addr<ahead->addr)//将回收的空间，插入到空闲分区链表的合适的位置
        {
          q=p;
          q=q->next;
          if(q->addr>ahead->addr || q==NULL)// q指向p下一个结点，判断下一个结点的位置是否比回收的空间的地址大，也就是说再往下判断一位。如果是或者后一位为空，则插入。
              break;
          if(q->addr<ahead->addr)
              p=p->next;
        }
        
//        if(q==NULL)
//            if(p->addr+p->size==ahead->addr)
//            {
//                p->size+=ahead->size;
//                p->next=NULL;
//            }
        
        if((p->addr+p->size!=ahead->addr) && (ahead->addr+ahead->size!=q->addr))//被释放空间的前后都没有空闲区可以合并
        {
            p->next=ahead;
            ahead->next=q;
            return;
        }
        
        if((p->addr+p->size==ahead->addr) && (ahead->addr+ahead->size!=q->addr))//被释放空间前方有可合并的空闲空间
        {
            p->size+=ahead->size;
            return;
        }
        
        if((p->addr+p->size!=ahead->addr) && (ahead->addr+ahead->size==q->addr))//被释放空间后方有可合并的空闲空间
        {
            q->addr=ahead->addr;
            q->size+=ahead->size;
            return;
        }
        
        if((p->addr+p->size==ahead->addr) && (ahead->addr+ahead->size==q->addr))//被释放空间前、后方有可合并的空闲空间
        {
            p->size+=ahead->size+q->size;
            q=q->next;
            p->next=q;
            return;
        }
    }
}

void printlink()
{
   

    int i=0;
 
    if(head!=NULL)
    {
        printf("\n\t空闲分区号\t分区地址\t分区大小\t\n");
        printf("\n    ---------------------------\n");
    }
     q=head;
    
    i=1;
    while(q!=NULL)
    {
        
        //printf("    %d\t         %d\t             %d\t\n",i,q->addr,q->size);
        printf("    %d\t\t    %d\t\t   %d\t\n",i,q->addr,q->size);
        q=q->next;
        i++;
    }
  return;
}

void printtab()
{
    int f=0,g=0,h=0,i=0,j=0,k=0,flag=0,tab_compact[20];
    
    
    
    for(i=0;i<20;i++)
        tab_compact[i]=-1;
    
    int tab_min=0;//原理：选择排序
    
    
    
    for(i=0;i<20;i++)
    {//此处借用堆的原理
        
        for(g=0;g<20;g++)//先找到第一个已经分配了内存的作业，随便找一个就行
        {
            f=0;
            for(h=0;h<20;h++)
                if(tab_compact[h]==g)
                    f=1;
            if(f==1)
                continue;
            
            if(tab[g].flag==1)
            {
                tab_min=g;
                break;
            }
        }
        
        for(j=0;j<20;j++)
        {
            //tab_min=0;
            f=0;
            for(h=0;h<20;h++)
                if(tab_compact[h]==j)
                    f=1;
            if(f==1)
                continue;
            
            if(tab[j].flag==1)
                if(tab[j].addr<tab[tab_min].addr)
                    tab_min=j; //找出当前地址最小的作业，压入数组
        }
        
        if(tab_min==tab_compact[k-1])
            break;
        tab_compact[k]=tab_min;
        k++;
        
    }
    
    
    
    
    for(i=0;i<20 && flag==0;i++)
        if(tab[i].flag==1)  flag=1;
    
        if(flag==0)
        {
            printf("\n尚未有任何作业申请空间！");
            return;
        }
        printf("\n\t作业号\t作业地址\t作业长度\t\n");
        printf("\n    ------------------------\n");
//        for(i=0;i<20;i++)
//            if(tab[i].flag==1)
//                printf("  %d\t   %d\t    %d\n",i,tab[i].addr,tab[i].size);
    
    for(i=0;i<20;i++)     //示例代码此处没有这个功能，作业没有按起始地址由低到高排列
    {
        if(tab_compact[i]==-1)
            break;
        
        if(tab_compact[i]!=-1)
        {
            printf("    %d\t    %d\t    %d\t\n",tab_compact[i],tab[tab_compact[i]].addr,tab[tab_compact[i]].size);
        }
        
    }
    return;
}

void compact()//紧凑
{
    int tab_size=0,link_size=0,f=0,g=0,h=0,i=0,j=0;
    int tab_compact[20],k=0;//
    
    for(i=0;i<20;i++)
        tab_compact[i]=-1;
    
    for(i=0;i<20;i++)
        if(tab[i].flag==1)
        tab_size+=tab[i].size;//统计作业总共占用了多少内存空间
    
    link_size=memory_size-tab_size;
    head->addr=tab_size;
    head->size=link_size;
    head->next=NULL;  //把链表头结点以后的部分掐断
    
    int tab_min=0;//原理：选择排序
    
//    for(i=0;i<20;i++)//先找到第一个已经分配了内存的作业，随便找一个就行          这一段写错了，引以为戒！！！
//        if(tab[i].flag==1)
//        {
//            tab_min=i;
//            break;
//        }
//
//    for(i=0;i<20;i++)//此处借用堆的原理
//        for(j=0;j<20;j++)
//        {
//            //tab_min=0;
//            if(tab[j].flag==1)
//                if(tab[j].addr<tab[tab_min].addr)
//                {
//                    tab_min=j; //找出当前地址最小的作业，压入数组
//                    tab_compact[k]=tab_min;
//                    k++;
//                }
//
//        }
    
    
    for(i=0;i<20;i++)
    {//此处借用堆的原理
        
        for(g=0;g<20;g++)//先找到第一个已经分配了内存的作业，随便找一个就行
        {
            f=0;
            for(h=0;h<20;h++)
                if(tab_compact[h]==g)
                    f=1;
            if(f==1)
                continue;
            
            if(tab[g].flag==1)
            {
                tab_min=g;
                break;
            }
        }
        
        for(j=0;j<20;j++)
        {
            //tab_min=0;
            f=0;
            for(h=0;h<20;h++)
                if(tab_compact[h]==j)
                    f=1;
            if(f==1)
                continue;
            
            if(tab[j].flag==1)
                if(tab[j].addr<tab[tab_min].addr)
                    tab_min=j; //找出当前地址最小的作业，压入数组
        }
        
        if(tab_min==tab_compact[k-1])
            break;
        tab_compact[k]=tab_min;
        k++;
    }
    
    
    
    tab[tab_compact[0]].addr=0;
    
    for(i=0;i<20-1;i++)
    {
        
        if(tab_compact[i+1]==-1)
            break;
        
        if(tab_compact[i+1]!=-1)
            tab[tab_compact[i+1]].addr=tab[tab_compact[i]].addr+tab[tab_compact[i]].size;
        
    }
    return;
}

int main()//示例代码中main函数没有返回值，非常不规范
{
      head=(LINK *)malloc(sizeof(LINK));//开辟一个结点
      head->next=NULL;//因为此时是空链表
          
      printf("\n Input memory size:");
      
      int n=0;
      scanf("%d",&n);
      printf("\n");
    
      head->size=n;
      memory_size=n;//设置内存空间大小
      head->addr=0;//败笔，头结点一般不存有效数据
    
      int i=0;
      for(;i<20;i++)
      {
//        tab[n].jno=i;
//        tab[n].flag=0;  //实验手册里这一段写错了，而且是低级错误，当定义内存空间为200是不报错，当定义为300时就会报错
          
          tab[i].jno=i;    //定义作业1的编号为1，作业2的编号为2，作业3的编号为3......以此类推
          tab[i].flag=0;   //就算不定义，tab[i].flag也会被Xcode默认为“0”（打个断点输出一下就知道）   flag=0即为作业未曾分配空间
          
      }
      
      int choice=0;
    
      while(1)//示例中用的是do while循环，显然没搞清do while和while的区别和适用情况
      {
          printf("\n\t          选项菜单            \n");
          printf("\n\t-------------------------\n");
          printf("\n\t1--申请内存      2--释放内存    \n");
          printf("\n\t3--输出分区表    4--输出作业表   \n");
          printf("\n\t5--紧凑         0--退出系统    \n\n");
          printf("            请选择:");
          scanf("%d",&choice);
      
          switch(choice)
          {
             case 1:allocm() ;   break;
             case 2:freem();     break;
             case 3:printlink(); break;
             case 4:printtab();  break;
             case 5:compact();   break;
             case 0: exit(0);    break;//示例代码中，没有使用exit命令退出
                  
             default:printf("\n请重新输入：\n");break;
          }          
      }
  }

//本可以加入清屏命令system”cls“，使界面更加清爽，但是Xcode不支持这个库。如果使用visual stdio 2019，这个可以有。
//学生手册里的示例代码bug太多，而且不规范，希望老师能把这份代码作为实验手册上的示例。
