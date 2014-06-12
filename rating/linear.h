#include<vector>
#include<map>

typedef std::pair<int, double> pid;
typedef std::vector<pid> curve;
typedef std::vector<pid>::iterator curve_i;
typedef std::vector<pid>::const_iterator curve_ci;

// get interpolation value y respective to x
extern double interpolation(curve line, int x);

// get delta y between x1 and x2
extern double delta_y(curve line, int x1, int x2);

// initialize integration map
extern curve read_curve(std::string fname);

// do integral
extern curve integral(const curve& line);

// output curve
std::ostream& operator<<(std::ostream& o, const curve& line);
