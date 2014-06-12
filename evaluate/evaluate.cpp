#include<iostream>
#include<fstream>
#include<string>
#include<queue>
#include<set>
#include"../datainfo.h"

#include<boost/program_options.hpp>
namespace po = boost::program_options;


using namespace std;

// Declare the supported options.
po::options_description desc("Allowed options");
po::variables_map vm;

string pfname,              // prediction file name
       tfname,              // test file name
       ofname;              // output file name 

int main(int argc, char* argv[]){
    
    desc.add_options()
        ("pred-file,p", po::value<string>(&pfname)->required(), "prepared prediction file (required), sorted and contains only user and item column")
        ("test-file,t", po::value<string>(&tfname)->required(), "prepared test file (required), contains only user and item column")
        ("output-file,o", po::value<string>(&ofname)->required(), "output file (required)")
        ("verbose,v", "Enable verbose output")
        ("help,h", "Display this message")
    ;

    try{ po::store(po::parse_command_line(argc, argv, desc), vm); po::notify(vm);    }
    catch(exception e){ cout << "Usage: evaluate [options]"<<endl; cout << desc <<endl; return 1; }

    if (vm.count("help")) { cout << "Usage: evaluate [options]"<<endl; cout << desc <<endl; return 1; }


    // read test
    ifstream ftest(tfname.c_str(), ios::in);
    set<string> stest;

    while(!ftest.eof()){

        int u, i; ftest>>u>>i; if(ftest.fail())   break;
        stest.insert(to_string(u)+'\t'+to_string(i));
    }
    
    // read predict
    ifstream fpredict(pfname.c_str(), ios::in);

    ofstream fout(ofname.c_str(), ios::out);
    fout.setf(ios::scientific, ios::floatfield); fout.precision(6);


    int nhit  = 0;              // hit count
    int npred = 0;              // predict count
    int ntest = stest.size();   // test count


    // key:     recall
    // value:   precision
    map<double,double> mpr;

    while(!fpredict.eof()){

        int u,i;    fpredict>>u>>i; if(fpredict.fail()) break;

        npred++;

        string key = to_string(u)+'\t'+to_string(i);
        if(stest.count(key)){
            nhit++;

            double recall = (double)nhit / ntest;
            double precis = (double)nhit / npred;

            mpr[recall] = precis;
        } 
    }

    double cur_prec = 0;
    for(map<double,double>::reverse_iterator i=mpr.rbegin(); i!=mpr.rend(); ++i){
        double recall = i->first;
        double precis = i->second;

        // non-decreasing precise
        if(i->second <= cur_prec)   continue;
        cur_prec = precis;

        double f1     = 2*precis*recall / (precis+recall);
        fout<<recall<<'\t'<<precis<<'\t'<<f1<<endl;
    }

    fpredict.close();
    fout.close();

    return 0;
}
