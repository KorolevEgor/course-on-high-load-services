INSERT INTO `User`
(id, first_name, last_name, login, password, addres)
VALUES(1, 'Ivan', 'Ivanov', 'iviv', '123', 'abc def 123');

INSERT INTO `User`
(id, first_name, last_name, login, password, addres)
VALUES(2, 'Petr', 'Petrov', 'pepe', 'pass', 'qwe rty 456');

-------------

INSERT INTO `Delivery`
(id, recipient_name, sender_name, recipient_addres, sender_addres, date, state)
VALUES(1, 'iviv', 'pepe', 'a', 'b', '01-10-2023', 'new');

INSERT INTO `Package`
(id, name, weight, price, delivery_id)
VALUES(1, 'd 1', '10.25', '1000', 1);

INSERT INTO `Package`
(id, name, weight, price, delivery_id)
VALUES(2, 'd 2', '1.00', '10000', 1);

INSERT INTO `Package`
(id, name, weight, price, delivery_id)
VALUES(3, 'd 3', '1.00', '10000', 1);

-------------

INSERT INTO `Delivery`
(id, recipient_name, sender_name, recipient_addres, sender_addres, date, state)
VALUES(2, 'pepe', 'iviv', 'b', 'a', '01-11-2023', 'new');

INSERT INTO `Package`
(id, name, weight, price, delivery_id)
VALUES(4, 'd 4', '10.25', '1000', 2);

INSERT INTO `Package`
(id, name, weight, price, delivery_id)
VALUES(5, 'd 5', '1.00', '10000', 2);

