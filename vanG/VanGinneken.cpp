#include "VanGinneken.h"

/////////////////////////////////////////////////////
///// Van-Ginneken Buffer Insertion Algorithm   /////
///// Author: S Venkata Satish Kumar Pasumarthi /////
/////////////////////////////////////////////////////



void VanGinneken::BuildTree(Node* node,vector<Edge>& e,vector<Node>& sinks){
    if(node->num>0&&node->num<nsinks+1){
        node->left=NULL;
        node->right=NULL;
        node->cap_n_RAT=sinks[node->num-1].cap_n_RAT;
    }
    if(!check_visited(e)){
        for(int i=0;i<e.size();i++){
            if(node->num==e[i].first){
                e[i].isVisited=true;
                if(node->left!=NULL&&node->right==NULL){
                    node->right=new Node;
                    node->right->left=NULL;
                    node->right->right=NULL;
                    node->wt_right=e[i].wt;
                    node->right->num =e[i].last;
                    BuildTree(node->right,e,sinks);
                }else if(node->left==NULL&&(node->right!=NULL||node->right==NULL)){
                    node->left=new Node;
                    node->left->right=NULL;
                    node->left->left=NULL;
                    node->wt_left=e[i].wt;
                    node->left->num=e[i].last;
                    BuildTree(node->left,e,sinks);
                }
            }
        }
    }
    return;
}

void VanGinneken::AddDriver(lpair& List,Node* node){
    if(not List.empty()){
        for(lpair::iterator iter=List.begin();iter!=List.end();iter++){
            lpair tmp=node->mapList[*iter];
            iter->second=(iter->second)-(iter->first);
            node->mapList[*iter]=tmp;
        }
    }
    return;
}

void VanGinneken::AddWire(lpair& List,Node* parent,Node* child,int wt){
    if(not List.empty()){
        for(lpair::iterator iter=List.begin();iter!=List.end();iter++){
            lpair addtmp=child->mapList[*iter];
            addtmp.push_back(make_pair(parent->num,0));
            //update the delay value
            // q2 = q1 -(rx)*(cx)/2-rxc1 [r=c=1 given]
            iter->second=(iter->second)-(wt*wt)/2-wt*(iter->first);
            //update the cap value
            //c2 = c1+(cx)
            iter->first=iter->first+wt;
            parent->mapList[*iter]=addtmp;
        }
    }
    return;
}

void VanGinneken::InsertBuffer(lpair &List,Node* parent){
    if(!List.empty()){
        lpair tmpList,addtmp;
        tmpList =List;
        for(lpair::iterator iter=tmpList.begin();iter!=tmpList.end();iter++){
            addtmp=parent->mapList[*iter];
            addtmp.push_back(make_pair(parent->num,1));
            iter->second=(iter->second)-(iter->first)-1;
            iter->first=1;
            List.push_back(*iter);
            parent->mapList[*iter]=addtmp;
        }
    }
    return;
}

//Sub-routine to check if the entire edge list is visited
bool VanGinneken::check_visited(vector<Edge>& e){
    for(int i=0;i<e.size();i++){
        if(not e[i].isVisited) return false;
    }
    return true;
}

bool my_compare(pair<int,int>& a,pair<int,int>& b){
    return a.first < b.first;
}
//Sub-routine to PruneSolList the solutionLis
void VanGinneken::PruneSolList(lpair &solList){
    //customSort
    solList.sort(my_compare);//Sort according to cap values (first field)

    lpair::iterator iter1=solList.begin();
    lpair::iterator iter2=solList.begin();
    iter2++;//point to second element
    //iter1 => first element; iter2 => second element
    // we don't need to check for c1 > c2 case as we already sorted the solList
    for(;iter2!=solList.end();){
        if(iter1->first < iter2->first){
            if((iter1->second) >= (iter2->second)){//never happens. But for sanity
                iter2=solList.erase(iter2);
            }else{//continue scanning
                iter1++;
                iter2++;
            }
        }else if(iter1->first == iter2->first){//Check for q if c is equal
            //delete element with smaller q
            if(iter1->second >= iter2->second){
                iter2=solList.erase(iter2);
            }else{
                iter1=solList.erase(iter1);
                iter2++;
            }
        }
    }
}

lpair VanGinneken::MergeSolList(lpair &lc,lpair &rc){
    //Push vector rc to vector lc
    for(lpair::iterator iter=rc.begin();iter!=rc.end();iter++){
        lc.push_back(*iter);
    }
    return lc;
}

lpair VanGinneken::MergeSol(lpair& lc,lpair& rc,Node* parent){
    int cap;
    lpair tmp,LeftChild,RightChild,tmpM;
    lpair::iterator left_iter=lc.begin();//iterator for left child
    lpair::iterator right_iter=rc.begin();//iterator for right child
    for(;left_iter!=lc.end()&&right_iter!=rc.end();){
        LeftChild  = parent->mapList[*left_iter];
        RightChild = parent->mapList[*right_iter];
        tmpM=MergeSolList(LeftChild,RightChild);
        //CMergeSol = Cl+Cr
        //qMergeSol = min(ql,qr)
        cap=(left_iter->first)+(right_iter->first);
        //ql < qr
        if(left_iter->second < right_iter->second){
            tmp.push_back(make_pair(cap,left_iter->second));//pick min
            parent->mapList[make_pair(cap,left_iter->second)]=tmpM;
            left_iter++;
        }else{ //ql > qr
            tmp.push_back(make_pair(cap,right_iter->second));//pick min
            parent->mapList[make_pair(cap,right_iter->second)]=tmpM;
            right_iter++;
        }
    }
    return tmp;
}

//Recursive call to do the WireAddition,BufferInsertion and Pruning
lpair VanGinneken::VG_Algo(Node* node){
    lpair Left,Right,Middle,tmp;
    lpair initial;
    //Making sure we are not screwing up root
    if((node->num > 0) && (node->num < nsinks+1)){
        tmp=node->cap_n_RAT;
        for(lpair::iterator iter=tmp.begin();iter!=tmp.end();++iter){
            initial.push_back(make_pair(node->num,0));
            node->mapList[*iter]=initial;
        }
        return tmp;
    }else{
        //Deal with Left child
        if(node->left!=NULL){//Left Child
            //Follow the steps for left child
            Left=VG_Algo(node->left);
            AddWire(Left,node,node->left,node->wt_left);
            InsertBuffer(Left,node);
            PruneSolList(Left);
        }
        //Deal with Right child now
        if(node->right!=NULL){
            Right=VG_Algo(node->right);
            AddWire(Right,node,node->right,node->wt_right);
            InsertBuffer(Right,node);
            PruneSolList(Right);
        }
        //Merging the left and right child
        if(node->left!=NULL && node->right!=NULL){//MergeSol Point
            Middle=MergeSol(Left,Right,node);
            PruneSolList(Middle);
            return Middle;
        }
        //return the non-NULL child
        if(node->left!=NULL && node->right==NULL){
            return Left;
        }
        if(node->left==NULL && node->right!=NULL){
            return Right;
        }
    }
    exit (-1);
}

int main(int argc, char *argv[]) {

    //args for file parsing
    string line;
    int count = 0; //Initialize to count the lines and detect inputs accordingly
    char* token[3] = {};//array to store values in infile
    char oneline[100];//need this because strtok cannot handle strings

    int val1,val2,val3;
    int counter;
    int nbuff=0;
    vector<int> tmp;

    //Initializing VanGinneken class object
    VanGinneken V;


    //Input arguments check
    if (argc < 2) {
        cout << "Atleast one argument is expected\n";
        cout << "Exiting...\n";
        cout << "Usage: " << argv[0] << " <input_file>\n";
        exit (1);
    }

    //Input file check
    ifstream infile;
    infile.open(argv[1]);//open the input file
    if(infile.fail()){
        cout << "Failed to open the input file\n";
        cout << "Please the file permissions\n";
        cout << "Exiting...\n";
        exit (1);
    }
    if(!infile.good()) {
        cout << "Input file doesn't exist. Please check..\n";
        cout << "Exiting...\n";
        exit (1); //exit if file not found
    }

    //Initiliaze the root

    vector<Node> sinkList;
    vector<Edge> edgeList;

    while(getline(infile,line)){
        strcpy(oneline,line.c_str());
        token[0] = strtok(oneline," ");
        if(token[0]){//zero if line is blank;
            V.nsinks = atoi(token[0]);
            break;
        }
    }

    //Setting counter for reading sink info
    counter = V.nsinks ;

    while(counter > 0){
        getline(infile,line);
        strcpy(oneline,line.c_str());
        token[0] = strtok(oneline," ");
        if(token[0]){//zero if line is blank;
            for(int n=1;n<=2;n++){
                token[n] = strtok(NULL," ");
                if(!token[n]) break;
            }
        }
        val1 = atoi(token[0]);
        val2 = atoi(token[1]);
        Node n1;
        n1.num = V.nsinks-counter+1;//Setting number to the nodes
        n1.cap_n_RAT.push_back(make_pair(val1,val2));
        sinkList.push_back(n1);
        counter--;
    }

    //Setting counter for reading edge info
    counter = 2*(V.nsinks)-1;

    while(counter > 0){
        getline(infile,line);
        strcpy(oneline,line.c_str());
        token[0] = strtok(oneline," ");
        if(token[0]){//zero if line is blank;
            for(int n=1;n<=3;n++){
                token[n] = strtok(NULL," ");
                if(!token[n]) break;
            }
        }
        val1 = atoi(token[0]);
        val2 = atoi(token[1]);
        val3 = atoi(token[2]);
        //Push the edge into EdgeList Vector
        Edge e1;
        e1.first=val1;
        e1.last=val2;
        e1.wt=val3;
        e1.isVisited=false;
        edgeList.push_back(e1);
        counter--;
    }

    //Build the tree as we have all the data now
    V.BuildTree(V.root,edgeList,sinkList);

    //Run the algorithm
    V.root->cap_n_RAT=V.VG_Algo(V.root);

    //Add Driver to the path
    V.AddDriver(V.root->cap_n_RAT,V.root);

    //Pruning the solution space
    V.PruneSolList(V.root->cap_n_RAT);

    //Fetch the last element - solution
    npair sol=V.root->cap_n_RAT.back();

    //Candidate Buffer Solutions
    lpair CBS=V.root->mapList[sol];

    for(lpair::iterator iter=CBS.begin();iter!=CBS.end();iter++){
        if(iter->second==1){
            nbuff++;//increment buffer counter
            tmp.push_back(iter->first);//get the sinks
        }
    }

    //Print results
    cout << "~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*" << endl;
    cout << "Van Ginneken Algorithm for buffer insertion \n";
    cout << "~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*" << endl;
    cout << "Number of buffers	:: " << nbuff << endl;
    cout << "Buffer Insertion Nodes 	:: ";
    for(int i=tmp.size()-1;i>0;i--){
        cout << tmp[i] << ",";
    }
    cout << tmp[0] << endl;
    cout << "Req. Arrival Time (RAT) :: " << sol.second << endl;
    cout << "~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*" << endl;
    return 0;
}