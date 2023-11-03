#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <iomanip>
#include <stack>
#include <cmath>
#include <chrono>

#include "fmt.h"

const int BLOCK_SIZE = 100000; // 每个块的大小

int partition(double a[], int start, int end)
{
    double pivot = a[end];
    int pIndex = start;
 
    for (int i = start; i < end; i++)
    {
        if (a[i] <= pivot)
        {
            std::swap(a[i], a[pIndex]);
            pIndex++;
        }
    }
 
    std::swap (a[pIndex], a[end]);
 
    return pIndex;
}
 
// Iterative Quicksort routine
void iterativeQuicksort(double a[], int n)
{
    std::stack<std::pair<int, int>> s;
 
    int start = 0;
    int end = n - 1;
 
    s.push(std::make_pair(start, end));
 
    while (!s.empty())
    {
        start = s.top().first, end = s.top().second;
        s.pop();
 
        int pivot = partition(a, start, end);
 
        if (pivot - 1 > start) {
            s.push(std::make_pair(start, pivot - 1));
        }
 
        if (pivot + 1 < end) {
            s.push(std::make_pair(pivot + 1, end));
        }
    }
}

void internalSort(std::vector<double>& block) {
    // std::sort(block, block.begin(), block.end());
    iterativeQuicksort(block.data(), block.size());
}

// 多路归并外排函数
void multiwayMergeExternalSort(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream input(inputFile);
    std::ofstream output(outputFile);
    if (input.fail() || output.fail()) {
        std::cout << "fail to open files" << std::endl;
        return;
    }

    std::vector<double> block; // 存储块的数据
    std::priority_queue<std::pair<double, double>, std::vector<std::pair<double, double>>, std::greater<std::pair<double, double>>> minHeap;

    double num;
    int blockIndex = 0;

    // 划分阶段：读取输入文件并划分为多个块
    // 获取当前时间点
    auto start = std::chrono::high_resolution_clock::now();
    while (input >> std::scientific >>std::setprecision(16) >> num) {
        block.push_back(num);

        if (block.size() == BLOCK_SIZE) {
            internalSort(block);

            // 将块写入磁盘文件
            std::string blockFileName = "block_" + std::to_string(blockIndex) + ".txt";
            std::ofstream blockFile(blockFileName);
            for (double value : block) {
                blockFile << std::scientific << std::setprecision(16) << value << " ";
            }
            blockFile.close();

            minHeap.push({ block[0], blockIndex });
            block.clear();
            blockIndex++;
        }
    }

    // 处理最后一个不完整的块
    if (!block.empty()) {
        internalSort(block);

        std::string blockFileName = "block_" + std::to_string(blockIndex) + ".txt";
        std::ofstream blockFile(blockFileName);
        for (double value : block) {
            blockFile << value << " ";
        }
        blockFile.close();

        minHeap.push({ block[0], blockIndex });
        block.clear();
        blockIndex++;
    }

    input.close();
    // 获取当前时间点
    auto end = std::chrono::high_resolution_clock::now();

    // 计算时间差
    std::chrono::duration<double> duration = end - start;

    // 打印运行时间（以秒为单位）
    std::cout << "读入时间: " << duration.count() << " 秒" << std::endl;


    // 多路归并阶段：合并排序后的块
    std::vector<std::ifstream> blockFiles(blockIndex);
    std::vector<double> blockPositions(blockIndex);
    for (int i = 0; i < blockIndex; i++) {
        std::string blockFileName = "block_" + std::to_string(i) + ".txt";
        blockFiles[i].open(blockFileName);
        blockFiles[i] >> num;
        blockPositions[i] = num;
    }
    while (!minHeap.empty()) {
        auto [value, index] = minHeap.top();
        minHeap.pop();

        int exponent;
        double mantissa = std::frexp(value, &exponent);
        // output << std::showpos << std::scientific << std::setprecision(9) << value << "\n";
        output << double2str(value, 3) << '\n';
        if (blockFiles[index] >> num) {
            blockPositions[index] = num;
            minHeap.push({ num, index });
        }
        else {
            blockFiles[index].close();
            std::string blockFileName = "block_" + std::to_string(index) + ".txt";
            std::remove(blockFileName.c_str());
        }
    }
    output.close();
    std::cout << "排序完成。" << std::endl;
}

int main() {
    // 获取当前时间点
    auto start = std::chrono::high_resolution_clock::now();

    std::string inputFile = "/media/zhang/data/BaiduNetdiskDownload/sort/data.txt"; // 输入文件名
    std::string outputFile = "/media/zhang/data/BaiduNetdiskDownload/sort/good_result.txt"; // 输出文件名

    multiwayMergeExternalSort(inputFile, outputFile);

    // 获取当前时间点
    auto end = std::chrono::high_resolution_clock::now();

    // 计算时间差
    std::chrono::duration<double> duration = end - start;

    // 打印运行时间（以秒为单位）
    std::cout << "总运行时间: " << duration.count() << " 秒" << std::endl;

    return 0;
}