#include "user.h"
#include "database.h"
#include "../config/config.h"

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

    User User::fromJSON(const std::string &str)
    {
        User user;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        user.id() = object->getValue<long>("id");
        user.first_name() = object->getValue<std::string>("first_name");
        user.last_name() = object->getValue<std::string>("last_name");
        user.addres() = object->getValue<std::string>("addres");
        user.login() = object->getValue<std::string>("login");
        user.password() = object->getValue<std::string>("password");

        return user;
    }

    std::optional<long> User::auth(std::string &login, std::string &password)
    {
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
                if (rs.moveFirst()) return id;
            }
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
        }
        return {};
    }

    std::optional<User> User::read_by_id(long id)
    {
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
            if (rs.moveFirst()) return a;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            
        }
        return {};
    }

    std::optional<User> User::read_by_login(std::string &login)
    {
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
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            
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
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            std::vector<User> result;
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