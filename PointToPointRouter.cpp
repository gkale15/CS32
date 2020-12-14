#include "provided.h"
#include "ExpandableHashMap.h"
#include <list>
#include <queue>
#include <vector>
using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
    
private:
    const StreetMap* m_sm;
    
    //function that gets the streetsegment that contains geocoord a and b
    void getSS(const GeoCoord a, const GeoCoord b, StreetSegment& s) const {
        vector<StreetSegment> av;
        m_sm->getSegmentsThatStartWith(b, av);
        for (int i = 0; i <av.size(); i++) {
            if(av[i].end.latitudeText == a.latitudeText && av[i].end.longitudeText == a.longitudeText) {
                s = av[i];
            }
        }
    }
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
:m_sm(sm)
{
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    ExpandableHashMap<GeoCoord, GeoCoord> toLastLink;
    //check if same point first
    totalDistanceTravelled = 0;
    if (start.latitudeText == end.latitudeText && start.longitudeText == end.longitudeText) {
        totalDistanceTravelled = 0;
        route.clear();
        return DELIVERY_SUCCESS;
    }
    //check if coords are valid
    route.clear();
    vector<StreetSegment>t;
    if(!m_sm->getSegmentsThatStartWith(start, t) || !m_sm->getSegmentsThatStartWith(end, t)) {
        totalDistanceTravelled = 0;
        route.clear();
        return BAD_COORD;
    }
    //have a queue based search algorithm
    queue<GeoCoord> openqueue;
    openqueue.push(start);
    //push the starting geocoord in there
    while(!openqueue.empty()) {
        //set comparison gc to the one at the start
        GeoCoord q = openqueue.front();
        openqueue.pop();
        t.clear();
        //get all the segments that start with this stuff
        m_sm->getSegmentsThatStartWith(q, t);
        //push geocoords into the queue while they are already not in the hashmap of geocoord to geocoord
        for(int i = 0; i<t.size(); i++) {
            if(toLastLink.find(t[i].end) == nullptr) {
                toLastLink.associate(t[i].end, q);
                openqueue.push(t[i].end);
                //openqueue.front() = t[i].end;
            }
            //if you find the end, then trace back through the hashtable, associating values as keys and until you get back to the starting value
            //the whole time push these as streetsegments into the route list
            if(t[i].end.latitudeText == end.latitudeText && t[i].end.longitudeText == end.longitudeText) {
                GeoCoord* g;
                g = toLastLink.find(t[i].end);
                totalDistanceTravelled+=distanceEarthMiles(t[i].end,(*g));
                StreetSegment s;
                getSS(t[i].end, *g, s);
                route.push_front(s);
                while (g->latitudeText != start.latitudeText || g->longitudeText != start.longitudeText) {
                    GeoCoord f = *g;
                    g = toLastLink.find(f);
                    //increment distance
                    totalDistanceTravelled+=distanceEarthMiles(*g, f);
                    getSS(f, *g, s);
                    route.push_front(s);
                }
                return DELIVERY_SUCCESS;
            }
        }
        
    }
    
    totalDistanceTravelled = 0;
    return NO_ROUTE;  // Delete this line and implement this function correctly
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
