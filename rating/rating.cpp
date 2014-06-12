#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include"linear.h"
#include"../datainfo.h"
#include"../util.h"

#include<boost/program_options.hpp>
namespace po = boost::program_options;

#include<boost/date_time/gregorian/gregorian.hpp>
namespace gg = boost::gregorian;


using namespace std;

typedef map<int, int> mii;
typedef map<int, int>::const_iterator mii_ci;
typedef map<string, int> msi;
typedef map<string, int>::const_iterator msi_ci;
typedef map<string, double> msd;
typedef map<string, double>::const_iterator msd_ci;


// Declare the supported options.
po::options_description desc("Allowed options");
po::variables_map vm;

int predict_begin, predict_end;
string ifname, ofname, lfname, cfname;


void rating(const msi& mop, const msi& mtransaction, msd& m_ratings, const string curve_name){
    int x1 = predict_begin - 1;    // integral sector: [begin, end]
    int x2 = predict_end;

    cout<<"computing: "<<curve_name<<", operation map size: "<<mop.size()<<endl;
    if(vm.count("verbose")){
        cout<<"operation matrix:\n"<<mop<<endl;
        cout<<"transaction matrix:\n"<<mtransaction<<endl;
        cout<<"x1: "<<x1<<", x2: "<<x2<<endl;
    }

    // key:     ts
    // value:   count
    mii mcurve;
    for(msi_ci opt = mop.begin(); opt != mop.end(); ++opt){
        
        msi_ci transaction = mtransaction.find(opt->first);

        // didn't buy or bought without opt
        if(transaction == mtransaction.end() || transaction->second < opt->second)    continue;

        mcurve[transaction->second - opt->second]++;
    }
    

    double total = mop.size();

    curve line;
    for(mii_ci i = mcurve.begin(); i!=mcurve.end(); ++i)
        line.push_back(make_pair(i->first, i->second / total));
    line = integral(line);

    ofstream fcurve(curve_name.c_str(), ios::out);
    fcurve<<line;
    fcurve.close();

    for(msi_ci i=mop.begin(); i!=mop.end(); ++i){

        double rt = delta_y(line, x1 - i->second, x2 - i->second);
        if(rt)  m_ratings[i->first] = rt;
    }

    if(vm.count("verbose")) cout<<"rating matrix:\n"<<m_ratings<<endl;
}

void help(po::options_description desc){
    cout << "Usage: curve [options]"<<endl;
    cout << desc <<endl;
}

int main(int argc, char* argv[]){

    desc.add_options()
        ("begin-day,b", po::value<int>(&predict_begin)->default_value(PRED_BEGIN_DATE), "The first day in prediction duration")
        ("end-day,e", po::value<int>(&predict_end)->default_value(PRED_END_DATE), "The last day in prediction duration")
        ("input-file,i", po::value<string>(&ifname)->required(), "Input file (required)")
        ("output-file,o", po::value<string>(&ofname)->required(), "Output file (required)")
        ("lasttransact-file,l", po::value<string>(&lfname)->required(), "Output last-deal-date file (required)")
        ("count-file,c", po::value<string>(&cfname)->required(), "Output transaction-count file (required)")
        ("verbose,v", "Enable verbose output")
        ("help,h", "Print this help message")
    ;

    try{
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);    
    }
    catch(exception e){ help(desc); return 1; }

    if (vm.count("help")) { help(desc); return 1; }
    

    // key:     uid iid
    // value:   first hit/star/cart/transact day
    msi mdate[4];

    // key:     uid iid
    // value:   last transact day
    msi mlastdate;

    // key:     iid 
    // value:   transact count
    mii mtransactcount;

    // key:     iid
    // value:   users
    map<int, set<int> > musers;

    ifstream fin(vm["input-file"].as<string>().c_str(), ios::in);
    while(!fin.eof()){

        int uid, iid, op, month, day;
        fin>>uid>>iid>>op>>month>>day; if(fin.fail()) break;

        int date = gg::date_period(gg::date(2013, 1, 1), gg::date(2013, month, day)).length().days();
        string key = to_string(uid) + "\t" + to_string(iid);

        msi& m = mdate[op];
        if(m.find(key) == m.end())  m[key] = date;
        else    m[key] = min(m[key], date);

        if(op==1){
            mlastdate[key] = max(mlastdate[key], date);
            mtransactcount[iid]++;
            musers[iid].insert(uid);
        }
    }
    fin.close(); 


    // save last transaction dates
    ofstream fl(lfname.c_str(), ios::out);
    for(msi_ci i=mlastdate.begin(); i!=mlastdate.end(); ++i){
        fl<<i->first<<'\t'<<i->second<<endl;
    }
    fl.close();

    // save transaction counts
    ofstream fc(cfname.c_str(), ios::out);
    for(mii_ci i=mtransactcount.begin(); i!=mtransactcount.end(); ++i){
        fc<<i->first<<'\t'<<i->second<<'\t'<<musers[i->first].size()<<endl;
    }
    fc.close();


    // do rating
    msd m_ratings;

    cout<<"transaction count:\t"<<mdate[1].size()<<endl;
    rating(mdate[0], mdate[1], m_ratings, ofname + ".hit_curve");
    rating(mdate[2], mdate[1], m_ratings, ofname + ".star_curve");
    rating(mdate[3], mdate[1], m_ratings, ofname + ".cart_curve");

    for(msi_ci i=mdate[1].begin(); i!=mdate[1].end(); ++i) m_ratings[i->first] = 1;

    ofstream fout(ofname.c_str(), ios::out);
    fout.setf(ios::scientific, ios::floatfield); fout.precision(6);

    for(msd_ci i=m_ratings.begin(); i!= m_ratings.end(); ++i)   fout<<i->first<<'\t'<<i->second<<endl;

    fout.close();
    cout<<"rating saved to:\t"<<ofname<<endl;

    return 0;
}

