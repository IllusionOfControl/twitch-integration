import React, {useEffect, useState} from 'react';
import Welcome from './Welcome';
import Client from './Client';
import Error from './Error';
import './style.css'
import styled from 'styled-components'

const Container = styled.div`
  background-color: #b2b2b2;
  height: 100vh;
  width: 100%;
  display: flex;
  justify-content: center;
  align-items: center;
`

const App = () => {
  const [token, setToken] = useState(null);
  const [error, setError] = useState('');
  const [description, setDescription] = useState('');

  useEffect(() => {
    const searchParams = new URLSearchParams(window.location.hash.substring(1));
    const access_token = searchParams.get('access_token');
    const error = searchParams.get('error');
    const description = searchParams.get('error_description');

    setToken(access_token);
    setError(error);
    setDescription(description);
  }, []);

  if (error) {
    return (
      <Container>
        <Error error={error} description={description}/>
      </Container>
    );
  }

  if (token) {
    return (
      <Container>
        <Client token={token}/>
      </Container>
    )
  }

  return (
    <Container>
      <Welcome/>
    </Container>
  );
};

export default App;
