//
// Created by Adam k on 11/22/2022.
//

#ifndef GISPROJECT_COORDINATEINDEX_H
#define GISPROJECT_COORDINATEINDEX_H

#include <vector>

using namespace std;

struct Point {
    double lon;
    double lat;
    Point(double _lon, double _lat)
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

    vector<int> indexOf(const double & lon, const double & lat) {
        Point p(lon, lat);
        return indexOf(p, *root);
    }

    void insert(const double & lon, const double & lat, int offset) {
        Point p(lon, lat);
        insert(p, offset, *root);
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
                double centerLon = (quadNode.nwBound.lon + quadNode.seBound.lon)/2.0;
                double centerLat = (quadNode.nwBound.lat + quadNode.seBound.lat)/2.0;
                Point centerPoint(centerLon, centerLat);
                //nw se
                quadNode.ne = new QuadNode{topcenter, eastcenter, nullptr, nullptr, nullptr, nullptr};
                quadNode.nw = new QuadNode{quadNode.nwBound,centerPoint, nullptr, nullptr, nullptr, nullptr};
                quadNode.se = new QuadNode{centerPoint, quadNode.seBound, nullptr, nullptr, nullptr, nullptr};
                quadNode.sw = new QuadNode{westcenter, southcenter, nullptr, nullptr, nullptr, nullptr};
            } else {
                quadNode.info.points.push_back(p);
                vector<int> off;
                off.push_back(offset);
                quadNode.info.indices.push_back(off);
                return;
            }
        }
        double centerLon = (quadNode.nwBound.lon + quadNode.seBound.lon)/2.0;
        double centerLat = (quadNode.nwBound.lat + quadNode.seBound.lat)/2.0;
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

    vector<int> indexOf(const Point & p, const QuadNode& quadNode) {
        if (quadNode.ne == nullptr) {
            int index = 0;
            for (auto &point: quadNode.info.points) {
                if (p.lon == point.lon && p.lat == point.lat) {
                    return quadNode.info.indices[index];
                }
                index++;
            }
            vector<int> notFound;
            notFound.push_back(-1);
            return notFound;
        }
        double centerLon = (quadNode.nwBound.lon + quadNode.seBound.lon)/2.0;
        double centerLat = (quadNode.nwBound.lat + quadNode.seBound.lat)/2.0;
        if(p.lon >= centerLon && p.lat >= centerLat) {
            return indexOf(p, *quadNode.ne);
        } else if(p.lon < centerLon && p.lat >= centerLat) {
            return indexOf(p, *quadNode.nw);
        }else if(p.lon > centerLon && p.lat <= centerLat) {
            return indexOf(p, *quadNode.se);
        } else {
            return indexOf(p, *quadNode.sw);
        }
    }

};
#endif //GISPROJECT_COORDINATEINDEX_H
