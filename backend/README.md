# Backend

1. Перейдите в каталог проекта:
   ```bash
   cd backend
   ```
2. Создайте виртуальную среду (необязательно, но рекомендуется):
   ```bash
   python -m venv venv
   source venv/bin/activate  # для Unix/Linux
   venv\Scripts\activate     # для Windows
   ```
3. Установите зависимости:
   ```bash
   pip install -r requirements.txt
   ```

## Конфигурация
1. Скопируйте `.env.example` и переименуйте `.env`.
2. Измените `.env` чтобы задать идентификатор клиента приложения Twitch.

## Запуск
Запустисте приложение:
   ```bash
   python run.py
   ```