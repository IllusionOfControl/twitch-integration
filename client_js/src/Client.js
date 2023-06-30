import React, {useCallback, useEffect, useMemo, useRef, useState} from 'react';
import axios from 'axios';
import styled from 'styled-components'

const HelloButton = styled.button`
  display: inline-block;
  padding: 10px 20px;
  background-color: #f68350;
  color: white;
  text-decoration: none;
  font-weight: bold;
  border-radius: 4px;
  transition: background-color 0.3s ease;
  font-size: 32px;

  &:hover {
    background-color: #f3682b;
  }
`;

const Score = styled.p`
  text-align: center;
  font-size: 72px;
  color: #21620c;
`;


const clamp = (value, min, max) => {
  return Math.min(Math.max(value, min), max);
}

const { REACT_APP_CLIENT_ID, REACT_APP_BOT_URL} = process.env;

const Client = ({token}) => {
  const ws = useRef(null);
  const [score, setScore] = useState(0);
  const authToken = useMemo(() => token,[token]);
  const connectWebSocket = useCallback(() => {
    const socket = new WebSocket(`ws://${REACT_APP_BOT_URL}/ws?access_token=${token}`);

    socket.onopen = () => {
      console.log('WebSocket connection open');
    }

    socket.onmessage = (event) => {
      const command = event.data;

      switch (command) {
        case "plus":
          setScore((prevScore) => clamp(prevScore + 1, -10, 10));
          break;
        case "minus":
          setScore((prevScore) => clamp(prevScore - 1, -10, 10));
          break;
        default:
          break;
      }
    }

    socket.onclose = () => {
      console.log('WebSocket connection closed');

      setTimeout(connectWebSocket, 2000);
    };

    ws.current = socket;
  }, []);

  useEffect(() => {
    if (ws.current === null && authToken !== null) {
      connectWebSocket();

      return () => {
        ws.current.close();
      }
    }
  }, []);

  const handleHello = async () => {
    try {
      const config = {
        method: 'post',
        url: 'https://api.twitch.tv/helix/chat/announcements',
        params: {
          broadcaster_id: 214253716,
          moderator_id: 214253716,
        },
        headers: {
          'Client-ID': REACT_APP_CLIENT_ID,
          'Authorization': `Bearer ${token}`,
          'Content-Type': 'application/json',
        },
        data: {
          message: "Hello chat!",
          color: "orange"
        },
      };

      await axios(config);
    } catch (error) {
      console.error('Error during login:', error);
    }
  };

  return (
    <div>
      <HelloButton onClick={handleHello}>Say Hello!</HelloButton>
      <Score>{score}</Score>
    </div>
  );
};

export default Client;
