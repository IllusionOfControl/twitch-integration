import aiohttp

from app.config import Config


async def get_user_info(access_token):
    url = f"https://api.twitch.tv/helix/users"
    headers = {
        "Client-ID": Config().client_id,
        "Authorization": f"Bearer {access_token}"
    }

    async with aiohttp.ClientSession() as session:
        async with session.get(url, headers=headers) as response:
            if response.status == 200:
                data = await response.json()
                user_info = data["data"][0]
                return user_info
            else:
                raise Exception(f"Failed to retrieve user info: {response.status} - {response.reason}")
