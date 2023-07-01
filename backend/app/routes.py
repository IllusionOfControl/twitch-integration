from fastapi import APIRouter, WebSocket
from loguru import logger

from app.bot import TwitchBot
from app.twitch import get_user_info

router: APIRouter = APIRouter()


@router.websocket("/ws")
async def websocket_endpoint(access_token: str, websocket: WebSocket):
    try:
        await websocket.accept()

        try:
            twitch_user = await get_user_info(access_token)
        except Exception as e:
            logger.error(e)
            # await websocket.close(1000)
            return

        channel_name = twitch_user['login']

        logger.info(f'connection request from user {twitch_user}')

        bot = TwitchBot(token=access_token, channel=channel_name, websocket=websocket)

        await bot.start()
        while True:
            await websocket.receive()
    except Exception as e:
        logger.info(f'socket was closed: {e}')
