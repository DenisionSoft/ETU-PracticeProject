﻿#include "back.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
using boost::property_tree::ptree;

Schema Back::read_file(std::string filename) {

    ptree pt;
    read_xml(filename, pt);
    ptree scheme = pt.get_child("scheme");
    
    ptree sst = pt.get_child("scheme.size");
    int dim_x = sst.get<int>("cols", 0);
    int dim_y = sst.get<int>("rows", 0);
    double scale = sst.get<double>("scale", 1.0);

    Schema schema(dim_x, dim_y);

    schema.scale = scale;
    ptree sbt = scheme.get_child("bits");
    std::vector<integral_element> Curelems;

    BOOST_FOREACH(ptree::value_type & bit, sbt.get_child("")) {
        std::string id = bit.second.get<std::string>("<xmlattr>.bit_id");
        int id_i = stoi(id);

        int x = bit.second.get<int>("place.x");
        int y = bit.second.get<int>("place.y");

        std::string name = bit.second.get<std::string>("name", id);

        std::vector<connection> Curconns;
        if (id_i < 10000) {
            ptree blinkt = bit.second.get_child("links");
            BOOST_FOREACH(ptree::value_type & link, blinkt.get_child("")) {
                std::string lid = link.second.get<std::string>("<xmlattr>.link_id");
                int lid_i = stoi(lid);

                int initial = id_i;
                int final = link.second.get<int>("to");
                std::string name = link.second.get<std::string>("link_name", lid);
                int id = lid_i;

                connection Curcon(initial, final, id, name);
                Curconns.push_back(Curcon);
            }

        }

        integral_element Curbit(x, y, id_i, name);
        if (id_i < 10000) {
            Curbit.connections = Curconns;
        }
        Curelems.push_back(Curbit);
    }

    schema.elements = Curelems;

    ptree smt = scheme.get_child("map");

    std::vector<std::vector<int>> Curmap(dim_y,
        std::vector<int>(dim_x));

    BOOST_FOREACH(ptree::value_type & row, smt.get_child("")) {
        std::string rid = row.second.get<std::string>("<xmlattr>.y");
        int rid_i = stoi(rid);
        ptree rct = row.second;

        BOOST_FOREACH(ptree::value_type & col, rct.get_child("")) {
            if (col.first == "col") {
                std::string cid = col.second.get<std::string>("<xmlattr>.x");
                int cid_i = stoi(cid);
                Curmap[rid_i - 1][cid_i - 1] = col.second.get<int>("");
            }

        }
    }

    schema.schema_map = Curmap;
    return schema;

}

int Back::write_file(Schema schema, std::string filename) {
    ptree pt;
    ptree& scheme = pt.put("scheme", "");
    ptree& size = scheme.put("size", "");

    size.add("cols", schema.dimentions_x);
    size.add("rows", schema.dimentions_y);


    write_xml(filename , pt);
}
