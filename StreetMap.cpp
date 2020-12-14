#include "provided.h"
#include "ExpandableHashMap.h"
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <fstream>
using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<std::string>()(g.latitudeText + g.longitudeText);
}


class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;

private:
    ExpandableHashMap<GeoCoord, vector<StreetSegment>> m_thestreets;
    
    
};

StreetMapImpl::StreetMapImpl()
:m_thestreets()
{}

StreetMapImpl::~StreetMapImpl()
{}

bool StreetMapImpl::load(string mapFile)
{
    //load the passed in file as an ifstream
    ifstream infile(mapFile);
    if (!infile) return false;
    
    
    string lat1,lon1,lat2,lon2;
    GeoCoord s,e;
    StreetSegment ss;
    vector<StreetSegment> ssvec;
    string stname;
    
    //first line will always be street, so save it as a streetname
    while(getline(infile, stname)) {
        //next line will always be a number, so save as an int
        string numtimes;
        getline(infile, numtimes);
        //for that int number of times, run through for loop and set latitude and longitude variables accordingly per line
        for(int i = 0; i < stoi(numtimes); i++) {
            
            string line;
            getline(infile, line);
            istringstream iss(line);
            iss>>lat1>>lon1>>lat2>>lon2;
            iss.ignore(10000,'\n');
            
            s = GeoCoord(lat1, lon1);
            e = GeoCoord(lat2, lon2);
            //if the streetsegment from s to e is not in the map, push it in
            if (m_thestreets.find(s) == nullptr) {
                ss =  StreetSegment(s, e, stname);
                ssvec.clear();
                ssvec.push_back(ss);
                m_thestreets.associate(s,ssvec);
                ssvec.clear();
               //if the streetsegment from e to s is not in the map, push it in
                if (m_thestreets.find(e) == nullptr) {
                    ss = StreetSegment(e, s, stname);
                    ssvec.push_back(ss);
                    m_thestreets.associate(e, ssvec);
                    ssvec.clear();
                }
                //else updqate the association that has e as keytype
                else {
                    vector<StreetSegment>* updatevec;
                    updatevec = m_thestreets.find(e);
                    ss =  StreetSegment(e, s, stname);
                    updatevec->push_back(ss);
                }
            }
            
            //else updqate the association that has e as keytype
            else if (i < stoi(numtimes)) {
                vector<StreetSegment>* updatevec;
                updatevec = m_thestreets.find(s);
                ss =  StreetSegment(s, e, stname);
                updatevec->push_back(ss);
                //if the streetsegment from s to e is not in the map for a line that is not i =0, push it in
                if (m_thestreets.find(e) == nullptr) {
                    ss = StreetSegment(e, s, stname);
                    ssvec.push_back(ss);
                    m_thestreets.associate(e, ssvec);
                    ssvec.clear();
                }
                //else update association
                else {
                    updatevec = m_thestreets.find(e);
                    ss =  StreetSegment(e, s, stname);
                    updatevec->push_back(ss);
                }
            }
            
        }
        
        
    }
    return true;  // Delete this line and implement this function correctly
}

//just utilizing the find function to change return types and stuff
bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    if(m_thestreets.find(gc) == nullptr) {
        return false;
    }
    segs = *m_thestreets.find(gc);
    return true;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}
