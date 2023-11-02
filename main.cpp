//copyright@ 纯净的天空 && yansha  
 //5、July，updated，2010.05.28。 
//harryshayne，update again。2011.6.30 
#include <iostream>  
#include <ctime>  
#include <fstream>  
#include <stdio.h>
#include <string.h>
//#include "ExternSort.h"  
using namespace std;  
  
 //使用多路归并进行外排序的类  
 //ExternSort.h  
   
/* 
* 大数据量的排序 
* 多路归并排序 
* 以千万级整数从小到大排序为例 
* 一个比较简单的例子，没有建立内存缓冲区 
*/  
  
#ifndef EXTERN_SORT_H  
#define EXTERN_SORT_H  
  
#include <cassert>  
//#define k 5  
#define MIN -1//这里开始的时候出现了一个BUG，如果定义的MIN大于等于待排序的数，则会是算法出现错误
#define MAX 10000000//最大值，附加在归并文件结尾
typedef int* LoserTree;
typedef int* External;
 
class ExternSort  
{  
public:  
    void sort()  
    {  
        time_t start = time(NULL);  
          
        //将文件内容分块在内存中排序，并分别写入临时文件  
        k = memory_sort();  //
          
        //归并临时文件内容到输出文件  
        //merge_sort(file_count); 
        ls=new int[k];
        b=new int[k+1];
        K_Merge();
        delete []ls;
        delete []b;
          
        time_t end = time(NULL);  
        printf("total time:%f\n", (end - start) * 1000.0/ CLOCKS_PER_SEC);  
    }  
      
    //input_file:输入文件名  
    //out_file:输出文件名  
    //count: 每次在内存中排序的整数个数  
    ExternSort(const char *input_file, const char * out_file, int count)  
    {  
        m_count = count;  
        m_in_file = new char[strlen(input_file) + 1];  
        strcpy(m_in_file, input_file);  
        m_out_file = new char[strlen(out_file) + 1];  
        strcpy(m_out_file, out_file);  
    }  
    virtual ~ExternSort()  
    {  
        delete [] m_in_file;  
        delete [] m_out_file;  
    }  
      
private:  
    int m_count; //数组长度  
    char *m_in_file;   //输入文件的路径  
    char *m_out_file; //输出文件的路径  
    int k;//归并数，此数必须要内排序之后才能得到，所以下面的ls和b都只能定义为指针(注意和书上区别)
    LoserTree ls;//定义成为指针，之后动态生成数组
    External b;//定义成为指针，在成员函数中可以把它当成数组使用
    //int External[k];
protected:  
    int read_data(FILE* f, int a[], int n)  
    {  
        int i = 0;  
        while(i < n && (fscanf(f, "%d", &a[i]) != EOF)) i++;  
        printf("read:%d integer\n", i);  
        return i;  
    }  
    void write_data(FILE* f, int a[], int n)  
    {  
        for(int i = 0; i < n; ++i)  
            fprintf(f, "%d ", a[i]);  
        fprintf(f,"%d",MAX);//在最后写上一个最大值
    }  
    char* temp_filename(int index)  
    { 
        char *tempfile = new char[100];  
        sprintf(tempfile, "temp%d.txt", index);  
        return tempfile;  
    }  
    static int cmp_int(const void *a, const void *b)  
    {  
        return *(int*)a - *(int*)b;  
    }  
      
    int memory_sort()  
    {  
        FILE* fin = fopen(m_in_file, "rt");  
        int n = 0, file_count = 0;  
        int *array = new int[m_count];  
          
        //每读入m_count个整数就在内存中做一次排序，并写入临时文件  
        while(( n = read_data(fin, array, m_count)) > 0)  
        {  
            qsort(array, n, sizeof(int), cmp_int);     
            //这里，调用了库函数阿，在第四节的c实现里，不再调用qsort。  
            char *fileName = temp_filename(file_count++);  
            FILE *tempFile = fopen(fileName, "w");  
            free(fileName);  
            write_data(tempFile, array, n);  
            fclose(tempFile);  
        }  
          
        delete [] array;  
        fclose(fin);  
          
        return file_count;  
    }  
 
    void Adjust(int s)
    {//沿从叶子节点b[s]到根节点ls[0]的路径调整败者树
        int t=(s+k)/2;//ls[t]是b[s]的双亲节点
        while(t>0)
        {
            if(b[s]>b[ls[t]])//如果失败，则失败者位置s留下，s指向新的胜利者
            {
                int tmp=s;
                s=ls[t];
                ls[t]=tmp;
            }
            t=t/2;
        }
        ls[0]=s;//ls[0]存放调整后的最大值的位置
    }
 
    void CreateLoserTree()
    {
        b[k]=MIN;//额外的存储一个最小值
        for(int i=0;i<k;i++)ls[i]=k;//先初始化为指向最小值，这样后面的调整才是正确的
                                    //这样能保证非叶子节点都是子树中的“二把手”
        for(int i=k-1;i>=0;i--)
            Adjust(i);//依次从b[k-1],b[k-2]...b[0]出发调整败者树
    }
 
    void K_Merge()
    {//利用败者数把k个输入归并段归并到输出段中
        //b中前k个变量存放k个输入段中当前记录的元素
        //归并临时文件  
        FILE *fout = fopen(m_out_file, "wt");  
        FILE* *farray = new FILE*[k];  
        int i;  
        for(i = 0; i < k; ++i)  //打开所有k路输入文件
        {  
            char* fileName = temp_filename(i);  
            farray[i] = fopen(fileName, "rt");  
            free(fileName);  
        }  
          
        for(i = 0; i < k; ++i)  //初始读取
        {  
            if(fscanf(farray[i], "%d", &b[i]) == EOF)//读每个文件的第一个数到data数组  
            {
                printf("there is no %d file to merge!",k);
                return;
            }
        }  
    //    for(int i=0;i<k;i++)input(b[i]);
 
        CreateLoserTree();
        int q;
        while(b[ls[0]]!=MAX)//
        {
            q=ls[0];//q用来存储b中最小值的位置，同时也对应一路文件
            //output(q);
            fprintf(fout,"%d\n",b[q]);
            //input(b[q],q);
            fscanf(farray[q],"%d",&b[q]);
            Adjust(q);
        }
        //output(ls[0]);
        fprintf(fout,"%d\n",b[ls[0]]);
        //delete [] hasNext;  
        //delete [] data;  
          
        for(i = 0; i < k; ++i)  //清理工作
        {  
            fclose(farray[i]);  
        }  
        delete [] farray;  
        fclose(fout);  
    }

};  
  
#endif  
  
  
//测试主函数文件  
/* 
* 大文件排序 
* 数据不能一次性全部装入内存 
* 排序文件里有多个整数，整数之间用空格隔开 
*/  
  
const unsigned int count = 10000000; // 文件里数据的行数  
const unsigned int number_to_sort = 100000; //在内存中一次排序的数量  
const char *unsort_file = "/media/zhang/data/BaiduNetdiskDownload/sort/data.txt"; //原始未排序的文件名  
const char *sort_file = "/media/zhang/data/BaiduNetdiskDownload/sort/good.txt"; //已排序的文件名  
void init_data(unsigned int num); //随机生成数据文件  
  
int main(int argc, char* *argv)  
{  
    srand(time(NULL));  
    init_data(count);  
    ExternSort extSort(unsort_file, sort_file, number_to_sort);  
    extSort.sort();  

    return 0;  
}  
  
void init_data(unsigned int num)  
{  
    FILE* f = fopen(unsort_file, "wt");  
    for(int i = 0; i < num; ++i)  
        fprintf(f, "%d ", rand());  
    fclose(f);  
}  