//#include "ExpandableHashMap.h"
//#include "provided.h"
//#include <iostream>
//#include <list>
//#include <vector>
//using namespace std;
//
//int main() {
//    StreetMap sm;
//
//    if(sm.load("/Users/gkale/Desktop/mapdata.txt"))
//        std::cout << "true"<< std::endl;
//
//    DeliveryPlanner dp(&sm);
//    vector<DeliveryCommand> a;
//    vector<DeliveryRequest> f;
//    GeoCoord depot("34.0625329", "-118.4470263");
//    GeoCoord end3("34.0616291", "-118.4416199");
//    GeoCoord end("34.0712323", "-118.4505969");
//    GeoCoord end4("34.0656797", "-118.4505131");
//
//    DeliveryRequest dq("Pabst Blue Ribbon beer", end);
//    DeliveryRequest dt("Chicken tenders", end3);
//    DeliveryRequest dkk("B-Plate salmon", end4);
//    double ay;
//    f.push_back(dt);
//    f.push_back(dkk);
//    f.push_back(dq);
//
//    dp.generateDeliveryPlan(depot, f, a, ay);
//    cout << ay <<endl;
//}


//    double die;
//    std::list<StreetSegment> s;
//    GeoCoord a = GeoCoord("34.0356922", "-118.4937358");
//    GeoCoord b = GeoCoord("34.0462248", "-118.4958401");
//    p.generatePointToPointRoute(a, b, s, die);
//    std::cout << die<<std::endl;
//    std::cout<<s.size()<<std::endl;
//    std::list<StreetSegment>::iterator it = s.begin();
//    while (it!=s.end()) {
//        std::cout << it->name<<std::endl;
//        std::cout << it->start.latitudeText<<std::endl;
//        std::cout << it->start.longitudeText<<std::endl;
//        std::cout << it->end.latitudeText<<std::endl;
//        std::cout << it->end.longitudeText<<std::endl;
//        std::cout << std::endl;
//        it++;
//    }

//    StreetMap sm;
//        sm.load("/Users/gkale/Desktop/smallermap.txt");
//
//    PointToPointRouter p2p(&sm);
//    GeoCoord start("1", "1");
//    GeoCoord end("6", "6");
//    double totalDistanceTravelled = 0;
//    list<StreetSegment> route;
//    p2p.generatePointToPointRoute(start, end, route, totalDistanceTravelled);
//
//    list<StreetSegment> :: iterator it;
//    it = route.begin();
//    cerr<<it->name << " " << it->start.latitudeText << " " << it->start.longitudeText<<endl;
//    for (; it != route.end(); it++)
//    cerr <<(*it).name<< " " <<(*it).end.latitudeText << " " << (*it).end.longitudeText<<endl;

//}






#include "provided.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v);
bool parseDelivery(string line, string& lat, string& lon, string& item);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " mapdata.txt deliveries.txt" << endl;
        return 1;
    }

    StreetMap sm;

    if (!sm.load(argv[1]))
    {
        cout << "Unable to load map data file " << argv[1] << endl;
        return 1;
    }

    GeoCoord depot;
    vector<DeliveryRequest> deliveries;
    if (!loadDeliveryRequests(argv[2], depot, deliveries))
    {
        cout << "Unable to load delivery request file " << argv[2] << endl;
        return 1;
    }

    cout << "Generating route...\n\n";

    DeliveryPlanner dp(&sm);
    vector<DeliveryCommand> dcs;
    double totalMiles;
    DeliveryResult result = dp.generateDeliveryPlan(depot, deliveries, dcs, totalMiles);
    if (result == BAD_COORD)
    {
        cout << "One or more depot or delivery coordinates are invalid." << endl;
        return 1;
    }
    if (result == NO_ROUTE)
    {
        cout << "No route can be found to deliver all items." << endl;
        return 1;
    }
    cout << "Starting at the depot...\n";
    for (const auto& dc : dcs)
        cout << dc.description() << endl;
    cout << "You are back at the depot and your deliveries are done!\n";
    cout.setf(ios::fixed);
    cout.precision(2);
    cout << totalMiles << " miles travelled for all deliveries." << endl;
}

bool loadDeliveryRequests(string deliveriesFile, GeoCoord& depot, vector<DeliveryRequest>& v)
{
    ifstream inf(deliveriesFile);
    if (!inf)
        return false;
    string lat;
    string lon;
    inf >> lat >> lon;
    inf.ignore(10000, '\n');
    depot = GeoCoord(lat, lon);
    string line;
    while (getline(inf, line))
    {
        string item;
        if (parseDelivery(line, lat, lon, item))
            v.push_back(DeliveryRequest(item, GeoCoord(lat, lon)));
    }
    return true;
}

bool parseDelivery(string line, string& lat, string& lon, string& item)
{
    const size_t colon = line.find(':');
    if (colon == string::npos)
    {
        cout << "Missing colon in deliveries file line: " << line << endl;
        return false;
    }
    istringstream iss(line.substr(0, colon));
    if (!(iss >> lat >> lon))
    {
        cout << "Bad format in deliveries file line: " << line << endl;
        return false;
    }
    item = line.substr(colon + 1);
    if (item.empty())
    {
        cout << "Missing item in deliveries file line: " << line << endl;
        return false;
    }
    return true;
}
