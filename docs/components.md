# Компонентная архитектура
<!-- Состав и взаимосвязи компонентов системы между собой и внешними системами с указанием протоколов, ключевые технологии, используемые для реализации компонентов.
Диаграмма контейнеров C4 и текстовое описание. 
-->
## Компонентная диаграмма

```plantuml
@startuml
!include https://raw.githubusercontent.com/plantuml-stdlib/C4-PlantUML/master/C4_Container.puml

!define DEVICONS https://raw.githubusercontent.com/tupadr3/plantuml-icon-font-sprites/master/devicons
!define DEVICONS2 https://raw.githubusercontent.com/tupadr3/plantuml-icon-font-sprites/master/devicons2
!define FONTAWESOME https://raw.githubusercontent.com/tupadr3/plantuml-icon-font-sprites/master/font-awesome-5

!include DEVICONS2/cplusplus.puml
!include DEVICONS/mysql.puml
!include DEVICONS/react.puml
!include FONTAWESOME/users.puml

AddElementTag("microService", $shape=EightSidedShape(), $bgColor="CornflowerBlue", $fontColor="white", $legendText="microservice")
AddElementTag("storage", $shape=RoundedBoxShape(), $bgColor="lightSkyBlue", $fontColor="white")

Person(sender, "Отправитель", $sprite="users")
Person(courier, "Курьер", $sprite="users")
Person(receiver, "Получатель", $sprite="users")
Person(admin, "Администратор", $sprite="users")

System_Ext(web_site, "Клиентский веб-сайт", "HTML, CSS, JavaScript, React", "Веб-интерфейс", $sprite="react")

System_Boundary(site, "Доставка посылок") {
   Container(user_service, "Сервис работы с пользователями, авиризация", "C++", "Сервис авторизации и действий над пользователями", $tags = "microService", $sprite="cplusplus")
   Container(delivery_service, "Сервис доставкок", "C++", "Сервис для создания, редактирования, просмотра доставок", $tags = "microService", $sprite="cplusplus")
   Container(package_service, "Сервис посылкок", "C++", "Сервис для создания, редактирования, просмотра посылок", $tags = "microService", $sprite="cplusplus")
   Container(event_listener, "Сервис слушатель событий", "C++", "Сервис для прослушивания событий на сохранение пользователей", $tags = "microService", $sprite="cplusplus")

   Container(proxysql, "proxysql", "Proxy")
   ContainerDb(db, "db node 01", "MySQL", "Хранение данных о пользователях, посылках и доставках", $tags = "storage", $sprite="mysql")
   ContainerDb(db2, "db node 02", "MySQL", "Хранение данных о пользователях", $tags = "storage", $sprite="mysql")
   ContainerDb(db3, "db node 03", "MySQL", "Хранение данных о последовательностях", $tags = "storage", $sprite="mysql")

   ContainerDb(redis, "Redis", "Redis", "Хранение данных о пользователях", $tags = "storage", $sprite="redis")

   SystemQueue(kafka, "Kafka", "Пользователи для сохранения/обновления")
}

Rel(sender, web_site, "Создание, просмотр, отмена доставки посылки")
Rel(receiver, web_site, "Получение информации по доставкам по получателю")
Rel_R(courier, web_site, "Получение, обновление информации по доставкам")
Rel_L(admin, web_site, "Получение/обновление/удаление информации по отправителям/курьерам/доставкам")

Rel(web_site, user_service, "Авторизация, регистрация, поиск", "localhost/auth")
Rel(user_service, proxysql, "INSERT/SELECT/UPDATE", "SQL")
Rel(user_service, redis, "save/select")

Rel(web_site, delivery_service, "Работа с доставками", "localhost/delivery")
Rel(delivery_service, proxysql, "INSERT/SELECT/UPDATE", "SQL")

Rel(web_site, package_service, "Работа с посылками", "localhost/package")
Rel(package_service, proxysql, "INSERT/SELECT/UPDATE", "SQL")

Rel(proxysql, db, "INSERT/SELECT/UPDATE")
Rel(proxysql, db2, "INSERT/SELECT/UPDATE")
Rel(proxysql, db3, "SELECT")

Rel(delivery_service, user_service, "Авторизация")
Rel(package_service, user_service, "Авторизация")

Rel(user_service, kafka, "Отправляет пользователей на сохранение/обновление")
Rel(kafka, event_listener, "Потребляет пользователей на сохранение/обновление")
Rel(event_listener, proxysql, "INSERT/SELECT/UPDATE", "SQL")

@enduml
```
## Список компонентов  

### Сервис регистрации, авторизации
**REST API**:
-	Создание нового пользователя
      - входные параметры: login, пароль, имя, фамилия, email
      - выходные параметры: **токен**
-	Поиск пользователя по логину
     - входные параметры:  login
     - выходные параметры: **имя, фамилия, email**
-	Поиск пользователя по маске имени и фамилии
     - входные параметры: маска фамилии, маска имени
     - выходные параметры: **login, имя, фамилия, email**
-   Авторизация пользователя
     - входные параметры: логин, пароль
     - выходные параметры: **токен**

### Сервис доставок
**REST API**:
- Создание доставки
  - Входные параметры: идентификатор отправителя, идентификатор получателя, идентификатор посылки, дата/время доставки, адрес доставки, стоимость доставки, комментарий получателя
  - Выходыне параметры: идентификатор доставки
- Получение информации по доставке
  - Входные параметры: идентификатор доставки
  - Выходыне параметры: статус заявки, идентификатор отправителя, идентификатор получателя, идентификатор посылки, дата/время доставки, адрес доставки, стоимость доставки, комментарий получателя
- Получение всех активных доставок по отправителю
  - Входные параметры: идентификатор отправителя
  - Выходыне параметры: список доставок
- Получение всех активных доставок по получателю
  - Входные параметры: идентификатор получателя
  - Выходыне параметры: список доставок
- Получение всех активных доставок по курьеру
  - Входные параметры: идентификатор курьера
  - Выходыне параметры: список доставок
- Обновление статуса доставки
  - Входные параметры: идентификатор доставки, статус
  - Выходыне параметры: отсутствуют
- Обновление информации по доставке
  - Входные параметры: идентификатор доставки, статус заявки, идентификатор отправителя, идентификатор получателя, идентификатор посылки, дата/время доставки, адрес доставки, стоимость доставки, комментарий получателя
  - Выходыне параметры: отсутствуют
- Удаление доставки
  - Входные параметры: идентификатор доставки
  - Выходыне параметры: отсутствуют

### Сервис посылок
**REST API**:
- Создание посылки
  - Входные параметры: название, описание, характеристики, стоимость, адрес на изображение посылки
  - Выходыне параметры: идентификатор посылки
- Получение информации по посылке
  - Входные параметры: идентификатор посылки
  - Выходыне параметры: название, описание, характеристики, стоимость, адрес на изображение посылки
- Обновление информации по посылке
  - Входные параметры: идентификатор посылки, название, описание, характеристики, стоимость, адрес на изображение посылки
  - Выходыне параметры: отсутствуют
- Удаление посылки
  - Входные параметры: идентификатор посылки
  - Выходыне параметры: отсутствуют
