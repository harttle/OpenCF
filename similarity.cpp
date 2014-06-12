#include<iostream>
#include<fstream>
#include<string>
#include"datainfo.h"
#include"util.h"

#include<boost/program_options.hpp>
namespace po = boost::program_options;


using namespace std;

// program options
po::options_description desc("Allowed options");
po::variables_map vm;

int nuser, nitem, nrating, base, adjust;
string ifname, ofname;


// print cos
// compute cos sim among rows, print to file 
void print_cos(mat& m){
    cout<<"calc cosine similarity..."<<endl;

    normalize2_row(m);

    if(vm.count("verbose"))
        cout<<"normalized matrix:\n"<<m<<endl;


    cout<<"dot product every combination..."<<endl;
    ofstream fout(ofname.c_str(), ios::out);
    fout.setf(ios::scientific, ios::floatfield); fout.precision(6);


    int n = m.size1();
    for(int i=0; i<n; i++){
        cout<<(i+1)*100/n<<"%\r"; cout.flush();

        row ri(m, i);

        for(int j=i+1; j<n; j++){
            row rj(m, j);

            double cos = inner_prod(ri, rj);
            if(cos) fout<<i<<'\t'<<j<<'\t'<<cos<<endl;
        }
    }
    fout.close(); cout<<endl;
}

int main(int argc, char* argv[]){

    // Declare the supported options.

    desc.add_options()
        ("base,b", po::value<int>(&base)->default_value(1), "Specify user/item based similarity:\n0 :\tuser based\n1 :\titem based")
        ("user-count,u", po::value<int>(&nuser)->default_value(USER_COUNT), "User count")
        ("item-count,i", po::value<int>(&nitem)->default_value(ITEM_COUNT), "Item count")
        ("rating-count,r", po::value<int>(&nrating)->default_value(RATING_COUNT), "Rating count")
        ("input-file,f", po::value<string>(&ifname)->required(), "Input file (required)")
        ("output-file,o", po::value<string>(&ofname)->required(), "Output file (required)")
        ("adjust,a", po::value<int>(&adjust)->default_value(2), "Index to remove average value:\n0 : adjust user-index \n1 : adjust item-index\n2 : no-adjust")
        ("verbose,v", "Enable verbose output")
        ("help,h", "Display this message")
    ;

    try{
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);    
    }
    catch(exception e){
        cout << "Usage: similarity [options]"<<endl;
        cout << desc <<endl;
        return 1;
    }

    if (vm.count("help")) {
        cout << "Usage: similarity [options]"<<endl;
        cout << desc <<endl;
        return 1;
    }


    shared_ptr<mat> pm = read_rating(ifname,
        adjust < 2 ? adjust : base,
        nuser , nitem, nrating);
    if(vm.count("verbose"))
        cout<<"rating matrix:\n"<<*pm<<endl;


    if(adjust < 2){

        remove_avg_row(*pm);

        if(vm.count("verbose"))
            cout<<"adjusted matrix:\n"<<*pm<<endl;
    }

    if(adjust < 2 && adjust != base){

        *pm = trans(*pm);
        if(vm.count("verbose")) cout<<"transposed matrix:\n"<<*pm<<endl;
    }

    print_cos(*pm);

    return 0;
}
