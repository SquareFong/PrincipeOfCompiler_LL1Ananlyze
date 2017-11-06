#ifndef GRAMMARANALYSIS_GRAMMARANALYSIS_H
#define GRAMMARANALYSIS_GRAMMARANALYSIS_H

#include <string>
#include <iostream>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <fstream>
#include <stack>
#include <iomanip>
#include <queue>
#include <deque>
using namespace std;

template<typename T>
void cupEqual(set<T> &lhs, set<T> rhs){
    for(auto it = rhs.begin(); it != rhs.end(); ++it){
        lhs.insert(*it);
    }
}


typedef string str;
class grammarAnalysis {
    //开始字符
    string S;
    //终结符集
    set<char > V_T;
    //非终结符集
    set<string> V_N;
    //产生式集
    map<string, vector<string>> P;
    vector<string> P_Order;

    //文法顺序
    vector<string> V_N_Order;
    map<string, int> V_N_Order_;

    //可空集合
    set<string> nullAble;

    //first集
    map<string, set<char>> firstSet;

    //follow集
    map<string, set<char>> followSet;

    vector<set<char>> first_sSet;

    vector<vector<int>> LL1AnalysisSheet;
public:

    void initialize(){
        S.clear();
        V_T.clear();
        V_N.clear();
        V_N_Order.clear();
        P.clear();
        P_Order.clear();
        nullAble.clear();
        firstSet.clear();
        followSet.clear();
    }

    bool readFile(string FilePath){
        ifstream openFile(FilePath);
        if(openFile){
            char temp[256];
            bool isFirst(true);
            while (openFile.getline(temp,256)){
                string currentLine(temp);
                for(unsigned it = 1; it < currentLine.size(); ++it){
                    if( currentLine[it] == ' ')
                    {
                        string Vn=currentLine.substr(0,it);
                        string p=currentLine.substr(it+1,currentLine.size()-it-1);
                        if(p == "`") {
                            p = "";
                        }
                        if(p == "")
                            nullAble.insert(Vn);
                        else {
                            for(int i(0); i < p.size(); ++i){
                                if(p[i] < 'A' || p[i] > 'Z')
                                    V_T.insert(p[i]);
                            }
                        }
                        P_Order.push_back(p);
                        if(V_N.find(Vn) != V_N.end()) {
                            P[Vn].push_back(p);
                        }
                        else {
                            vector<string> t(1,p);
                            P.insert(make_pair(Vn,t));
                            V_N_Order.push_back(Vn);
                            V_N_Order_.insert(make_pair(Vn,V_N_Order_.size()));
                        }
                        if(isFirst) {
                            isFirst = false;
                            S = Vn;
                        }
                        V_N.insert(Vn);
                        break;
                    }
                }
            }
            return true;
        } else {
            cout << "No such file: " << FilePath << endl;
            return false;
        }
    }

    void show(){
        cout << "S = " << S << endl;

        cout << "Vt : ";
        for(auto it = V_T.begin(); it != V_T.end(); ++it){
            cout << *it << ' ';
        }
        cout << endl;

        cout << "Vn : ";
        for(auto it = V_N_Order.begin(); it != V_N_Order.end(); ++it){
            cout << *it << ' ';
        }
        cout << endl;

        int i(0);
        for(auto it = V_N_Order.begin(); it != V_N_Order.end(); ++it){
            auto p = P[(*it)];
            cout << i++ << ": " << (*it) ;
            for(auto it2 = p.begin(); it2 != p.end(); ++it2) {
                if(it2 == p.begin())
                    cout << "  -> ";
                else
                    cout << i++ << ": " << "    | ";
                if ((*it2) == "") {
                    cout << endl;
                    continue;
                } else {
                    cout << (*it2);
                    cout << endl;
                }
            }
        }

        cout << "Nullable : ";
        for(auto it = nullAble.begin(); it != nullAble.end(); ++it){
            cout << *it << ' ';
        }
        cout << endl;
    }

    void nullAbleCalculate(){
        unsigned long n(0);
        while (n != nullAble.size()){
            n = nullAble.size();
            for(auto it = P.begin(); it != P.end(); ++it){
                string currentVn = (*it).first;
                vector<string> currentProducts = (*it).second;
                for(int j(0); j < currentProducts.size(); ++j) {
                    string currentP = currentProducts[j];

                    stack<string> beta;
                    for (int p = 0; p < currentP.size(); ++p) {
                        int k(0);
                        while (currentP[p + (++k)] == '\''){

                        }
                        string tempV=currentP.substr(p,k);
                        p = p+k-1;
                        beta.push(tempV);
                    }

                    while (!beta.empty()){

                        if(nullAble.find(beta.top()) == nullAble.end()){
                            break;
                        }

                        beta.pop();
                    }

                    if(beta.empty()){
                        nullAble.insert(currentVn);
                    }
                }
            }
        }
    }

    void firstCalculate(){
        //初始化first集
        for(auto it = V_N.begin(); it != V_N.end(); ++it){
            firstSet.insert(make_pair((*it),set<char>()));
        }
        //记录first集的变化
        vector<int> changes(firstSet.size(), -1);
        bool isChanging = true;

        while (isChanging){
            //遍历每个非终结符
            for(auto it = P.begin(); it != P.end(); ++it) {
                string currentVn = (*it).first;
                vector<string> currentProducts = (*it).second;
                //遍历当前非终结符的所有产生式
                for(auto it2 = currentProducts.begin(); it2 != currentProducts.end(); ++it2) {
                    auto currentP=(*it2);
                    if(currentP.length() == 0) {
                        firstSet[currentVn].insert('`');
                        break;
                    }
                    //遍历产生式中的每一个元素
                    for (int i = 0; i < currentP.size(); ++i) {
                        int j(0);
                        while (currentP[i + (++j)] == '\''){

                        }
                        string tempV=currentP.substr(i,j);
                        i = i+j-1;
                        if(V_N.find(tempV) != V_N.end()){
                            auto tempVn = firstSet[tempV];
                            //两个非终结符的first集的并运算
                            cupEqual(firstSet[currentVn],tempVn);
//                            for(auto it3 = tempVn.begin(); it3 != tempVn.end(); ++ it3){
//                                firstSet[currentVn].insert((*it3));
//                            }
                            //如果不属于nullAble
                            if(nullAble.find(tempV) == nullAble.end()){
                                break;
                            }
                        }

                        if((tempV.length() == 1) && (V_T.find(tempV[0]) != V_T.end())){
                            firstSet[currentVn].insert(tempV[0]);
                            break;
                        }

                    }
                }

            }
            //判断是否有变化
            isChanging = false;
            int isC(0);
            auto it = firstSet.begin();
            for(; it != firstSet.end(); ++it, ++isC){
                if(changes[isC] != (*it).second.size()) {
                    changes[isC] = (int)(*it).second.size();
                    isChanging = true;
                }
            }
        }

    }

    void firstShow(){
        cout << "First Set:\n";
        for(auto it = firstSet.begin(); it != firstSet.end(); ++it){
            cout << (*it).first << " : " ;
            set<char> &temp = (*it).second;
            for(auto it2 = temp.begin(); it2 != temp.end(); ++it2){
                cout << (*it2) << ' ';
            }
            cout << endl;
        }
    }

    void followCalculate(){
        //初始化follow集
        for(auto it = V_N.begin(); it != V_N.end(); ++it){
            followSet.insert(make_pair((*it),set<char>()));
        }
        //记录follow集的变化
        vector<int> changes(followSet.size(), -1);
        bool isChanging = true;

        while (isChanging){
            //遍历每个非终结符
            for (auto &it : P) {
                string currentVn = it.first;
                vector<string> currentProducts = it.second;
                //遍历当前非终结符的所有产生式
                for (auto &currentProduct : currentProducts) {
                    auto currentP= currentProduct;
                    if(currentP.length() == 0) {
                        followSet[currentVn].insert('`');
                        break;
                    }
                    auto temp = followSet[currentVn];
                    //遍历产生式中的每一个元素,并逆序
                    stack<string> beta;
                    for (int i = 0; i < currentP.size(); ++i) {
                        int j(0);
                        while (currentP[i + (++j)] == '\''){

                        }
                        string tempV=currentP.substr(i,j);
                        i = i+j-1;
                        beta.push(tempV);
                    }
                    while (!beta.empty()) {
                        string &beta_i = beta.top();
                        if(V_N.find(beta_i) != V_N.end()) {
                            cupEqual(followSet[beta_i],temp);
                            if(nullAble.find(beta_i) == nullAble.end())
                                temp = firstSet[beta_i];
                            else
                                cupEqual(temp, firstSet[beta_i]);
                        }
                        if((beta_i.length() == 1) &&(V_T.find(beta_i[0]) != V_T.end())){
                            set<char> t;
                            t.insert(beta_i[0]);
                            temp = t;
                        }
                        beta.pop();
                    }
                    /////////////////////////////////////////////
                    //cupEqual(followSet[currentVn], temp);
                }
            }


            //判断是否有变化
            isChanging = false;
            int isC(0);
            auto it = followSet.begin();
            for(; it != followSet.end(); ++it, ++isC){
                if(changes[isC] != (*it).second.size()) {
                    changes[isC] = (int)(*it).second.size();
                    isChanging = true;
                }
            }
        }
    }

    void followShow(){
        cout << "Follow Sets: \n";
        for(auto it = followSet.begin(); it != followSet.end(); ++it){
            cout << (*it).first << " : ";
            auto temp = (*it).second;
            for(auto it1 = temp.begin(); it1 != temp.end(); ++it1) {
                cout << (*it1) << ' ';
            }
            cout << endl;
        }
    }

    void first_sCalculate(){
        //初始化first_s集
//        for(auto it = V_N.begin(); it != V_N.end(); ++it){
//            first_sSet.insert(make_pair((*it),set<char>()));
//        }

        //遍历每个非终结符
        for(auto it = V_N_Order.begin(); it != V_N_Order.end(); ++it) {
            string currentVn = (*it);
            vector<string> currentProducts = P[(*it)];
            //遍历当前非终结符的所有产生式
            for (auto it2 = currentProducts.begin(); it2 != currentProducts.end(); ++it2) {
                set<char> tempFirst_S;
                auto currentP = (*it2);
                if (currentP.length() == 0) {
                    //tempFirst_S.insert('`');
                    cupEqual(tempFirst_S, followSet[currentVn]);
                    //break;
                }
                //遍历产生式中的每一个元素
                for (int i = 0; i < currentP.size(); ++i) {
                    int j(0);
                    while (currentP[i + (++j)] == '\'') {

                    }
                    string beta_i = currentP.substr(i, j);
                    i = i + j - 1;
                    if(V_N.find(beta_i) != V_N.end()){
                        cupEqual(tempFirst_S,firstSet[beta_i]);
                        if(nullAble.find(beta_i) == nullAble.end())
                            break;
                        cupEqual(tempFirst_S, followSet[currentVn]);
                    }
                    if((beta_i.length() == 1) &&(V_T.find(beta_i[0]) != V_T.end())){
                        set<char> t;
                        t.insert(beta_i[0]);
                        cupEqual(tempFirst_S,t);
                        break;
                    }

                }
                first_sSet.push_back(tempFirst_S);
            }
        }
    }

    void first_sShow(){

        cout << "First_s Set:\n";
        for(int i(0); i < first_sSet.size(); ++i){
            cout << setw(20) << i;
        }
        cout << endl;
        for(int i(0); i < first_sSet.size(); ++i){
            set<char> &temp = first_sSet[i];
            string s = "{";
            for(auto it = temp.begin(); it != temp.end(); ++it){
                if(it != temp.begin())
                    s += ",";
                s += (*it);
            }
            s += "}";
            cout << setw(20) << s;
        }
    }

    string intToStr(int num){
        if(num == 0)
            return "0";
        string number("");
        while (num){
            char t = '0' + num%10;
            number += t;
            num /= 10;
        }

        return number;
    }

    void analysisSheetFormat(){
        bool ok(true);
        string sheet[V_N.size()][128];
        int n(0);
        for(int i(0); i < V_N.size(); ++i) {
            string currentVn = V_N_Order[i];
            for(int j(0); j < P[currentVn].size(); ++j){
                int current = n+j;
                set<char> nextSet = first_sSet[current];
                for(auto it = nextSet.begin(); it != nextSet.end(); ++it) {
                    string ind = intToStr(current);
                    if(sheet[i][*it] == "")
                    {
                        sheet[i][*it] = intToStr(current);
                    } else {
                        sheet[i][*it] = sheet[i][*it] + "," + intToStr(current);
                        ok = false;
                    }
                }
            }
            n = n + P[V_N_Order[i]].size();

        }

//        cout << setw(5) << ' ';
//        for(auto it = V_T.begin(); it != V_T.end(); ++it){
//            cout << setw(10) << *it;
//        }
//        cout << endl;
//        auto it1 = V_N_Order.begin();
//        for(int i(0); i < V_N_Order.size(); ++it1, ++i){
//            cout << setw(5) << *it1;
//            for(auto it = V_T.begin(); it != V_T.end(); ++it){
//                cout << setw(10) << sheet[i][*it];
//            }
//            cout << endl;
//        }


        if(ok){
            //auto it = V_N_Order.begin();
            for(int i(0); i < V_N_Order.size(); ++i){
                vector<int> tempV(128,-1);
                for(auto it2 = V_T.begin(); it2 != V_T.end(); ++it2){
                    string tn = sheet[i][*it2];
                    if(tn.length()) {
                        int nextNum(atoi(tn.c_str()));
                        int sub = *it2;
                        tempV[sub] = nextNum;
                    }
                }
                LL1AnalysisSheet.push_back(tempV);
            }

            cout << "Analyze sheet \n";
            cout << setw(5) << ' ';
            for(auto it = V_T.begin(); it != V_T.end(); ++it){
                cout << setw(10) << *it;
            }
            cout << endl;
            auto it1 = V_N_Order.begin();
            for(int i(0); i < V_N_Order.size(); ++it1, ++i){
                cout << setw(5) << *it1;
                for(auto it = V_T.begin(); it != V_T.end(); ++it){
                    cout << setw(10) << sheet[i][*it];
                }
                cout << endl;
            }

        }

    }

    bool LL1Analyze(vector<string> tokens){
        int i = 0;
        deque<string> analyzeStack;
        analyzeStack.push_front(S);
        while (!analyzeStack.empty() && i < tokens.size()) {
            string &t = analyzeStack.front();
            if((t.length() == 1) && (V_T.find(t[0]) != V_T.end())){
                if( t[0] == tokens[i++][0])
                    analyzeStack.pop_front();
                else
                {
                    //cout << "erro" << endl;
                    return false;
                }

            } else if(V_N.find(t) != V_N.end()){
                analyzeStack.pop_front();
                int row = V_N_Order_[t];
                int column=tokens[i][0];

                int np = LL1AnalysisSheet[row][column];
                if(np < 0)
                    return false;
                string next = P_Order[np];
//                auto nextP = P[next];
//                for(int l = 0; l < nextP.size(); ++l){
//                    string tempP = nextP[l];
                stack<string> tempStack;
                    for(int i(0); i<next.size(); ++i) {
                        int j(0);
                        while (next[i + (++j)] == '\'') {

                        }
                        string tempV = next.substr(i, j);
                        i = i + j - 1;
                        tempStack.push(tempV);
                    }
//                }
                while (!tempStack.empty()) {
                    string &string1(tempStack.top());
                    analyzeStack.push_front(string1);

                    tempStack.pop();
                }
            }
        }
        return true;
    }
};


#endif //GRAMMARANALYSIS_GRAMMARANALYSIS_H
