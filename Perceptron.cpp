//
//  main.cpp
//  Perceptron
//
//  Created by feng on 15/10/23.
//  Copyright © 2015年 ttcn. All rights reserved.
//

#include <iostream>
#include <vector>
using namespace std;

/**
 *  求两个向量的内积
 *
 *  @param lhs 向量1
 *  @param rhs 向量2
 *
 *  @return 内积
 */
int product(const vector<int> &lhs, const vector<int> &rhs) {
    int result = 0;
    for (int i = 0; i < lhs.size() - 1; ++i) {
        result += lhs[i] * rhs[i];
    }
    
    return result;
}

/**
 *  感知机, 原始形式
 *
 *  @param dataSet 数据集
 *
 *  @return 返回结果(w, b)
 */
vector<int> perceptron(const vector<vector<int>> &dataSet) {
    if (dataSet.size() == 0) {
        return vector<int>();
    }
    
    auto len = dataSet[0].size();
    vector<int> w_b(len, 0);
    int n = 1;
    while (true) {
        bool finished = true;
        for (auto &ix : dataSet) {
            int y = ix[len - 1];
            if (y *  (product(ix, w_b) + w_b[len - 1]) <= 0) {
                for (auto j = 0; j < len - 1; ++j) {
                    w_b[j] += n * y * ix[j];
                }
                w_b[len - 1] += n * y;
                finished = false;
            }
        }
        
        if (finished) {
            break;
        }
    }
    
    return w_b;
}

/**
 *  感知机，对偶形式
 *
 *  @param dataSet 数据集
 *
 *  @return 结果
 */
vector<int> perceptron_s(const vector<vector<int>> &dataSet) {
    // 计算Gram矩阵
    vector<vector<int>> gram;
    for (auto &i :  dataSet) {
        vector<int> tmp;
        for (auto &j : dataSet) {
            tmp.push_back(product(i, j));
        }
        gram.push_back(tmp);
    }
    
    vector<int> a(dataSet.size(), 0);
    int b = 0;
    int n = 0.8;
    
    auto len = dataSet[0].size();
    while (true) {
        bool finished = true;
        for (int i = 0; i < dataSet.size(); ++i) {
            int sum = 0;
            for (int j = 0; j < dataSet.size(); ++j) {
                int yj = dataSet[j][len - 1];
                sum += a[i] * gram[i][j] * yj;
            }
            
            int yi = dataSet[i][len - 1];
            if (yi *(sum + b) <= 0) {
                a[i] += n;
                b += n * yi;
                finished = false;
                cout << a[0] << " " << a[1] << " " << a[2] << " " << b << endl;
            }
        }
        
        if (finished) {
            break;
        }
    }
    
    vector<int> w_b;
    for (int i = 0; i < dataSet.size(); ++i) {
        int tmp = 0;
        for (int j = 0; j < dataSet.size(); ++j) {
            tmp += dataSet[i][j] * a[j];
        }
        w_b.push_back(tmp);
    }
    
    w_b.push_back(b);
    return w_b;
}


int main(int argc, const char * argv[]) {
    vector<vector<int>> dataSet;
    dataSet.push_back({3, 3, 1});
    dataSet.push_back({4, 3, 1});
    dataSet.push_back({1, 1, -1});
    
    vector<int> result = perceptron_s(dataSet);
    cout << result[0] << " " << result[1] << " " << result[2] << endl;
    
    return 0;
}
