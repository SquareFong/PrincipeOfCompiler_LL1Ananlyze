#include <iostream>
#include "grammarAnalysis.h"
int main() {
    grammarAnalysis g;
    g.readFile("C:\\Users\\Square\\Desktop\\g3.txt");
    g.nullAbleCalculate();
    cout << "Basic informations: \n";
    g.show();
    g.firstCalculate();
    g.firstShow();
    g.followCalculate();
    g.followShow();

    g.first_sCalculate();
    //g.show();
    g.first_sShow();
    cout << endl << endl;

    g.analysisSheetFormat();

    ////////////////test here//////////////////
    cout << endl << endl;
    vector<string> test1;
    test1.push_back("a");
    test1.push_back("c");
    test1.push_back("d");
    test1.push_back("d");
    cout << "acdd: ";
    if(g.LL1Analyze(test1))
        cout << "Accept!\n";
    else
        cout << "error!" <<endl;
    test1[2] = "a";
    cout << "acad: ";
    if(g.LL1Analyze(test1))
        cout << "Accept!\n";
    else
        cout << "error!" <<endl;


//    vector<string> test;
//    test.push_back("i");
//    test.push_back("+");
//    test.push_back("i");
//    test.push_back("*");
//    test.push_back("i");
//    cout << "analyze: i+i*i : ";
//    if(g.LL1Analyze(test))
//        cout << "Accept!\n";
//    else
//        cout << "erro!" <<endl;
//
//    test.insert(test.begin(),")");
//    cout << "analyze: )i+i*i : ";
//    if(g.LL1Analyze(test))
//        cout << "Accept!\n";
//    else
//        cout << "erro!" <<endl;


    return 0;
}