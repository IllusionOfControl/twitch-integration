from typing import Callable

from twitchio.ext import commands


class TwitchBot(commands.Bot):
    def __init__(self, token, channel, message_callback: Callable):
        super().__init__(token=token, prefix="", initial_channels=[channel])

        self.message_callback: Callable = message_callback

    async def event_message(self, message):
        if message.echo:
            return

        await self.message_callback(message)

        await self.handle_commands(message)
