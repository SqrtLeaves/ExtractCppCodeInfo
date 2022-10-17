#ifndef PASUMARTHI6526_H
#define PASUMARTHI6526_H

#include <stdlib.h>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <string>
#include <cstring>

using namespace std;

typedef list<pair<int,int> > lpair;//List Pair
typedef pair<int,int> npair; //Node Pair

//struct object for Edge
struct Edge{
public:
    int wt;
    int first;
    int last;
    bool isVisited;
};

//struct object for Double Linked List
struct Node{
public:
    int nsinks;
    Node* left;//Left Pointer
    Node* right;//Right Pointer
    int num;//Node number to keep track of the nodes
    int wt_left;
    int wt_right;
    lpair cap_n_RAT;
    map<pair<int,int>,lpair > mapList;
};

class VanGinneken {

public:
    Node* root;
    int nsinks;
    VanGinneken(){
        root = new Node;
        //Initializing root
        root->num=0;
        root->left=NULL;
        root->right=NULL;
    }; //constructor;
    bool check_visited(vector<Edge> &);
    void BuildTree(Node*,vector<Edge>&,vector<Node>&);
    void AddWire(lpair &,Node*,Node*,int);
    void InsertBuffer(lpair &,Node*);
    void PruneSolList(lpair &);
    void AddDriver(lpair &,Node*);

    lpair VG_Algo(Node*);
    lpair MergeSolList(lpair &,lpair &);
    lpair MergeSol(lpair &,lpair &,Node*);
    ~VanGinneken(){};
};


#endif