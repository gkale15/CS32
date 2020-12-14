#include "provided.h"
#include <vector>
#include <map>
#include <list>
#include <cmath>
#include <algorithm>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
private:
    PointToPointRouter p;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
:p(sm)
{
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    //calcualte old crow distance piece by piece
    oldCrowDistance = 0;
    for(int i = 0; i < deliveries.size(); i++) {
        if(i==0) oldCrowDistance+=distanceEarthMiles(depot, deliveries[0].location);
        else {
            oldCrowDistance+=distanceEarthMiles(deliveries[i-1].location, deliveries[i].location);
        }
    }

    //a vector of pairs to radial angle from the depot, to the posiition in the original vector
    vector<pair<double, int>> radialdist;
    for(int i = 0; i <deliveries.size()-1; i++) {
        StreetSegment s = StreetSegment(depot, deliveries[i].location, "");
        double a = angleOfLine(s);
        //if the radial angle is greatre than 320, then just subtract by 360 because it is close enough to 0
        if (a > 320) a -= 360;
        radialdist.emplace_back(a,i);
    }
    
    //organize angles by value
    vector<DeliveryRequest> temp;
    sort(radialdist.begin(), radialdist.end());
    //push in the original requests into the temp vector in order of the index of the pair given
    for(int i = 0; i < radialdist.size(); i++) {
        DeliveryRequest r = deliveries[radialdist[i].second];
        temp.push_back(r);
    }
    //push the final depot position, and set the deliveries vector as the temporary one
    temp.push_back(deliveries.back());
    deliveries.clear();
    deliveries = temp;
    
    //calculate new crow distance piece by piece
    newCrowDistance = 0;
    for(int i = 0; i < deliveries.size(); i++) {
        if(i==0) newCrowDistance+=distanceEarthMiles(depot, deliveries[0].location);
        else {
            newCrowDistance+=distanceEarthMiles(deliveries[i-1].location, deliveries[i].location);
        }
    }
}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}

