from fastapi import FastAPI

from app.routes import router as api_router
from app.config import Config


def get_application():
    config = Config()
    config.configure_logging()

    application = FastAPI()

    application.include_router(api_router)

    return application


app = get_application()
