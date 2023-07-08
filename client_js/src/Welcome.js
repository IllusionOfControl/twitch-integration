import React from 'react';
import styled from 'styled-components'

const TwitchLoginButton = styled.a`
  display: block;
  padding: 10px 20px;
  background-color: #6441a5;
  color: white;
  text-decoration: none;
  font-weight: bold;
  border-radius: 4px;
  transition: background-color 0.3s ease;
  font-size: 32px;

  &:hover {
    background-color: #8d65d3;
  }
`;

const { REACT_APP_CLIENT_ID } = process.env;

const twitchOAuthUrl = (() => {
  const baseUrl = 'https://id.twitch.tv';
  const path = '/oauth2/authorize';
  const queryParams = {
    client_id: REACT_APP_CLIENT_ID,
    redirect_uri: 'http://localhost:3000',
    response_type: 'token',
    scope: 'user:read:email chat:read moderator:manage:announcements',
  };

  const url = new URL(path, baseUrl);
  Object.keys(queryParams).forEach((key) => url.searchParams.append(key, queryParams[key]));
  return url.href;
})();

const Welcome = () => {
  return (
    <TwitchLoginButton href={twitchOAuthUrl}>Login with Twitch</TwitchLoginButton>
  );
};

export default Welcome;
