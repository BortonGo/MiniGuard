# MiniGuard Roadmap

Этот файл — единая точка состояния проекта для работы с Mac и Windows.

- `[x]` — выполнено и проверено;
- `[ ]` — ещё не выполнено;
- после каждой рабочей сессии обновляется блок **Журнал прогресса**.

Последнее обновление: **2026-08-23**.

## Текущий фокус

**Этап 0: открыть GitHub-копию MiniGuard в Ubuntu через Windows VS Code, проверить
remote C++ extensions и закончить toolchain smoke test.** После этого собрать
существующий минимальный CMake-проект или создать его самостоятельно, если старый
проект не найдётся.

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
- [ ] Создан минимальный C++20-проект через CMake.
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

- [ ] Process, PID, UID/GID.
- [ ] File descriptor и таблица открытых файлов процесса.
- [ ] Syscall, переход userspace → kernel space, errno.
- [ ] `/proc`, `/sys` и virtual filesystem.
- [ ] inode, permissions и signals.
- [ ] Упражнение с `open/read/close`.
- [ ] `getpid()` и исследование `/proc/<pid>`.
- [ ] Сравнение POSIX I/O и `std::ifstream`.
- [ ] Разбор syscalls программы через strace.

## Этап 2 — File Monitor

- [ ] Наблюдение событий через fanotify без блокировки.
- [ ] `fanotify_init`, `fanotify_mark`, event queue.
- [ ] Получение PID/UID/path события.
- [ ] File descriptors, blocking/non-blocking I/O, VFS и mount points.
- [ ] Проверка поведения на обычной Linux filesystem.

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
