#include "user.h"
#include "database.h"
#include "../config/config.h"
#include "cache.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{

    void User::init()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();

            std::vector<std::string> shards = database::Database::get_all_hints();
            for (const std::string &shard: shards) {
                Statement create_stmt(session);
                create_stmt << "CREATE TABLE IF NOT EXISTS `User` (`id` INT NOT NULL AUTO_INCREMENT,"
                            << "`first_name` VARCHAR(256) NOT NULL,"
                            << "`last_name` VARCHAR(256) NOT NULL,"
                            << "`login` VARCHAR(256) UNIQUE NOT NULL,"
                            << "`password` VARCHAR(256) NOT NULL,"
                            << "`addres` VARCHAR(256) NULL,"
                            << "PRIMARY KEY (`id`),KEY `fn` (`first_name`),KEY `ln` (`last_name`));"
                            << shard,
                    now;
                std::cout << create_stmt.toString() << std::endl;
            }
            Statement create_seq_stmt(session);
            create_seq_stmt << "create sequence if not exists user_id_sequence;" << database::Database::squence_sharding_hint(), now;
            std::cout << create_seq_stmt.toString() << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr User::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("first_name", _first_name);
        root->set("last_name", _last_name);
        root->set("addres", _addres);
        root->set("login", _login);
        root->set("password", _password);

        return root;
    }

    User User::from_json_object(Poco::JSON::Object::Ptr object)
    {
        User user;

        user.id() = object->getValue<long>("id");
        user.first_name() = object->getValue<std::string>("first_name");
        user.last_name() = object->getValue<std::string>("last_name");
        user.addres() = object->getValue<std::string>("addres");
        user.login() = object->getValue<std::string>("login");
        user.password() = object->getValue<std::string>("password");

        return user;
    }

    User User::fromJson(const std::string &str) {
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
        return from_json_object(object);
    }

    void User::save_to_cache() {
        try {
            std::stringstream ss;
            Poco::JSON::Stringifier::stringify(toJSON(), ss);
            std::string obj = ss.str();
            database::Cache::get().put(std::to_string(_id), obj);
            database::Cache::get().put(_login, obj);
        } catch (std::exception &e) {
            std::cerr << "Failed to save user to cache: " << e.what() << std::endl;
        }
    }

    User User::get_from_cache_by_id(long id) {
        try {
            std::string result = "";
            std::cout << "Trying to get from cache by id" << std::endl;
            if (database::Cache::get().get(std::to_string(id), result))
                {
                    std::cout << result << std::endl;
                    return fromJson(result);
                }
            std::cout << "empty" << std::endl;
            return empty();
        } catch (std::exception &e) {
            std::cerr << "Failed to get user from cache by id: " << e.what() << std::endl;
            return empty();
        }
    }

    User User::get_from_cache_by_login(std::string login) {
        try {
            std::string result = "";
            std::cout << "Trying to get from cache by login" << std::endl;
            if (database::Cache::get().get(login, result))
                {
                    std::cout << result << std::endl;
                    return fromJson(result);
                }
            std::cout << "empty" << std::endl;
            return empty();
        } catch (std::exception &e) {
            std::cerr << "Failed to get user from cache by login: " << e.what() << std::endl;
            return empty();
        }
    }

    std::optional<long> User::auth(std::string &login, std::string &password)
    {

        User user = get_from_cache_by_login(login);
        if (user.get_id() > 0) {
            std::cout << "Got user from cache!" << std::endl;
            if (user.get_password() == password) {
                return user.get_id();
            }
            std::cout << "Failed to auth user by cache, trying db!" << std::endl;
        }

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            long id;
            std::vector<std::string> shards = database::Database::get_all_hints();
            for (const std::string &shard: shards) {
                Poco::Data::Statement select(session);
                select << "SELECT id FROM User where login=? and password=?"
                       << shard,
                    into(id),
                    use(login),
                    use(password),
                    range(0, 1); //  iterate over result set one row at a time

                select.execute();
                Poco::Data::RecordSet rs(select);
                if (rs.moveFirst()) {
                    if (database::Cache::get().is_cache_enabled()) {
                        User user = read_by_id(id).value();
                        user.save_to_cache();
                    }
                    return id;
                }
            }
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cerr << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cerr << "statement:" << e.what() << std::endl;
        }
        return {};
    }

    std::optional<User> User::read_by_id(long id)
    {

        User cache_user = get_from_cache_by_id(id);
        if (cache_user.get_id() > 0) {
            std::cout << "Got user from cache!" << std::endl;
            return cache_user;
        }

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            User a;
            select << "SELECT id, first_name, last_name, addres,login,password FROM User where id=?"
                   << database::Database::sharding_hint(id),
                into(a._id),
                into(a._first_name),
                into(a._last_name),
                into(a._addres),
                into(a._login),
                into(a._password),
                use(id),
                range(0, 1); //  iterate over result set one row at a time
            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst()) {
                a.save_to_cache();
                return a;
            }
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cerr << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cerr << "statement:" << e.what() << std::endl;
            
        }
        return {};
    }

    std::optional<User> User::read_by_login(std::string &login)
    {

        User user = get_from_cache_by_login(login);
        if (user.get_id() > 0) {
            std::cout << "Got user from cache!" << std::endl;
            return std::optional(user);
            std::cout << "Failed to auth user by cache, trying db!" << std::endl;
        }

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            User a;
            std::vector<std::string> shards = database::Database::get_all_hints();
            for (const std::string &shard: shards) {
                Poco::Data::Statement select(session);
                select << "SELECT id, first_name, last_name, addres FROM User where login=?"
                       << shard,
                    into(a._id),
                    into(a._first_name),
                    into(a._last_name),
                    into(a._addres),
                    use(login),
                    range(0, 1); //  iterate over result set one row at a time
                a._login = login;
                a._password = "*******";

                select.execute();
                Poco::Data::RecordSet rs(select);
                if (rs.moveFirst()) return a;
            }
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cerr << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cerr << "statement:" << e.what() << std::endl;
            
        }
        return {};
    }

    std::vector<User> User::read_all()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            std::vector<User> result;
            std::vector<std::string> shards = database::Database::get_all_hints();
            for (const std::string &shard: shards) {
                User a;
                Poco::Data::Statement select(session);
                select << "SELECT id, first_name, last_name, addres, login, password FROM User"
                       << shard,
                    into(a._id),
                    into(a._first_name),
                    into(a._last_name),
                    into(a._addres),
                    into(a._login),
                    into(a._password),
                    range(0, 1); //  iterate over result set one row at a time

                while (!select.done())
                {
                    if (select.execute())
                        result.push_back(a);
                }
            }
            return result;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    std::vector<User> User::search(std::string first_name, std::string last_name)
    {
        std::vector<User> result;
        User user = empty();
        user._first_name = first_name;
        user._last_name = last_name;
        if (search_by_cache(user, result)) {
            std::cout << "Get search from cache!" << std::endl;
            return result;
        }

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            first_name += "%";
            last_name += "%";
            std::vector<std::string> shards = database::Database::get_all_hints();
            for (const std::string &shard: shards) {
                User a;
                Poco::Data::Statement select(session);
                select << "SELECT id, first_name, last_name, addres, login, password FROM User where first_name LIKE ? and last_name LIKE ?"
                       << shard,
                    into(a._id),
                    into(a._first_name),
                    into(a._last_name),
                    into(a._addres),
                    into(a._login),
                    into(a._password),
                    use(first_name),
                    use(last_name),
                    range(0, 1); //  iterate over result set one row at a time

                while (!select.done())
                {
                    if (select.execute())
                        result.push_back(a);
                }
            }
            save_search_to_cache(user, result);
            return result;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    void User::save_to_mysql()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);
            _id = database::Database::generate_new_id();
            std::cout << "saved to " << database::Database::sharding_hint(_id) << "\n";
            insert << "INSERT INTO User (id,first_name,last_name,addres,login,password) VALUES(?, ?, ?, ?, ?, ?)"
                << database::Database::sharding_hint(_id),
                use(_id),
                use(_first_name),
                use(_last_name),
                use(_addres),
                use(_login),
                use(_password);

            insert.execute();
            save_to_cache();
            std::cout << "inserted:" << _id << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    std::string User::build_search_cache_key(User &like_user) {
        std::string key;
        if (!like_user.get_login().empty()) {
            key.append(like_user.get_login());
        }
        if (!like_user.get_first_name().empty() && !like_user.get_last_name().empty()) {
            key.append(like_user.get_first_name() + "-" + like_user.get_last_name());
        }
        if (key.empty()) {
            key = "full";
        }
        return key;
    }

    bool User::search_by_cache(User &like_user, std::vector<User> &search_results) {
        // Формируем ключ для поиска
        std::string key = build_search_cache_key(like_user);
        try {
            std::cout << "Getting search results by key " << key << std::endl;
            std::string result_cache;
            if (database::Cache::get().get(key, result_cache)) {
                if (result_cache.empty()) {
                    return false;
                }
                Poco::JSON::Parser parser;
                Poco::Dynamic::Var result = parser.parse(result_cache);
                Poco::JSON::Array::Ptr array = result.extract<Poco::JSON::Array::Ptr>();
                for (int i = 0; i < (int) array->size(); i++) {
                    User user = from_json_object(array->getObject(i));
                    search_results.push_back(user);
                }
                return true;
            } else {
                return false;
            }
        } catch (std::exception &e) {
            std::cerr << "Failed to get search results from cache: " << e.what() << std::endl;
            return false;
        }
    }

    void User::save_search_to_cache(User &like_user, std::vector<User> &search_results) {
        std::string key = build_search_cache_key(like_user);

        try {
            Poco::JSON::Array arr;
            for (database::User user: search_results) {
                arr.add(user.toJSON());
            }
            std::stringstream ss;
            Poco::JSON::Stringifier::stringify(arr, ss);
            std::string result = ss.str();
            std::cout << "Saving search results by key " << key << std::endl;
            database::Cache::get().put(key, result);
        } catch (std::exception &e) {
            std::cerr << "Failed to save search results to cache: " << e.what() << std::endl;
        }
    }

    const std::string &User::get_login() const
    {
        return _login;
    }

    const std::string &User::get_password() const
    {
        return _password;
    }

    std::string &User::login()
    {
        return _login;
    }

    std::string &User::password()
    {
        return _password;
    }

    long User::get_id() const
    {
        return _id;
    }

    const std::string &User::get_first_name() const
    {
        return _first_name;
    }

    const std::string &User::get_last_name() const
    {
        return _last_name;
    }

    const std::string &User::get_addres() const
    {
        return _addres;
    }

    long &User::id()
    {
        return _id;
    }

    std::string &User::first_name()
    {
        return _first_name;
    }

    std::string &User::last_name()
    {
        return _last_name;
    }

    std::string &User::addres()
    {
        return _addres;
    }
}