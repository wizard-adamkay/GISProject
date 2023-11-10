//
// Created by Adam k on 11/22/2022.
//

#ifndef GISPROJECT_COORDINATEINDEX_H
#define GISPROJECT_COORDINATEINDEX_H

#include <vector>
#include <string>
#include "GISRecord.h"

using namespace std;

struct Point {
    int lon;
    int lat;
    Point(int _lon, int _lat)
    {
        lon = _lon;
        lat = _lat;
    }
};

struct Info {
    vector<vector<int>> indices;
    vector<Point> points;
    Info()= default;
};

struct QuadNode {
    Point nwBound;
    Point seBound;
    Info info;
    QuadNode *ne;
    QuadNode *nw;
    QuadNode *se;
    QuadNode *sw;
    QuadNode(const Point & nwb, const Point & seb, QuadNode *tr, QuadNode *tl, QuadNode *br, QuadNode *bl): nwBound{nwb}, seBound{seb}, ne{tr}, nw{tl}, se{br}, sw{bl} {}
};

class Quadtree {
public:
    Quadtree(){
        root = nullptr;
    }
    Quadtree(Point nwb, Point seb){
        root = new QuadNode(nwb, seb, nullptr, nullptr, nullptr, nullptr);
    }
    vector<int> indexOf(const int & lon, const int & lat, int latArea, int lonArea) {
        Point nwb(lon-lonArea, lat+latArea);
        Point seb(lon+lonArea, lat-latArea);
        return indexOf(*root, nwb, seb);
    }

    void insert(const int & lon, const int & lat, int offset) {
        Point p(lon, lat);
        insert(p, offset, *root);
    }

    string debug(){
        return "root" + debug(*root, 0);
    }
private:
    QuadNode *root;
    int splitOn = 4;

    void insert(const Point & p, int offset, QuadNode& quadNode ) {
        if (quadNode.ne == nullptr) {
            int index = 0;
            for (auto &point: quadNode.info.points) {
                if (p.lon == point.lon && p.lat == point.lat) {
                    quadNode.info.indices[index].push_back(offset);
                    return;
                }
                index++;
            }
            if(quadNode.info.points.size() >= splitOn){
                int centerLon = (quadNode.nwBound.lon + quadNode.seBound.lon)/2;
                int centerLat = (quadNode.nwBound.lat + quadNode.seBound.lat)/2;

                Point centerPoint(centerLon, centerLat);
                quadNode.ne = new QuadNode{Point(centerPoint.lon, quadNode.nwBound.lat), Point(quadNode.seBound.lon, centerPoint.lat), nullptr, nullptr, nullptr, nullptr};
                quadNode.nw = new QuadNode{quadNode.nwBound,centerPoint, nullptr, nullptr, nullptr, nullptr};
                quadNode.se = new QuadNode{centerPoint, quadNode.seBound, nullptr, nullptr, nullptr, nullptr};
                quadNode.sw = new QuadNode{Point(quadNode.nwBound.lon, centerPoint.lat), Point(centerPoint.lon, quadNode.seBound.lat), nullptr, nullptr, nullptr, nullptr};
                for(int i = 0; i < quadNode.info.points.size(); i++){
                    for(int j = 0; j < quadNode.info.indices[i].size(); j++){
                        insert(quadNode.info.points[i], quadNode.info.indices[i][j], quadNode);
                    }
                }
                quadNode.info.points.clear();
                quadNode.info.indices.clear();
            } else {
                quadNode.info.points.push_back(p);
                vector<int> off;
                off.push_back(offset);
                quadNode.info.indices.push_back(off);
                return;
            }
        }
        int centerLon = (quadNode.nwBound.lon + quadNode.seBound.lon)/2;
        int centerLat = (quadNode.nwBound.lat + quadNode.seBound.lat)/2;
        if(p.lon >= centerLon && p.lat >= centerLat) {
            insert(p, offset, *quadNode.ne);
       } else if(p.lon < centerLon && p.lat >= centerLat) {
            insert(p, offset, *quadNode.nw);
        }else if(p.lon > centerLon && p.lat <= centerLat) {
            insert(p, offset, *quadNode.se);
        } else {
            insert(p, offset, *quadNode.sw);
        }
    }

    vector<int> indexOf(const QuadNode& quadNode, Point nwb, Point seb) {
        if (quadNode.ne == nullptr) {
            vector<int> offsets;
            int index = 0;
            for (auto &point: quadNode.info.points) {
                if (pointWithinArea(point, nwb, seb)) {
                    for(int offset : quadNode.info.indices[index]){
                        offsets.push_back(offset);
                    }
                }
                index++;
            }
            return offsets;
        }
        vector<vector<int>> offsetsList;
        if(areaWithinArea(quadNode.ne->nwBound, quadNode.ne->seBound, nwb, seb)) {
            offsetsList.push_back(indexOf(*quadNode.ne, nwb, seb));
        }
        if(areaWithinArea(quadNode.nw->nwBound, quadNode.nw->seBound, nwb, seb)) {
            offsetsList.push_back(indexOf(*quadNode.nw, nwb, seb));
        }
        if(areaWithinArea(quadNode.se->nwBound, quadNode.se->seBound, nwb, seb)) {
            offsetsList.push_back(indexOf(*quadNode.se, nwb, seb));
        }
        if(areaWithinArea(quadNode.sw->nwBound, quadNode.sw->seBound, nwb, seb)){
            offsetsList.push_back(indexOf(*quadNode.sw, nwb, seb));
        }
        vector<int> offsets;
        for(const vector<int>& i : offsetsList){
            for(int offset : i){
                offsets.push_back(offset);
            }
        }
        return offsets;
    }

    string debug(const QuadNode& q, int depth){
        string s;
        string indent;
        for(int i = 0; i < depth; i++){
            indent += '\t';
        }
        depth++;
        if(q.ne == nullptr){
            s = "{";
            for(int i = 0; i < q.info.points.size(); i++){
                s+="[("+to_string(q.info.points[i].lat)+','+to_string(q.info.points[i].lon)+')';
                for(int index : q.info.indices[i]){
                    s+=','+ to_string(index);
                }
                s+=']';
            }
            s+='}';
            return s;
        }
        s = '\n';
        s += indent+"ne>"+debug(*q.ne, depth)+'\n';
        s += indent+"nw>"+debug(*q.nw, depth)+'\n';
        s += indent+"se>"+debug(*q.se, depth)+'\n';
        s += indent+"sw>"+debug(*q.sw, depth)+'\n';
        return s;
    }

    static bool pointWithinArea(Point p, Point nwb, Point seb){
        if(p.lon >= nwb.lon && p.lon <= seb.lon && p.lat >= seb.lat && p.lat <= nwb.lat){
            return true;
        }
        return false;
    }

    static bool areaWithinArea(Point nwb1, Point seb1, Point nwb2, Point seb2){
        if(nwb2.lon == seb2.lon && nwb2.lat == seb2.lat){
            return pointWithinArea(nwb2, nwb1, seb1);
        }
        if (nwb1.lon > seb2.lon || nwb2.lon > seb1.lon){
            return false;
        }
        if (seb1.lat > nwb2.lat || seb2.lat > nwb1.lat){
            return false;
        }
        return true;
    }
};

class CoordinateIndex {
public:
    CoordinateIndex()= default;
    CoordinateIndex(int northWestBoundLon, int northWestBoundLat, int southEastBoundLon, int southEastBoundLat){
        Point nwb(northWestBoundLon, northWestBoundLat);
        Point seb(southEastBoundLon, southEastBoundLat);
        quadtree = Quadtree(nwb, seb);
    }

    vector<int> find(int lon, int lat, int lonArea, int latArea) {
        return quadtree.indexOf(lon, lat,  latArea, lonArea);
    }

    void insert(const string& line, int offset) {
        vector<string> processedLines = processLine(line);
        int lon = DMStoS(processedLines[8]);
        int lat = DMStoS(processedLines[7]);
        quadtree.insert(lon, lat, offset);
    }

    string debug(){
        return quadtree.debug();
    }

private:
    Quadtree quadtree;
    static vector<string> processLine(const string& line){
        string holder;
        vector<string> splitInputs;
        for(char i : line) {
            if (i != '|') {
                holder += i;
            } else {
                splitInputs.push_back(holder);
                holder = "";
            }
        }
        return splitInputs;
    }

    static int DMStoS(const string& dms){
        int deg, min, sec;
        int neg = 1;
        int lon = 0;
        if ((dms[dms.length() - 1] == 'W') || (dms[dms.length() - 1] == 'E'))
        {
            lon++;
        }
        if ((dms[dms.length() - 1] == 'W') || (dms[dms.length() - 1] == 'S'))
        {
            neg = -1;
        }
        deg = stoi(dms.substr(0,2+lon));
        min = stoi(dms.substr(2+lon, 2));
        sec = stoi(dms.substr(4+lon, 2));
        int total = (((deg*60+min) * 60) + sec) * neg;
        return total;
    }
};
#endif //GISPROJECT_COORDINATEINDEX_H
