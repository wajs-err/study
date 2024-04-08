# Pequod

Погружает рабочую копию репозитория, сабмодулем которого является, в докер-контейнер.

https://en.wikipedia.org/wiki/Pequod_(Moby-Dick)

## Интеграция

Ожидаемая структура репозитория:

```
{REPO_DIR}
└── docker
   ├── client
   ├── config
   │  └── bashrc
   ├── image
   │  └── Dockerfile
   └── docker-compose.yml
```

В этой структуре `client` – данный репозиторий `pequod`, прицепленный сабмодулем.

### `docker-compose.yml`

```yaml
version: "3.3"

services:
  wrapper:
    build:
      context: image
      dockerfile: Dockerfile
    container_name: $CONTAINER_NAME
    cap_add:
      - SYS_PTRACE
    stdin_open: true
    tty: true
    volumes:
      - $HOST_WORKSPACE_DIR:/workspace
    ports:
      - "127.0.0.1:2227:22"
```

Путь `/workspace` для контейнера менять нельзя!

## Зависимости

- `docker`
- `docker-compose`
- `python3`

## Команды

Скрипты находятся в директории [bin](/bin)

| Скрипт | Описание  |
| - | - |
| `create`   | Создать контейнер |
| `login` | Залогиниться в контейнер под текущим пользоваетелем |
| `root` | Залогиниться в контейнер под рутом |
| `restart` | Переподнять созданный контейнер |
| `remove` | Удалить контейнер |

## Благодарности

Идея и реализация - [Лев Хорошанский](https://github.com/TmLev)
