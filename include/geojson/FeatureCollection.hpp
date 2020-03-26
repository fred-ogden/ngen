#ifndef GEOJSON_FEATURE_COLLECTION_H
#define GEOJSON_FEATURE_COLLECTION_H

#include "Feature.hpp"
#include <memory>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <typeinfo>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace geojson {
    class FeatureCollection {
        public:
            typedef std::vector<Feature> FeatureList;

            FeatureCollection(FeatureList &features, std::vector<double> bounding_box)
            : features(features),
                bounding_box(bounding_box)
            {}

            FeatureCollection(const FeatureCollection &feature_collection) {
                bounding_box = feature_collection.get_bounding_box();
                for (Feature feature : feature_collection) {
                    features.push_back(feature);
                }
            }

            virtual ~FeatureCollection(){};

            typedef FeatureList::iterator iterator;

            typedef FeatureList::const_iterator const_iterator;

            int get_size() {
                return features.size();
            };

            std::vector<double> get_bounding_box() const {
                return bounding_box;
            }

            Feature get_feature(int index) const {
                return features[index];
            };

            const_iterator begin() const {
                return features.cbegin();
            }

            const_iterator end() const {
                return features.cend();
            }

            static FeatureCollection read(const std::string &file_path) {
                boost::property_tree::ptree tree;
                boost::property_tree::json_parser::read_json(file_path, tree);
                return FeatureCollection::parse(tree);
            }

            static FeatureCollection read(std::stringstream &data) {
                boost::property_tree::ptree tree;
                boost::property_tree::json_parser::read_json(data, tree);
                return FeatureCollection::parse(tree);
            }

            static FeatureCollection parse(const boost::property_tree::ptree json) {
                std::vector<std::string> bounding_box;
                for (auto &feature : json.get_child("bbox")) {
                    bounding_box.push_back(feature.second.data());
                }
                std::vector<double> bbox_values;

                for (int point_index = 0; point_index < bounding_box.size(); point_index++) {
                    bbox_values.push_back(std::stod(bounding_box[point_index]));
                }

                FeatureList features;
                boost::optional<const boost::property_tree::ptree&> e = json.get_child_optional("features");

                if (e) {
                    for(auto feature_tree : *e) {    
                        features.push_back(Feature(feature_tree.second));
                    }
                }
                else {
                    std::cout << "No features were found" << std::endl;
                }

                return FeatureCollection(features, bbox_values);
            }

        private:
            FeatureList features;
            std::vector<double> bounding_box;
    };
}

#endif // GEOJSON_FEATURE_COLLECTION_H