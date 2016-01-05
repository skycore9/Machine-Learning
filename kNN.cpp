//
//  main.cpp
//  kNN
//
//  Created by feng on 15/10/24.
//  Copyright © 2015年 ttcn. All rights reserved.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

template<typename T>
struct KdTree {
    // ctor
    KdTree():parent(nullptr), leftChild(nullptr), rightChild(nullptr) {}
    
    // KdTree是否为空
    bool isEmpty() { return root.empty(); }
    
    // KdTree是否为叶子节点
    bool isLeaf() { return !root.empty() && !leftChild && !rightChild;}
    
    // KdTree是否为根节点
    bool isRoot() { return !isEmpty() && !parent;}
    
    // 判断KdTree是否为根节点的左儿子
    bool isLeft() { return parent->leftChild->root == root; }
    
    // 判断KdTree是否为根节点的右儿子
    bool isRight() { return parent->rightChild->root == root; }
    
    // 存放根节点的数据
    vector<T> root;
    
    // 父节点
    KdTree<T> *parent;
    
    // 左儿子
    KdTree<T> *leftChild;
    
    // 右儿子
    KdTree<T> *rightChild;
};


/**
 *  矩阵转置
 *
 *  @param matrix 原矩阵
 *
 *  @return 原矩阵的转置矩阵
 */
template<typename T>
vector<vector<T>> transpose(const vector<vector<T>> &matrix) {
    size_t rows = matrix.size();
    size_t cols = matrix[0].size();
    vector<vector<T>> trans(cols, vector<T>(rows, 0));
    for (size_t i = 0; i < cols; ++i) {
        for (size_t j = 0; j < rows; ++j) {
            trans[i][j] = matrix[j][i];
        }
    }
    
    return trans;
}

/**
 *  找中位数
 *
 *  @param vec 数组
 *
 *  @return 数组中的中位数
 */
template<typename T>
T findMiddleValue(vector<T> vec) {
    sort(vec.begin(), vec.end());
    size_t pos = vec.size() / 2;
    return vec[pos];
}

/**
 *  递归构造KdTree
 *
 *  @param tree  KdTree根节点
 *  @param data  数据矩阵
 *  @param depth 当前节点深度
 *
 *  @return void
 */
template<typename T>
void buildKdTree(KdTree<T> *tree, vector<vector<T>> &data, size_t depth) {
    // 输入数据个数
    size_t samplesNum = data.size();
    
    if (samplesNum == 0) {
        return;
    }
    
    if (samplesNum == 1) {
        tree->root = data[0];
        return;
    }
    
    // 每一个输入数据的维度，属性个数
    size_t k = data[0].size();
    vector<vector<T>> transData = transpose(data);
    
    // 找到当前切分点
    size_t splitAttributeIndex = depth % k;
    vector<T> splitAttributes = transData[splitAttributeIndex];
    T splitValue = findMiddleValue(splitAttributes);
    
    vector<vector<T>> leftSubSet;
    vector<vector<T>> rightSubset;
    
    for (size_t i = 0; i < samplesNum; ++i) {
        if (splitAttributes[i] == splitValue && tree->isEmpty()) {
            tree->root = data[i];
        } else if (splitAttributes[i] < splitValue) {
            leftSubSet.push_back(data[i]);
        } else {
            rightSubset.push_back(data[i]);
        }
    }
    
    tree->leftChild = new KdTree<T>;
    tree->leftChild->parent = tree;
    tree->rightChild = new KdTree<T>;
    tree->rightChild->parent = tree;
    buildKdTree(tree->leftChild, leftSubSet, depth + 1);
    buildKdTree(tree->rightChild, rightSubset, depth + 1);
}

/**
 *  递归打印KdTree
 *
 *  @param tree  KdTree
 *  @param depth 当前深度
 *
 *  @return void
 */
template<typename T>
void printKdTree(const KdTree<T> *tree, size_t depth) {
    for (size_t i = 0; i < depth; ++i) {
        cout << "\t";
    }
    
    for (size_t i = 0; i < tree->root.size(); ++i) {
        cout << tree->root[i] << " ";
    }
    cout << endl;
    
    if (tree->leftChild == nullptr && tree->rightChild == nullptr) {
        return;
    } else {
        if (tree->leftChild) {
            for (int i = 0; i < depth + 1; ++i) {
                cout << "\t";
            }
            cout << "left : ";
            printKdTree(tree->leftChild, depth + 1);
        }
        
        cout << endl;
        
        if (tree->rightChild) {
            for (size_t i = 0; i < depth + 1; ++i) {
                cout << "\t";
            }
            cout << "right : ";
            printKdTree(tree->rightChild, depth + 1);
        }
        cout << endl;
    }
}

/**
 *  节点之间的欧氏距离
 *
 *  @param p1 节点1
 *  @param p2 节点2
 *
 *  @return 节点之间的欧式距离
 */
template<typename T>
T calDistance(const vector<T> &p1, const vector<T> &p2) {
    T res = 0;
    for (size_t i = 0; i < p1.size(); ++i) {
        res += pow(p1[i] - p2[i], 2);
    }
    
    return res;
}

/**
 *  搜索目标节点的最近邻
 *
 *  @param tree KdTree
 *  @param goal 待分类的节点
 *
 *  @return 最近邻节点
 */
template <typename T>
vector<T> searchNearestNeighbor(KdTree<T> *tree, const vector<T> &goal ) {
    // 节点数属性个数
    size_t k = tree->root.size();
    // 划分的索引
    size_t d = 0;
    KdTree<T> *currentTree = tree;
    vector<T> currentNearest = currentTree->root;
    // 找到目标节点的最叶节点
    while (!currentTree->isLeaf()) {
        size_t index = d % k;
        if (currentTree->rightChild->isEmpty() || goal[index] < currentNearest[index]) {
            currentTree = currentTree->leftChild;
        } else {
            currentTree = currentTree->rightChild;
        }
        
        ++d;
    }
    currentNearest = currentTree->root;
    T currentDistance = calDistance(goal, currentTree->root);
    
    KdTree<T> *searchDistrict;
    if (currentTree->isLeft()) {
        if (!(currentTree->parent->rightChild)) {
            searchDistrict = currentTree;
        } else {
            searchDistrict = currentTree->parent->rightChild;
        }
    } else {
        searchDistrict = currentTree->parent->leftChild;
    }
    
    while (!(searchDistrict->parent)) {
        T districtDistance = abs(goal[(d + 1) % k] - searchDistrict->parent->root[(d + 1) % k]);
        
        if (districtDistance < currentDistance) {
            T parentDistance = calDistance(goal, searchDistrict->parent->root);
            
            if (parentDistance < currentDistance) {
                currentDistance = parentDistance;
                currentTree = searchDistrict->parent;
                currentNearest = currentTree->root;
            }
            
            if (!searchDistrict->isEmpty()) {
                T rootDistance = calDistance(goal, searchDistrict->root);
                if (rootDistance < currentDistance) {
                    currentDistance = rootDistance;
                    currentTree = searchDistrict;
                    currentNearest = currentTree->root;
                }
            }
            
            if (!(searchDistrict->leftChild)) {
                T leftDistance = calDistance(goal, searchDistrict->leftChild->root);
                if (leftDistance < currentDistance) {
                    currentDistance = leftDistance;
                    currentTree = searchDistrict;
                    currentNearest = currentTree->root;
                }
            }
            
            if (!(searchDistrict->rightChild)) {
                T rightDistance = calDistance(goal, searchDistrict->rightChild->root);
                if (rightDistance < currentDistance) {
                    currentDistance = rightDistance;
                    currentTree = searchDistrict;
                    currentNearest = currentTree->root;
                }
            }
            
        }
        
        if (!(searchDistrict->parent->parent)) {
            searchDistrict = searchDistrict->parent->isLeft()? searchDistrict->parent->parent->rightChild : searchDistrict->parent->parent->leftChild;
        } else {
            searchDistrict = searchDistrict->parent;
        }
        ++d;
    }
    
    return currentNearest;
}

int main(int argc, const char * argv[]) {
    vector<vector<double>> trainDataSet{{2,3},{5,4},{9,6},{4,7},{8,1},{7,2}};
    KdTree<double> *kdTree = new KdTree<double>;
    buildKdTree(kdTree, trainDataSet, 0);
    printKdTree(kdTree, 0);
    
    vector<double> goal{3, 4.5};
    vector<double> nearestNeighbor = searchNearestNeighbor(kdTree, goal);
    
    for (auto i : nearestNeighbor) {
        cout << i << " ";
    }
    cout << endl;
    
    return 0;
}
