#include "provided.h"
#include <vector>
#include <list>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
     const StreetMap* m_sm;
     PointToPointRouter p;
    
    //just a function to get the direction for a proceed function
    void getdirection(string& s, const StreetSegment ss) const {
        double d = angleOfLine(ss);
        if ((d>= 0 && d<=22.5) || d>=337.5) s = "east";
        else if (d>=22.5 && d <67.5) s = "northeast";
        else if (d>=67.5 && d <112.5) s = "north";
        else if (d>=112.5 && d <157.5) s = "northwest";
        else if (d>=157.5 && d <202.5) s = "west";
        else if (d>=202.5 && d <247.5) s = "southwest";
        else if (d>=247.5 && d <292.5) s = "south";
        else if (d>=292.5 && d <337.5) s = "southeast";
    }
    
    //a function that returns turn direction, or returns false if there it should be a proceed instead
    bool getTurnDir(string& s, const StreetSegment a, const StreetSegment b) const {
        double d = angleBetween2Lines(a, b);
        if(d< 1 || d>359) return false;
        
        if(d>= 1 && d < 180) {
            s = "left";
            return true;
        }
        if(d >= 180 && d <= 359) {
            s = "right";
            return true;
        }
        return false;
    }
    
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
:m_sm(sm), p(m_sm)
{
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    //make a temporary vector of DelivReq with an extra destination back to the depot pushed in as a last request
    vector<DeliveryRequest> addEnd;
    DeliveryRequest fin("", depot);
    addEnd = deliveries;
    addEnd.push_back(fin);
    
    //optimize the order of these requests
    DeliveryOptimizer dop(m_sm);
    double olddist,newdist;
    dop.optimizeDeliveryOrder(depot, addEnd, olddist, newdist);

    if(addEnd.size() == 0) return BAD_COORD;
    
    //double iterator implementation
    for(int k = 0; k < addEnd.size(); k++) {
        list<StreetSegment> firstseg;
        double firstdist;
        if (k == 0) {
            if(p.generatePointToPointRoute(depot, addEnd[0].location, firstseg, firstdist) == BAD_COORD) return BAD_COORD;
            if(p.generatePointToPointRoute(depot, addEnd[0].location, firstseg, firstdist) == NO_ROUTE) return NO_ROUTE;
        }
        else{
            if(p.generatePointToPointRoute(addEnd[k-1].location, addEnd[k].location, firstseg, firstdist) == BAD_COORD) return BAD_COORD;
            if(p.generatePointToPointRoute(addEnd[k-1].location, addEnd[k].location, firstseg, firstdist) == NO_ROUTE) return NO_ROUTE;
        }
        list<StreetSegment>::iterator it = firstseg.begin();
        list<StreetSegment>::iterator it2 = firstseg.begin();
        //for each list of streetsegments, first check if it is a turn statement by seeing if the streetnames of that and next are not the same
        //insert turn statement, or continue if it needs to be a proceed statement
        while (it!= firstseg.end()) {
            it2 = it;
            if(!commands.empty() && it->name != commands.back().streetName() && commands.back().streetName() != "") {
                it2--;
                string s;
                DeliveryCommand turn;
                if(!getTurnDir(s, *it2, *it)) {
                    it2 = it;
                }
                else {
                    
                    turn.initAsTurnCommand(s, it->name);
                    commands.push_back(turn);
                    it2 = it;
                }
            
            }
        
            //while you still encounter the same street name, increment the second iteraotr until you get to the end of it, and get distance of the bit from the start of that street to the end of it
            if(it->name == it2->name) {
            
                while(it->name == it2->name && it2!=firstseg.end()) {
                    it2++;
                }
                it2--;
                list<StreetSegment>temp;
                double streetdist;
                p.generatePointToPointRoute(it->start, it2->end, temp, streetdist);
                string dir;
                getdirection(dir, *it);
            
                DeliveryCommand d;
                d.initAsProceedCommand(dir, it->name, streetdist);
                totalDistanceTravelled += streetdist;
                commands.push_back(d);
                it = it2;
                //if you reach the final ending location, return that the delivery was a success
                //otherwise push in as a simple proceed command
                if(it->end == addEnd[k].location) {
                    if(addEnd[k].location == depot)  {
                        return DELIVERY_SUCCESS;
                    }
                    DeliveryCommand a;
                    a.initAsDeliverCommand(addEnd[k].item);
                    commands.push_back(a);
                }
                it++;
            }
        }
    }
    
    
    
    
    return NO_ROUTE;
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}






//                getdirection(s, *it);
//                double distturn;
//                list<StreetSegment> turnss;
//                it2=it;
//                while(it->name == it2->name && it2!=firstseg.end()) {
//                    it2++;
//                }
//                it2--;
//                p.generatePointToPointRoute(it->start, it2->end, turnss, distturn);
//                turn.initAsProceedCommand(s, it->name, distturn);
