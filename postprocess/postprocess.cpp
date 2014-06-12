#include<iostream>
#include<fstream>
#include<string>
#include"../datainfo.h"
#include"../util.h"

#include<boost/program_options.hpp>
namespace po = boost::program_options;


using namespace std;

// program options
po::options_description desc("Allowed options");
po::variables_map vm;

int train_begin, train_end, predict_end;
string pfname, ofname, cfname, lfname;


int main(int argc, char* argv[]){

    // Declare the supported options.

    desc.add_options()
        ("train-begin", po::value<int>(&train_begin)->default_value(TRAIN_BEGIN_DATE), "Begin date of training set")
        ("train-end", po::value<int>(&train_end)->default_value(TRAIN_END_DATE), "End date of training set")
        ("predict-end", po::value<int>(&predict_end)->default_value(PRED_END_DATE), "End date of prediction")

        ("predict-file,p", po::value<string>(&pfname)->required(), "De-compacted prediction file (required)")
        ("lasttransact-file,l", po::value<string>(&lfname)->required(), "Last-transaction-date file (required)")
        ("count-file,c", po::value<string>(&cfname)->required(), "Transaction-count file (required)")
        ("output-file,o", po::value<string>(&ofname)->required(), "Output file (required)")
        ("verbose,v", "Enable verbose output")
        ("help,h", "Display this message")
    ;

    try{
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);    
    }
    catch(exception e){ cout << desc <<endl; return 1; }

    if (vm.count("help")) { cout << desc <<endl; return 1; }


    // load last transaction dates
    map<string, int> mlastdate;
    ifstream fl(lfname.c_str(), ios::in);

    while(!fl.eof()){

        int u, i, date; fl>>u>>i>>date; if(fl.fail())   break;
        mlastdate[to_string(u)+'\t'+to_string(i)] = date;
    }
    fl.close();

    // load transaction counts
    // key:     iid
    // value:   < sales volumn, user count >
    map<int, pair<int,int> > mcount;
    ifstream fc(cfname.c_str(), ios::in);

    while(!fc.eof()){

        int i, r, c;   fc>>i>>r>>c;   if(fc.fail())   break;
        mcount[i] = make_pair(r, c);
    }

    if(vm.count("verbose")) cout<<"mcount:\n"<<mcount<<endl;

    fc.close();

    // post processing predict
    ifstream fp(pfname.c_str(), ios::in);

    ofstream fo(ofname.c_str(), ios::out);
    fo.setf(ios::scientific, ios::floatfield);  fo.precision(6);

    while(!fp.eof()){

        int u,i;    double p;
        fp>>u>>i>>p;    if(fp.fail())   break;

        string key = to_string(u)+'\t'+to_string(i);

        // user has bought it once
        if( mlastdate.find(key) != mlastdate.end() ){

            int count_train      = mcount[i].first;
            int users_train      = mcount[i].second;
            int duration_train   = train_end - train_begin + 1;
            int duration_predict = predict_end - mlastdate[key];

            p *= (double)count_train * duration_predict / duration_train / users_train;

            if(vm.count("verbose")){
                cout<<"key:             "<<key<<endl;
                cout<<"count_train:     "<<count_train<<endl;
                cout<<"users_train:     "<<users_train<<endl;
                cout<<"duration_train:  "<<duration_train<<endl;
                cout<<"duration_predict:"<<duration_predict<<endl;
                cout<<"p:               "<<p<<endl;
            } 

        }
        
        fo<<u<<'\t'<<i<<'\t'<<p<<endl;
    }

    fo.close(); fp.close();

    return 0;
}
