import logging
import sys
from typing import Tuple

from loguru import logger
from pydantic import BaseSettings

from app.logging import InterceptHandler


class Config(BaseSettings):
    client_id: str
    logging_level: int = logging.INFO
    loggers: Tuple[str, str] = ("uvicorn", "uvicorn.access")

    class Config:
        env_file = '.env'

    def configure_logging(self) -> None:
        logging.getLogger().handlers = [InterceptHandler()]
        for logger_name in self.loggers:
            logging_logger = logging.getLogger(logger_name)
            logging_logger.handlers = [InterceptHandler(level=self.logging_level)]

        logger.configure(handlers=[{"sink": sys.stderr, "level": self.logging_level}])
