import asyncio

from fastapi import WebSocket
from loguru import logger

from twitchio.ext import commands
from websockets.exceptions import WebSocketException


class TwitchBot(commands.Bot):
    def __init__(self, token, channel, websocket: WebSocket):
        super().__init__(token=token, prefix="", initial_channels=[channel])

        self._websocket: WebSocket = websocket

    async def event_message(self, message):
        if message.echo:
            return
        try:
            if message.content == 'plus':
                await self._websocket.send_text('plus')
            elif message.content == 'minus':
                await self._websocket.send_text('minus')

        except Exception as e:
            await self.close()
