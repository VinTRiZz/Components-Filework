#include <string_view>

// AI-generated test
const std::string_view TEST_INI_DATA = R"(

; =============================================================================
; ТЕСТОВЫЙ INI-ФАЙЛ ДЛЯ ПРОВЕРКИ ПАРСЕРА
; Покрывает: комментарии, секции, ключи, значения, экранирование, Unicode,
; дубликаты, многострочность, граничные имена, спецсимволы и многое другое.
; =============================================================================

; 1. ГЛОБАЛЬНАЯ ОБЛАСТЬ (до первой секции)
; Комментарий с точкой с запятой
# Комментарий с решёткой (если поддерживается)

global_key_without_section = value_in_global_scope
empty_global_key =            ; комментарий после пустого значения
    leading_trailing_spaces   =    surrounded by spaces
; Значение с символом равенства внутри
equation_key = E=mc^2
; Значение, похожее на секцию
fake_section_value = [not a real section]

; 2. ПУСТЫЕ СТРОКИ И КОММЕНТАРИИ ВНУТРИ СЕКЦИИ

[normal_section]
; обычный ключ-значение
key1 = value1
key2 = value2

# ещё один стиль комментария
key_with_comment = something # это часть значения или комментарий?

; 3. СЕКЦИИ С РАЗНЫМИ ИМЕНАМИ

[ section with spaces ]          ; пробелы внутри скобок (стандарт не рекомендует, но многие парсеры допускают)
[section.with.dots]               ; точки в имени
[section-with-hyphens_and_underscores]
[]                                ; пустое имя секции
[unclosed_section                 ; нет закрывающей скобки - ошибочный случай
[section]with]extra]brackets]     ; вложенные скобки
[кириллическая_секция]           ; Unicode
[日本語セクション]
[section
with
newline]                          ; перевод строки внутри имени (маловероятно, что поддерживается, но для теста)

; 4. ДУБЛИКАТЫ СЕКЦИЙ
; Парсер может перезаписать предыдущую, объединить или выдать ошибку.

[duplicate_section]
first_key = first_value

[duplicate_section]
second_key = second_value

; 5. КЛЮЧИ И ЗНАЧЕНИЯ: ВСЕ ВОЗМОЖНЫЕ ФОРМАТЫ

[key_value_tests]
; Пустые ключи и значения
= empty_key              ; ключ отсутствует (ошибка или игнор)
empty_value =
  only_spaces_value =
; Пробелы вокруг знака равенства
normal = value
no_spaces=value
left_space =value
right_space= value
both =   value_with_spaces
; Ключи с пробелами внутри (редко поддерживаются, но есть)
key with spaces inside = works?
; Ключ без значения (только ключ)
bare_key_without_equals
; Несколько знаков равенства
multi_eq = key=value=another
; Ключи, совпадающие с зарезервированными словами (true/false/yes/no/on/off)
true = this_is_string_not_boolean
false = 0
yes = affirmative
no = negative
on = enabled
off = disabled

; 6. КАВЫЧКИ И ЭКРАНИРОВАНИЕ ЗНАЧЕНИЙ

[quoting_and_escaping]
; Двойные кавычки
double_quoted = "  value with leading and trailing spaces  "
; Одинарные кавычки
single_quoted = '  another spaced value  '
; Смешанные кавычки (парсер может по-разному интерпретировать)
mixed_quotes = "value with 'single' inside" and 'outside'
; Экранированные кавычки внутри значения (если парсер поддерживает экранирование)
escaped_double = "He said, \"Hello!\""
escaped_single = 'It\'s a test'
; Спецсимволы: обратный слеш, табуляция, перевод строки (зависит от парсера)
escape_chars = backslash\\ tab\t newline\n carriage\r
; Значение, содержащее точку с запятой (потенциальный комментарий)
semicolon_in_value = value;this is not a comment if quoted? "actually;still value"
hash_in_value = price#tag
; Значение, которое выглядит как ссылка на переменную (интерполяция)
template_value = %(variable)s or ${variable}

; 7. МНОГОСТРОЧНЫЕ ЗНАЧЕНИЯ
; Стандарт де-факто: последующие строки с отступом продолжают значение.

[multiline]
description = This is a very long value that spans
    multiple lines because it is indented
    with spaces or tabs.
another_multiline = First line
    Second line with tab indentation
mixed_indent = Line one
   Line two (tricky: spaces vs tabs)

; 8. РАЗНЫЕ ТИПЫ ДАННЫХ (как строки)

[data_types]
integer = 42
negative_integer = -17
float = 3.14159
bool_lower = true
bool_upper = TRUE
bool_camel = True
bool_yes = yes
bool_no = no
list_comma = value1,value2,value3
json_in_value = {"key": "value"}
xml_in_value = <tag attribute="value">text</tag>

; 9. ОЧЕНЬ ДЛИННЫЕ ИМЕНА И ЗНАЧЕНИЯ

[very_long_names_and_values]
this_is_an_extremely_long_key_name_that_exceeds_any_reasonable_limit_but_should_still_be_handled_gracefully_by_the_parser = short_val
short_key = This is an extremely long value that goes on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on and on

; 10. РАЗДЕЛ С UNICODE В КЛЮЧАХ И ЗНАЧЕНИЯХ

[unicode_content]
unicode_key_й = значение с кириллицей
emoji_value = 😀🎉💻
russian_section_test = Привет, мир!
japanese_key_日本語 = 漢字
right_to_left = قيمة عربية

; 11. СПЕЦИАЛЬНЫЕ СИМВОЛЫ В СЕКЦИЯХ И КЛЮЧАХ (если разрешены)

[special]chars]
key:with:colons = value1
key?with?question = value2
key*with*asterisk = value3
key/with/slash = value4
key\with\backslash = value5
[opensection = no_close

; 12. ПРОВЕРКА ОБРАБОТКИ BOM (если файл сохранён с UTF-8 BOM)
; Самый первый символ файла: \uFEFF — это не комментарий, а BOM.

; 13. ПУСТЫЕ СЕКЦИИ (без ключей)

[empty_section]

[another_empty_section]

; 14. СЕКЦИЯ С ТОЛЬКО КОММЕНТАРИЯМИ

[comments_only_section]
; здесь только комментарий
# и ещё один

; 15. СМЕШАННЫЕ ОКОНЧАНИЯ СТРОК (CRLF и LF) — тестируется самим файлом,
;     если его сохранить с \r\n в одних строках и \n в других.
;     В этом примере предполагаются стандартные LF.

; 16. СИМВОЛЫ КОММЕНТАРИЯ ВНУТРИ ЗНАЧЕНИЯ (требуют анализа)

[comment_in_value]
semicolon_inside = value1 ; this is actually part of the value?
hash_inside = value2 # same question
; Значение, начинающееся с символа комментария
starts_with_semicolon = ;not a comment if quoted?
starts_with_hash = #not a comment either?

; 17. ПРОВЕРКА ПРИОРИТЕТА ДУБЛИКАТОВ КЛЮЧЕЙ (последний перезаписывает)

[priority_test]
duplicate_key = first
duplicate_key = second
duplicate_key = third and final

; 18. ГЛУБОКАЯ ВЛОЖЕННОСТЬ? (INI обычно плоский, но можно имитировать точками)

[parent]
child.key = value
another.child.grandchild.key = deep value

; 19. ЗНАЧЕНИЯ, ЯВЛЯЮЩИЕСЯ ПУСТОЙ СТРОКОЙ ПОСЛЕ ОБРЕЗКИ ПРОБЕЛОВ

[trim_values]
should_be_empty =
should_be_empty_quoted = "   "
only_tabs =

; 20. КОММЕНТАРИИ В КОНЦЕ ФАЙЛА БЕЗ ПЕРЕВОДА СТРОКИ (EOF)
; Файл может заканчиваться строкой без \n.
last_key = last_value
; final comment without newline after it

)";