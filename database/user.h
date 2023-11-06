#ifndef AUTHOR_H
#define AUTHOR_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class User{
        private:
            long _id;
            std::string _first_name;
            std::string _last_name;
            std::string _addres;
            std::string _login;
            std::string _password;

            void save_to_cache();
            static User get_from_cache_by_id(long id);
            static User get_from_cache_by_login(std::string login);
            static bool search_by_cache(User &likeUser, std::vector<User> &search_results);
            static void save_search_to_cache(User &like_user, std::vector<User> &search_results);
            static std::string build_search_cache_key(User &like_user);

        public:

            static User empty() {
                User user;
                user._id = -1;
                return user;
            }

            static User fromJson(const std::string &json);
            static User from_json_object(Poco::JSON::Object::Ptr);

            long             get_id() const;
            const std::string &get_first_name() const;
            const std::string &get_last_name() const;
            const std::string &get_addres() const;
            const std::string &get_login() const;
            const std::string &get_password() const;

            long&        id();
            std::string &first_name();
            std::string &last_name();
            std::string &addres();
            std::string &login();
            std::string &password();

            static void init();
            static std::optional<User> read_by_id(long id);
            static std::optional<User> read_by_login(std::string &login);
            static std::optional<long> auth(std::string &login, std::string &password);
            static std::vector<User> read_all();
            static std::vector<User> search(std::string first_name,std::string last_name);
            void save_to_mysql();

            Poco::JSON::Object::Ptr toJSON() const;

    };
}

#endif