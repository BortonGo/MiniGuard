# MiniGuard Roadmap

Этот файл — единая точка состояния проекта для работы с Mac и Windows.

- `[x]` — выполнено и проверено;
- `[ ]` — ещё не выполнено;
- после каждой рабочей сессии обновляется блок **Журнал прогресса**.

Последнее обновление: **2026-08-26**.

## Текущий фокус

**Переход от Linux fundamentals к этапу 2 (File Monitor): проверить в Ubuntu уже
написанные программы и разобрать `fanotify_init()`.** В начале следующей сессии
нужно выполнить `git pull --ff-only`, убедиться в чистом `git status`, собрать оба
target с GCC через Ninja и запустить существующий `miniguard-monitor` без `sudo`.
До проверки результата `fanotify_init()` и понимания его флагов не добавлять
`fanotify_mark()` и event loop.

## Этап 0 — окружение и workflow

### Ubuntu VM

- [x] Ubuntu VM создана в Hyper-V на Windows.
- [x] Ubuntu имеет полноценное Linux-ядро.
- [x] SSH server запущен.
- [x] VM доступна из домашней сети по SSH.
- [x] Установлены GCC, Clang, CMake, Ninja, GDB и strace (подтверждено владельцем).
- [ ] Проверены Git, ltrace, Valgrind, pkg-config, linux-tools и kernel headers.
- [ ] Настроен snapshot чистого рабочего окружения в Hyper-V.

### macOS

- [x] Терминальный SSH с Mac в Ubuntu работает.
- [x] VS Code Remote SSH подключён к Ubuntu.
- [x] В удалённой среде установлены расширения C/C++ и CMake Tools.
- [x] Каталог `/home/bortongo/Projects/MiniGuard` открывается удалённо.
- [ ] Настроен вход по SSH-ключу вместо постоянного ввода пароля.

### Windows

- [ ] Проверено подключение `ssh` из PowerShell в Ubuntu.
- [x] Установлено расширение VS Code Remote SSH.
- [x] VS Code подключён к Ubuntu.
- [ ] В VS Code открыт `/home/bortongo/Projects/MiniGuard`.
- [ ] Проверены C/C++ и CMake Tools в удалённой среде.

### Git и минимальная сборка

- [x] Владельцем создан публичный GitHub-репозиторий `MiniGuard`.
- [x] Идея проекта и учебный контракт сохранены в `PROJECT_CONTEXT.md`.
- [ ] Репозиторий клонирован в `/home/bortongo/Projects/MiniGuard` в Ubuntu.
- [ ] Настроены Git username/email в Ubuntu.
- [x] Создан минимальный C++20-проект через CMake.
- [ ] Debug-сборка выполнена с GCC через Ninja.
- [ ] Debug-сборка выполнена с Clang через Ninja.
- [ ] Программа запущена под GDB.
- [ ] Программа запущена через strace; разобраны несколько syscall.
- [ ] Проверен одинаковый workflow из VS Code на Mac и Windows.

### Что нужно понимать после этапа 0

- [ ] Compiler, linker и build system — разные части сборки.
- [ ] CMake генерирует build configuration, но сам не компилирует C++.
- [ ] Ninja/Make исполняют граф команд сборки.
- [ ] Debug и Release отличаются оптимизациями и отладочной информацией.
- [ ] VS Code работает локально, а расширения, terminal и toolchain — в Ubuntu.

## Этап 1 — Linux fundamentals

- [x] В программе реализован вывод PID, real/effective UID и GID.
- [ ] Проверено понимание process, PID, UID/GID и различия real/effective IDs.
- [x] Пройден ручной lifecycle file descriptor; затем добавлен простой RAII
  wrapper с запрещённым копированием.
- [ ] Разобраны таблица file descriptors процесса, open file description и
  совместное владение ресурсом.
- [x] Реализовано упражнение с `open/read/close`, обработкой EOF, `EINTR` и
  сохранением `errno`.
- [ ] Разобран переход userspace → libc wrapper → syscall → kernel → VFS.
- [x] Через `fstat()` выводятся тип файла, inode, размер, UID/GID владельца и
  permissions.
- [ ] Разобраны `/proc`, `/sys`, virtual filesystem и signals.
- [ ] Исследован `/proc/<pid>` для процесса `miniguard`.
- [ ] Выполнено сравнение POSIX I/O и `std::ifstream`.
- [ ] Выполнен и разобран запуск программы через strace.

## Этап 2 — File Monitor

- [x] Добавлен отдельный executable `miniguard-monitor`.
- [x] Реализован первый вызов `fanotify_init()` с `FAN_CLASS_NOTIF`,
  `FAN_CLOEXEC` и RAII-владением queue fd.
- [ ] Проверены сборка и результат `fanotify_init()` в Ubuntu без `sudo`; разобраны
  возможные `EPERM`/`EINVAL` и ограничения конкретной версии ядра.
- [ ] Разобраны два разных close-on-exec флага: для fanotify fd и event fd.
- [ ] Добавлен `fanotify_mark()` только для отдельного безопасного тестового
  каталога.
- [ ] Реализовано наблюдение событий без permission-блокировки.
- [ ] Реализованы чтение и корректный разбор всех записей event queue.
- [ ] Получены PID/UID/path события; event file descriptors корректно закрываются.
- [ ] Разобраны blocking/non-blocking I/O, VFS, inode marks и mount points.
- [ ] Проверено поведение на обычной Linux filesystem.

## Этап 3 — Execution monitoring

- [ ] Наблюдение запуска executable-файлов.
- [ ] `fork`, `execve` и различие process/executable.
- [ ] Вывод PID, executable path и UID.

## Этап 4 — Scanner

- [ ] Безопасный scanner без настоящего malware.
- [ ] Hash, magic bytes, размер и executable permissions.
- [ ] ELF/non-ELF и простые эвристики.
- [ ] Безопасный тест на искусственных правилах или EICAR.

## Этап 5 — Rule Engine

- [ ] `Rule`, `RuleEngine`, `ScanResult`, `Verdict`.
- [ ] Правила отделены от scanner и не размазаны по `if`.
- [ ] Unit tests для rule engine.
- [ ] Разбор separation of concerns и dependency inversion.

## Этап 6 — ELF Analyzer

- [ ] Самостоятельный разбор ELF64 header.
- [ ] Program headers и section headers.
- [ ] `.text`, `.data`, `.bss`, `.rodata`, `.dynsym`, `.dynstr`.
- [ ] Entry point, architecture, PIE, NX и dynamic linking.
- [ ] `open`, `fstat`, `mmap`, `munmap` и сравнение `mmap/read`.
- [ ] CLI `miniguard inspect <file>`.

## Этап 7 — Process Monitor

- [ ] Парсинг `/proc/PID/status`, `cmdline`, `exe`, `maps`, `fd`.
- [ ] PID, executable, UID, память, threads и open file descriptors.
- [ ] Virtual address space, stack, heap, shared libraries и mappings.

## Этап 8 — Multithreading

- [ ] Сначала корректная однопоточная версия.
- [ ] Producer/consumer и bounded blocking queue.
- [ ] `std::thread`, mutex, condition_variable и atomics.
- [ ] Graceful shutdown, race condition и data race.

## Этап 9 — Quarantine

- [ ] Безопасный quarantine directory и metadata.
- [ ] Original path, hash, timestamp и rule.
- [ ] Permissions, ownership, TOCTOU и atomic rename.

## Этап 10 — CLI и daemon

- [ ] Разделены `miniguard` и `miniguardd`.
- [ ] IPC через Unix Domain Socket.
- [ ] Client/server, blocking I/O и protocol framing.

## Этап 11 — kernel module

- [ ] Минимальные `module_init` и `module_exit`.
- [ ] `pr_info`, `dmesg`, `insmod`, `rmmod`, `modprobe`.
- [ ] Character device `/dev/miniguard`.
- [ ] major/minor, `file_operations`, read/write/ioctl.
- [ ] `copy_to_user` и `copy_from_user`.
- [ ] Без hooking, скрытия процессов и rootkit-like техник.

## Этап 12 — debugging

- [ ] GDB применяется на протяжении проекта.
- [ ] strace/ltrace для системных и библиотечных вызовов.
- [ ] Valgrind, AddressSanitizer и UndefinedBehaviorSanitizer.
- [ ] Для каждого инструмента понятно, что он видит и когда применяется.

## Этап 13 — performance

- [ ] Измерены latency, CPU usage и events/second.
- [ ] Использованы `time`, perf и benchmark.
- [ ] Сначала correctness, затем measurement, затем optimization.

## Сквозные задачи

- [ ] Тесты добавляются вместе с компонентами.
- [ ] Поддерживается `docs/linux-notes.md`.
- [ ] Поддерживается `docs/interview-notes.md`.
- [ ] После каждого крупного этапа проводится mock interview.
- [ ] Каждый логический шаг оформляется отдельным commit.

## Журнал прогресса

### 2026-08-26

- Добавлен отдельный `miniguard-monitor`.
- Написан минимальный вызов `fanotify_init()` в notification-режиме; полученный
  file descriptor передаётся существующему RAII wrapper.
- В CMake добавлен второй executable target.
- Состояние `main` синхронизировано с `origin/main` на коммите `077eff1`.
- Roadmap приведён в соответствие с кодом; runtime-проверки намеренно не отмечены
  выполненными без повторной проверки в Ubuntu.

### 2026-08-25

- Создан минимальный C++20/CMake target `miniguard`.
- Реализовано POSIX-чтение файла через `open()`/`read()`/`close()` с обработкой
  ошибок и `EINTR`.
- Ручное закрытие file descriptor заменено небольшим RAII wrapper.
- Добавлен вывод метаданных через `fstat()`: тип, inode, размер, владелец и
  permissions; неподдерживаемые типы файла отклоняются до чтения.
- Добавлен вывод PID, real/effective UID и GID процесса.
- Изменения разделены на четыре небольших тематических коммита.

### 2026-08-23

- Выбрана полноценная Ubuntu VM вместо Dev Containers.
- Подтверждена доступность VM с Mac по SSH.
- Настроен VS Code Remote SSH на Mac.
- Установлены удалённые расширения C/C++ и CMake Tools.
- CLion Gateway отклонён: Linux-backend не скачивается из-за HTTP 451 export
  control restriction; обход ограничений не используется.
- Создана документационная основа и roadmap для продолжения на двух компьютерах.
- Создан публичный GitHub-репозиторий `BortonGo/MiniGuard`.
- VS Code на Windows подключён к Ubuntu VM через Remote SSH.
- Исходный промт преобразован в постоянный `PROJECT_CONTEXT.md`; добавлен
  `AGENTS.md` для будущих coding-agent сессий.
