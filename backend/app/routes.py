from fastapi import APIRouter, WebSocket
from loguru import logger
from starlette.websockets import WebSocketDisconnect

from app.bot import TwitchBot
from app.twitch import get_user_info

router: APIRouter = APIRouter()


@router.websocket("/ws")
async def websocket_endpoint(access_token: str, websocket: WebSocket):
    twitch_user = await get_user_info(access_token)
    channel_name = twitch_user['login']

    logger.info(f'connection request from user {twitch_user}')

    async def _callback(message):
        if message.content == 'plus':
            await websocket.send_text('plus')
        elif message.content == 'minus':
            await websocket.send_text('minus')

    bot = TwitchBot(token=access_token, channel=channel_name, message_callback=_callback)

    try:
        await websocket.accept()
        await bot.start()
    except WebSocketDisconnect:
        logger.info(f'connection closed from user {twitch_user}')
        await bot.close()
