#include <iostream>
#include <csignal>
#include <cppkafka/consumer.h>
#include <cppkafka/configuration.h>
#include "config/config.h"

#include "./database/user.h"

bool working = true;

bool check_name(const std::string &name)
{
    if (name.length() < 3)
    {
        std::cerr << "Name must be at leas 3 signs" << std::endl;
        return false;
    }

    if (name.find(' ') != std::string::npos)
    {
        std::cerr << "Name can't contain spaces" << std::endl;
        return false;
    }

    if (name.find('\t') != std::string::npos)
    {
        std::cerr << "Name can't contain spaces" << std::endl;
        return false;
    }

    return true;
};

int main() {
    try {
        signal(SIGINT, [](int) { working = false; });

        cppkafka::Configuration config = {
            {"metadata.broker.list", Config::get().get_queue_host()},
            {"group.id", Config::get().get_queue_group_id()},
            {"enable.auto.commit", false}};

        cppkafka::Consumer consumer(config);
        consumer.set_assignment_callback([](const cppkafka::TopicPartitionList &partitions)
                                         { std::cout << "Got assigned: " << partitions << std::endl; });
        consumer.set_revocation_callback([](const cppkafka::TopicPartitionList &partitions)
                                         { std::cout << "Got revoked: " << partitions << std::endl; });
        consumer.subscribe({Config::get().get_queue_topic()});

        std::cout << "[QUEUE] " << "Consuming messages from topic " << Config::get().get_queue_topic() << std::endl;

        while (working) {
            cppkafka::Message msg = consumer.poll();
            if (msg) {
                if (msg.get_error()) {
                    if (!msg.is_eof()) {
                        std::cout << "[QUEUE] " << "Received error notification: " << msg.get_error() << std::endl;
                    }
                } else {
                    if (msg.get_key()) {
                        std::cout << msg.get_key() << " -> ";
                    }
                    std::string payload = msg.get_payload();
                    std::cout << "[QUEUE] " << msg.get_payload() << std::endl;
                    database::User user = database::User::fromJson(payload);

                    std::cout << user.login() << std::endl;
                    std::optional<database::User> result = database::User::read_by_login(user.login());
                    if (!result && check_name(user.get_first_name()) && check_name(user.get_last_name()))
                    {
                        user.save_to_mysql();
                    }

                    consumer.commit(msg);
                }
            }
        }
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 1;
}
