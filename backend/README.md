# Backend

## Installation

1. Navigate to the project directory: 

   ```bash
   cd backend
   ```

2. Create a virtual environment (optional but recommended):

   ```bash
   python -m venv venv
   source venv/bin/activate  # for Unix/Linux
   venv\Scripts\activate     # for Windows
   ```

3. Install the dependencies:

   ```bash
   pip install -r requirements.txt
   ```

## Configuration

1. Copy `.env.example` file and rename it to `.env`.
2. Modify the `.env` file to set the necessary environment variables such as Client Id Twitch Application.

## Usage

Run the application:

   ```bash
   python run.py
   ```
