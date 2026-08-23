# Development environment

## Принятая схема

MiniGuard собирается, запускается и отлаживается в полноценной Ubuntu VM. Mac и
Windows подключаются к одной VM через VS Code Remote SSH.

```text
Mac ───────┐
           ├─ SSH ─→ Ubuntu VM в Hyper-V ─→ build/run/debug
Windows ───┘
```

Это даёт настоящее Linux-ядро для fanotify, procfs, perf и будущего kernel module.
Dev Containers в текущем workflow не используются.

## Текущее подключение

```text
Ubuntu user: bortongo
SSH port:    22
Project:     /home/bortongo/Projects/MiniGuard
```

Во время первичной настройки VM имела два IPv4-адреса:

```text
eth0: 172.23.43.119/20   # сеть Hyper-V/NAT
eth1: 192.168.0.107/24   # домашняя LAN, используется с Mac
```

Адреса выдаются сетью и могут измениться после перезапуска. Актуальные адреса нужно
проверять в консоли Ubuntu:

```bash
hostname -I
ip -br address
```

Состояние SSH:

```bash
systemctl is-active ssh
```

Подключение:

```bash
ssh bortongo@192.168.0.107
```

Пароль, приватные ключи и токены нельзя сохранять в этом репозитории.

## VS Code

На каждом клиентском компьютере устанавливается локально:

- VS Code;
- Remote - SSH (`ms-vscode-remote.remote-ssh`).

После подключения к Ubuntu на стороне `SSH: <host>` должны быть установлены:

- C/C++ (`ms-vscode.cpptools`);
- CMake Tools (`ms-vscode.cmake-tools`).

Расширения помогают IDE использовать инструменты, но не заменяют compiler, CMake
или debugger. Настоящий toolchain установлен в Ubuntu.

## Ежедневный workflow

```bash
cd ~/Projects/MiniGuard
git pull --ff-only
# работа, сборка, тесты
git status
git add <конкретные-файлы>
git commit -m "type: concise change"
git push
```

Не использовать `git add .` без просмотра `git status`. Build directories и
артефакты не должны попадать в Git.

## Ограничения одной VM

- Для подключения с Mac Windows-компьютер и Ubuntu VM должны быть включены.
- Mac и Windows должны видеть друг друга по локальной сети либо через разрешённое
  безопасное удалённое подключение.
- VPN может менять маршруты к локальной сети; при проблеме сначала сравнить обычный
  `ssh` и `ip route`, а не менять настройки VM вслепую.
- Если позже появится отдельная VM на Mac, GitHub станет механизмом синхронизации
  двух независимых Linux-копий проекта.
