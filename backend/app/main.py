from fastapi import FastAPI

from app.config import Config
from app.routes import router as api_router


def get_application():
    config = Config()
    config.configure_logging()

    application = FastAPI()

    application.include_router(api_router)

    return application


app = get_application()
