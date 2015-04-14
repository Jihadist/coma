#ifndef CONFIG_H
#define CONFIG_H

// Обмен с модулями
#define Ok      0x11; // ответ "всё в порядке"
#define GBsi    0x21; // запрос блока стартовой информации
#define Gac     0x22; // запрос настроечных коэффициентов
#define Gda     0x23; // запрос текущих данных без настройки
#define GBd     0x24; // запрос блока текущих данных
#define GFc     0x25; // запрос файла конфигурации
#define GNosc   0x26; // запрос информации об осциллограммах
#define GBosc   0x27; // запрос осциллограммы
#define GBev    0x28; // запрос банка событий
#define Wac     0x31; // запись настроечных коэффициентов
#define WFc     0x32; // запись файла конфигурации
#define WFfw    0x33; // запись файла ВПО
#define LN      0x41; // зажечь светодиод
#define LF      0x42; // погасить светодиод
#define IP      0x43; // запрос IP-адреса
#define PS      0x44; // контроль 1PPS
#define SEGOK   0x55; // ответ "сегмент принят в порядке"
#define Err     0xF0; // ответ "ошибка"

#endif // CONFIG_H

