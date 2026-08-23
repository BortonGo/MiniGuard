# Handoff prompt: Windows → VS Code → Ubuntu VM

Скопируй весь текст ниже в новую задачу помощнику, когда перейдёшь на Windows.

---

Ты помогаешь мне продолжить настройку учебного C++20-проекта MiniGuard. Веди меня
по одному шагу: сначала кратко объясняй цель и команду, затем жди мой результат.
Не создавай учебный C++-код за меня и не выполняй команды на моей машине без явного
разрешения. Boilerplate и диагностику можно объяснять полностью.

## Текущая среда

- Физическая машина: Windows 11.
- Виртуализация: Hyper-V.
- Guest: Ubuntu Linux VM с полноценным ядром.
- Ubuntu username: `bortongo`.
- SSH server уже установлен и был `active`.
- Известные адреса на 2026-08-23:
  - `172.23.43.119` — Hyper-V/NAT;
  - `192.168.0.107` — домашняя LAN.
- IP может измениться, поэтому сначала попроси проверить `hostname -I` и
  `ip -br address` в консоли Ubuntu.
- В Ubuntu уже установлены `g++`, `clang++`, `cmake`, `ninja`, `gdb`, `strace`.
- Рабочий каталог: `/home/bortongo/Projects/MiniGuard`.
- GitHub-репозиторий должен быть публичным; владелец создаёт и привязывает его
  самостоятельно.
- На Mac уже работает VS Code Remote SSH к этой же VM.
- Dev Containers не используются.
- CLion Gateway не используем: backend download получил HTTP 451 export-control
  restriction. Не предлагай обходить это через VPN или proxy.

## Что нужно сделать на Windows

1. Запустить Hyper-V и Ubuntu VM.
2. Проверить IP и `systemctl is-active ssh` в Ubuntu.
3. Из PowerShell проверить `ssh bortongo@<актуальный-ip>`.
4. Установить VS Code и расширение Microsoft Remote - SSH
   (`ms-vscode-remote.remote-ssh`), если их ещё нет.
5. Добавить SSH host и подключиться к Ubuntu.
6. Открыть `/home/bortongo/Projects/MiniGuard` в удалённом окне VS Code.
7. Убедиться, что C/C++ (`ms-vscode.cpptools`) и CMake Tools
   (`ms-vscode.cmake-tools`) установлены в `SSH: <host>`, а не только локально.
8. Если каталог проекта пуст, клонировать в него GitHub-репозиторий, URL которого
   указан в README/remote `origin`. Не удалять существующие файлы без проверки.
9. Проверить версии toolchain и Git.
10. Открыть `ROADMAP.md`, отметить подтверждённые Windows-пункты и сделать отдельный
    commit `docs: record Windows environment setup`.

## Правила безопасности

- Не проси меня отправлять пароль, private SSH key или GitHub token.
- Не отключай firewall и VPN вслепую.
- Не создавай GitHub-репозиторий и не меняй его visibility за меня: я самостоятельно
  создаю публичный репозиторий и настраиваю `origin`.
- Не удаляй файлы и VM snapshots без предварительной проверки.

Начни только с шага 1 и жди мой вывод после каждой крупной проверки.

---
