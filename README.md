# Backend/Client Twitch Integration

This repository contains the backend implementation for the Twitch integration project.
The project aims to develop a client application that allows users to authenticate with Twitch using OAuth, 
display a counter that ranges from -10 to +10, and interact with the Twitch chat.

## Description

The goal of this project is to create a client application that integrates with Twitch using OAuth for user authentication. The application should have the following features:

- Login Button: Allows users to authenticate with Twitch through the OAuth mechanism using a web browser.
- Counter: Displays a counter that ranges from -10 to +10. The counter value should be updated based on the chat messages received from the Twitch Stream chat.
- Chat Interaction: The backend script should read messages from the Twitch chat and interact with the client application. 
If a user in the chat sends a message containing "plus", the counter value on the client side should be incremented by 1. 
Similarly, if a user sends a message containing "minus", the counter value should be decremented by 1.
- Hello Button: Provides a button on the client side that allows users to send a "hello" message to the Twitch chat.

## Technologies Used

- Backend: Python **(FastAPI)**
- Client: JavaScript **(React)**
- Client: C++ **(Unreal Engine 5)**
- Twitch OAuth/API
- WebSockets

## Getting Started

To set up the project and run it locally, follow the instructions below:

1. Clone this repository to your local machine.
2. Install the necessary dependencies for the chosen backend language (Python or Go).
3. Configure the Twitch OAuth credentials in the backend application.
4. Build and run the backend application.
5. Set up the client application to connect to the backend server and interact with the Twitch API.
6. Launch the client application and test the functionality.

Please refer to the individual documentation in the respective backend and client repositories for detailed instructions on setting up and running each component.

- `backend` - [README.md](./backend/README.md)
- `client_js` - [README.md](./client_js/README.md)
- `client_ue5` - [README.md](./client_ue5/README.md)

## Deployment with Docker Compose

There is also an option to run using Docker Compose, follow the steps below:

1. Make sure Docker and Docker Compose are installed on your machine.

2. Clone this repository:

   ```shell
   git clone https://github.com/IllusionOfControl/twitch-integration.git
   cd twitch-integration
   ```

3. Configure the application according to their documentation.

4. Build the Docker images and start the containers:

   ```shell
   docker-compose up --build
   ```

   This command will build the Docker images for each component of the application and start the containers.

5. Access the application:

   The client application will be available at `http://localhost:3000`, and the backend API will be accessible at `http://localhost:8000`.

   Use your web browser to visit `http://localhost:3000` and start interacting with the Twitch Counter Application.

6. Stop the application:

   To stop the application and shut down the containers, press `Ctrl + C` in the terminal where `docker-compose up` is running.

For more detailed instructions on deploying each component separately or using Docker Compose, refer to the respective README files of each component.

To run the client on Unreal Engine 5, use the game engine directly.

## License

This project is licensed under the [MIT License](LICENSE).
