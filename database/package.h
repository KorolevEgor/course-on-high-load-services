#ifndef AUTHOR_H
#define AUTHOR_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class Package{
        private:
            long _id;
            std::string _name;
            std::string _weight;
            std::string _price;
            long _delivery_id;

        public:

            static Package fromJSON(const std::string & str);

            long             get_id() const;
            const std::string &get_name() const;
            const std::string &get_weight() const;
            const std::string &get_price() const;
            const long &get_delivery_id() const;

            long&        id();
            std::string &name();
            std::string &weight();
            std::string &price();
            long &delivery_id();

            static void init();
            static std::optional<Package> read_by_id(long id);
            static std::vector<Package> search(std::string login);
            void save_to_mysql();

            Poco::JSON::Object::Ptr toJSON() const;

    };
}

#endif